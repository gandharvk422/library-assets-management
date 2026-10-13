#include "Asset.h"

std::string Asset::typeName() const {
    return type == AssetType::Book ? "Book" : "Equipment";
}
