#include "Library.h"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace {
std::string clean(std::string value) {
    std::replace(value.begin(), value.end(), '|', '/');
    std::replace(value.begin(), value.end(), '\n', ' ');
    std::replace(value.begin(), value.end(), '\r', ' ');
    return value;
}
std::vector<std::string> split(const std::string& line, char delimiter) {
    std::vector<std::string> out;
    std::stringstream stream(line);
    std::string item;
    while (std::getline(stream, item, delimiter)) out.push_back(item);
    return out;
}
}

Library::Library() {
    std::string ignored;
    load(ignored);
}

std::string Library::lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

int Library::nextAssetId() const {
    int next = 1001;
    for (const auto& a : assets_) next = std::max(next, a.id + 1);
    return next;
}
int Library::nextLoanId() const {
    int next = 5001;
    for (const auto& l : loans_) next = std::max(next, l.loanId + 1);
    return next;
}

bool Library::addAsset(const Asset& asset, std::string& error) {
    if (asset.title.empty() || asset.creator.empty() || asset.category.empty()) {
        error = "Title, creator/manufacturer, and category are required.";
        return false;
    }
    if (asset.totalQuantity < 1) {
        error = "Quantity must be at least 1.";
        return false;
    }
    Asset copy = asset;
    copy.id = nextAssetId();
    copy.availableQuantity = copy.totalQuantity;
    assets_.push_back(copy);
    if (!save(error)) { assets_.pop_back(); return false; }
    return true;
}

bool Library::checkoutAsset(int assetId, const std::string& borrower,
                            const std::string& checkoutDate, const std::string& dueDate,
                            std::string& message) {
    auto it = std::find_if(assets_.begin(), assets_.end(),
                           [assetId](const Asset& a) { return a.id == assetId; });
    if (it == assets_.end()) { message = "Asset ID not found."; return false; }
    if (it->availableQuantity <= 0) { message = "No copies are currently available."; return false; }
    if (borrower.empty() || checkoutDate.empty() || dueDate.empty()) {
        message = "Borrower and dates are required."; return false;
    }
    if (dueDate < checkoutDate) { message = "Due date cannot be before checkout date."; return false; }
    --it->availableQuantity;
    loans_.push_back({nextLoanId(), assetId, clean(borrower), checkoutDate, dueDate, false});
    if (!save(message)) {
        ++it->availableQuantity;
        loans_.pop_back();
        return false;
    }
    message = "Checkout recorded. Loan ID: " + std::to_string(loans_.back().loanId);
    return true;
}

bool Library::returnAsset(int loanId, std::string& message) {
    auto it = std::find_if(loans_.begin(), loans_.end(),
                           [loanId](const Loan& l) { return l.loanId == loanId; });
    if (it == loans_.end()) { message = "Loan ID not found."; return false; }
    if (it->returned) { message = "This loan has already been returned."; return false; }
    auto asset = std::find_if(assets_.begin(), assets_.end(),
                              [it](const Asset& a) { return a.id == it->assetId; });
    if (asset == assets_.end()) { message = "Linked asset record is missing."; return false; }
    it->returned = true;
    ++asset->availableQuantity;
    if (!save(message)) {
        it->returned = false;
        --asset->availableQuantity;
        return false;
    }
    message = "Return recorded successfully.";
    return true;
}

std::vector<Asset> Library::searchAssets(const std::string& query) const {
    std::vector<Asset> result;
    const auto needle = lower(query);
    for (const auto& a : assets_) {
        const std::string haystack = lower(a.title + " " + a.creator + " " + a.category +
                                           " " + a.location + " " + a.typeName());
        if (needle.empty() || haystack.find(needle) != std::string::npos) result.push_back(a);
    }
    return result;
}

int Library::activeLoanCount() const {
    return static_cast<int>(std::count_if(loans_.begin(), loans_.end(),
                                         [](const Loan& l) { return !l.returned; }));
}
int Library::overdueCount(const std::string& today) const {
    return static_cast<int>(std::count_if(loans_.begin(), loans_.end(),
        [&today](const Loan& l) { return !l.returned && l.dueDate < today; }));
}

bool Library::save(std::string& error) const {
    try {
        std::filesystem::create_directories(dataDirectory_);
        const auto assetTmp = dataDirectory_ + "/assets.tmp";
        const auto loanTmp = dataDirectory_ + "/loans.tmp";
        {
            std::ofstream out(assetTmp, std::ios::trunc);
            if (!out) { error = "Cannot write asset data file."; return false; }
            for (const auto& a : assets_)
                out << a.id << '|' << (a.type == AssetType::Book ? 'B' : 'E') << '|'
                    << clean(a.title) << '|' << clean(a.creator) << '|' << clean(a.category) << '|'
                    << clean(a.location) << '|' << a.totalQuantity << '|' << a.availableQuantity << '\n';
        }
        {
            std::ofstream out(loanTmp, std::ios::trunc);
            if (!out) { error = "Cannot write loan data file."; return false; }
            for (const auto& l : loans_)
                out << l.loanId << '|' << l.assetId << '|' << clean(l.borrower) << '|'
                    << l.checkoutDate << '|' << l.dueDate << '|' << (l.returned ? 1 : 0) << '\n';
        }
        // Windows does not replace an existing destination with rename(), so remove
        // the old snapshots first to keep this example portable across platforms.
        std::error_code ec;
        std::filesystem::remove(dataDirectory_ + "/assets.db", ec);
        ec.clear();
        std::filesystem::remove(dataDirectory_ + "/loans.db", ec);
        std::filesystem::rename(assetTmp, dataDirectory_ + "/assets.db");
        std::filesystem::rename(loanTmp, dataDirectory_ + "/loans.db");
        return true;
    } catch (const std::exception& e) {
        error = std::string("Could not save data: ") + e.what();
        return false;
    }
}

bool Library::load(std::string& error) {
    try {
        std::filesystem::create_directories(dataDirectory_);
        assets_.clear(); loans_.clear();
        std::ifstream assetsFile(dataDirectory_ + "/assets.db");
        std::string line;
        while (std::getline(assetsFile, line)) {
            auto p = split(line, '|');
            if (p.size() != 8) continue;
            try {
                Asset a;
                a.id = std::stoi(p[0]); a.type = p[1] == "B" ? AssetType::Book : AssetType::Equipment;
                a.title = p[2]; a.creator = p[3]; a.category = p[4]; a.location = p[5];
                a.totalQuantity = std::stoi(p[6]); a.availableQuantity = std::stoi(p[7]);
                if (a.totalQuantity > 0 && a.availableQuantity >= 0 &&
                    a.availableQuantity <= a.totalQuantity) assets_.push_back(a);
            } catch (...) { /* Ignore malformed records and continue loading. */ }
        }
        std::ifstream loansFile(dataDirectory_ + "/loans.db");
        while (std::getline(loansFile, line)) {
            auto p = split(line, '|');
            if (p.size() != 6) continue;
            try {
                loans_.push_back({std::stoi(p[0]), std::stoi(p[1]), p[2], p[3], p[4], p[5] == "1"});
            } catch (...) { /* Ignore malformed records and continue loading. */ }
        }
        return true;
    } catch (const std::exception& e) {
        error = std::string("Could not load data: ") + e.what();
        return false;
    }
}
