#pragma once
#include <string>

enum class AssetType { Book, Equipment };

struct Asset {
    int id{};
    AssetType type{AssetType::Book};
    std::string title;
    std::string creator;       // Author for books, manufacturer for equipment
    std::string category;
    std::string location;
    int totalQuantity{1};
    int availableQuantity{1};

    std::string typeName() const;
};
