#include "Library.h"
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

namespace {
std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return value;
}
int readInt(const std::string& prompt) {
    while (true) {
        const auto raw = readLine(prompt);
        try {
            std::size_t used = 0;
            int value = std::stoi(raw, &used);
            if (used == raw.size()) return value;
        } catch (...) {}
        std::cout << "Please enter a valid whole number.\n";
    }
}
void printAssets(const std::vector<Asset>& assets) {
    if (assets.empty()) { std::cout << "\nNo assets found.\n"; return; }
    std::cout << "\n" << std::left << std::setw(7) << "ID" << std::setw(12) << "TYPE"
              << std::setw(29) << "TITLE" << std::setw(23) << "CREATOR / BRAND"
              << std::setw(16) << "CATEGORY" << std::setw(10) << "AVAILABLE" << "LOCATION\n";
    std::cout << std::string(112, '-') << '\n';
    for (const auto& a : assets) {
        std::cout << std::left << std::setw(7) << a.id << std::setw(12) << a.typeName()
                  << std::setw(29) << a.title.substr(0, 27) << std::setw(23) << a.creator.substr(0, 21)
                  << std::setw(16) << a.category.substr(0, 14)
                  << (std::to_string(a.availableQuantity) + "/" + std::to_string(a.totalQuantity))
                  << "     " << a.location << '\n';
    }
}
void printLoans(const std::vector<Loan>& loans) {
    bool any = false;
    std::cout << "\n" << std::left << std::setw(9) << "LOAN ID" << std::setw(9) << "ASSET ID"
              << std::setw(24) << "BORROWER" << std::setw(14) << "CHECKOUT"
              << std::setw(14) << "DUE DATE" << "STATUS\n";
    std::cout << std::string(86, '-') << '\n';
    for (const auto& l : loans) {
        any = true;
        std::cout << std::left << std::setw(9) << l.loanId << std::setw(9) << l.assetId
                  << std::setw(24) << l.borrower.substr(0, 22) << std::setw(14) << l.checkoutDate
                  << std::setw(14) << l.dueDate << (l.returned ? "Returned" : "Active") << '\n';
    }
    if (!any) std::cout << "No loans recorded.\n";
}
void showMenu() {
    std::cout << "\n========== LIBRARY ASSETS MANAGEMENT ==========\n"
              << " 1. Dashboard\n 2. List all assets\n 3. Search assets\n"
              << " 4. Add asset\n 5. Check out asset\n 6. Return asset\n"
              << " 7. Loan register\n 8. Overdue report\n 9. Save data\n 0. Exit\n"
              << "==============================================\n";
}
}

int main() {
    Library library;
    std::cout << "Library Assets Management System v1.0\n";
    while (true) {
        showMenu();
        const int choice = readInt("Choose an option: ");
        std::string message;
        switch (choice) {
            case 1: {
                int totalCopies = 0, availableCopies = 0, books = 0, equipment = 0;
                for (const auto& a : library.assets()) {
                    totalCopies += a.totalQuantity; availableCopies += a.availableQuantity;
                    if (a.type == AssetType::Book) ++books; else ++equipment;
                }
                std::cout << "\n--- Dashboard ---\n"
                          << "Unique asset records : " << library.assets().size() << '\n'
                          << "Book records         : " << books << '\n'
                          << "Equipment records    : " << equipment << '\n'
                          << "Total copies/items   : " << totalCopies << '\n'
                          << "Available copies     : " << availableCopies << '\n'
                          << "Active loans          : " << library.activeLoanCount() << '\n'
                          << "Overdue loans         : " << library.overdueCount(readLine("Today's date (YYYY-MM-DD): ")) << '\n';
                break;
            }
            case 2: printAssets(library.assets()); break;
            case 3: printAssets(library.searchAssets(readLine("Search title, creator, category, type, or location: "))); break;
            case 4: {
                Asset a;
                const int type = readInt("Asset type (1 = Book, 2 = Equipment): ");
                if (type != 1 && type != 2) { std::cout << "Invalid asset type.\n"; break; }
                a.type = type == 1 ? AssetType::Book : AssetType::Equipment;
                a.title = readLine("Title / asset name: ");
                a.creator = readLine("Author / manufacturer: ");
                a.category = readLine("Category: ");
                a.location = readLine("Shelf / room location: ");
                a.totalQuantity = readInt("Quantity: ");
                if (library.addAsset(a, message))
                    std::cout << "Asset added. Assigned ID: " << library.assets().back().id << '\n';
                else std::cout << "Could not add asset: " << message << '\n';
                break;
            }
            case 5: {
                const int id = readInt("Asset ID: ");
                const auto borrower = readLine("Borrower name / ID: ");
                const auto checkout = readLine("Checkout date (YYYY-MM-DD): ");
                const auto due = readLine("Due date (YYYY-MM-DD): ");
                if (library.checkoutAsset(id, borrower, checkout, due, message)) std::cout << message << '\n';
                else std::cout << "Checkout failed: " << message << '\n';
                break;
            }
            case 6: {
                const int id = readInt("Loan ID to return: ");
                if (library.returnAsset(id, message)) std::cout << message << '\n';
                else std::cout << "Return failed: " << message << '\n';
                break;
            }
            case 7: printLoans(library.loans()); break;
            case 8: {
                const auto today = readLine("Today's date (YYYY-MM-DD): ");
                std::cout << "\n--- Overdue Loans ---\n";
                bool any = false;
                for (const auto& l : library.loans()) {
                    if (!l.returned && l.dueDate < today) {
                        any = true;
                        std::cout << "Loan " << l.loanId << " | Asset " << l.assetId
                                  << " | " << l.borrower << " | Due " << l.dueDate << '\n';
                    }
                }
                if (!any) std::cout << "No overdue loans found.\n";
                break;
            }
            case 9:
                if (library.save(message)) std::cout << "Data saved.\n";
                else std::cout << message << '\n';
                break;
            case 0:
                if (library.save(message)) std::cout << "Data saved. Goodbye!\n";
                else std::cout << "Warning: " << message << '\n';
                return 0;
            default: std::cout << "Choose a listed menu option.\n";
        }
    }
}
