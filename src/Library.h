#pragma once
#include "Asset.h"
#include <string>
#include <vector>

struct Loan {
    int loanId{};
    int assetId{};
    std::string borrower;
    std::string checkoutDate;
    std::string dueDate;
    bool returned{false};
};

class Library {
public:
    Library();
    bool addAsset(const Asset& asset, std::string& error);
    bool checkoutAsset(int assetId, const std::string& borrower,
                       const std::string& checkoutDate, const std::string& dueDate,
                       std::string& message);
    bool returnAsset(int loanId, std::string& message);
    std::vector<Asset> searchAssets(const std::string& query) const;
    const std::vector<Asset>& assets() const { return assets_; }
    const std::vector<Loan>& loans() const { return loans_; }
    int activeLoanCount() const;
    int overdueCount(const std::string& today) const;
    bool save(std::string& error) const;
    bool load(std::string& error);
    int nextAssetId() const;
    int nextLoanId() const;

private:
    std::vector<Asset> assets_;
    std::vector<Loan> loans_;
    std::string dataDirectory_{"data"};
    static std::string lower(std::string value);
};
