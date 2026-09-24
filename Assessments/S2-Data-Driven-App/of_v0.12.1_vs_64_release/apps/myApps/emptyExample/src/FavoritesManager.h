#pragma once

#include "ofMain.h"
#include "CountryData.h"

// Manages the user's favorite countries.
// The class keeps favorites in memory and saves them locally
// so they can be restored when GlobeLens is opened again.
class FavoritesManager {
public:
    // Set up the local favorites file and load any saved countries.
    // The default file is stored as favorites.json.
    void setup(const string& relativePath = "favorites.json");

    // Add a country to favorites.
    bool add(const CountryData& country);

    // Remove a country using its unique alpha-3 country code.
    bool remove(const string& alpha3);

    // Add or remove a country depending on its current favorite state.
    bool toggle(const CountryData& country);

    // Check whether a country is already in the favorites list.
    bool contains(const string& alpha3) const;

    // Provide read-only access to all saved favorite countries.
    const vector<CountryData>& getAll() const;

private:
    // Full path to the local JSON file used for persistent storage.
    string filePath;

    // Stores the favorite countries currently loaded in memory.
    vector<CountryData> favorites;

    // Internal file-handling functions.
    // These remain private because saving and loading are managed
    // automatically by the FavoritesManager.
    void load();
    void save() const;
};
