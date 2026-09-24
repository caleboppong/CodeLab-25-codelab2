#pragma once

#include "ofMain.h"
#include "CountryData.h"

class FavoritesManager {
public:
    void setup(const string& relativePath = "favorites.json");

    bool add(const CountryData& country);
    bool remove(const string& alpha3);
    bool toggle(const CountryData& country);

    bool contains(const string& alpha3) const;

    const vector<CountryData>& getAll() const;

private:
    string filePath;
    vector<CountryData> favorites;

    void load();
    void save() const;
};
