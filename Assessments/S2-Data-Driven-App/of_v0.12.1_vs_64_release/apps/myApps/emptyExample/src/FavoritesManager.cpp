#include "FavoritesManager.h"

void FavoritesManager::setup(const string& relativePath) {
    filePath = ofToDataPath(relativePath, true);
    load();
}

bool FavoritesManager::contains(const string& alpha3) const {
    if (alpha3.empty()) return false;

    for (const auto& country : favorites) {
        if (ofToLower(country.alpha3) == ofToLower(alpha3)) {
            return true;
        }
    }

    return false;
}

bool FavoritesManager::add(const CountryData& country) {
    if (!country.valid || country.alpha3.empty()) return false;
    if (contains(country.alpha3)) return false;

    favorites.push_back(country);
    save();
    return true;
}

bool FavoritesManager::remove(const string& alpha3) {
    for (auto it = favorites.begin(); it != favorites.end(); ++it) {
        if (ofToLower(it->alpha3) == ofToLower(alpha3)) {
            favorites.erase(it);
            save();
            return true;
        }
    }

    return false;
}

bool FavoritesManager::toggle(const CountryData& country) {
    if (contains(country.alpha3)) {
        return remove(country.alpha3);
    }

    return add(country);
}

const vector<CountryData>& FavoritesManager::getAll() const {
    return favorites;
}

void FavoritesManager::load() {
    favorites.clear();

    if (!ofFile::doesFileExist(filePath)) {
        save();
        return;
    }

    try {
        ofJson j = ofLoadJson(filePath);

        if (!j.is_array()) return;

        for (const auto& item : j) {
            CountryData country = CountryData::fromJson(item);

            if (country.valid) {
                favorites.push_back(country);
            }
        }
    }
    catch (const std::exception& e) {
        ofLogError("FavoritesManager") << e.what();
    }
}

void FavoritesManager::save() const {
    try {
        ofJson j = ofJson::array();

        for (const auto& country : favorites) {
            j.push_back(country.toJson());
        }

        ofSavePrettyJson(filePath, j);
    }
    catch (const std::exception& e) {
        ofLogError("FavoritesManager") << e.what();
    }
}
