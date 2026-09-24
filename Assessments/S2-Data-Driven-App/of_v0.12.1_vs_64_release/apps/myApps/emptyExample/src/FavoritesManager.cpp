#include "FavoritesManager.h"

// Set the location of the favorites file and load any
// previously saved countries when the application starts.
void FavoritesManager::setup(const string & relativePath) {
	filePath = ofToDataPath(relativePath, true);
	load();
}

// Check whether a country is already stored as a favorite.
// The alpha-3 country code is used because it uniquely identifies a country.
bool FavoritesManager::contains(const string & alpha3) const {
	if (alpha3.empty()) return false;

	// Compare codes without considering upper or lower case.
	for (const auto & country : favorites) {
		if (ofToLower(country.alpha3) == ofToLower(alpha3)) {
			return true;
		}
	}

	return false;
}

// Add a country to the favorites list.
// Invalid countries and duplicate entries are rejected.
bool FavoritesManager::add(const CountryData & country) {
	if (!country.valid || country.alpha3.empty()) return false;
	if (contains(country.alpha3)) return false;

	favorites.push_back(country);

	// Save immediately so the favorite remains after the app closes.
	save();

	return true;
}

// Remove a country from favorites using its alpha-3 code.
bool FavoritesManager::remove(const string & alpha3) {
	for (auto it = favorites.begin(); it != favorites.end(); ++it) {

		// Use a case-insensitive comparison for reliability.
		if (ofToLower(it->alpha3) == ofToLower(alpha3)) {
			favorites.erase(it);

			// Update the saved file after removing the country.
			save();

			return true;
		}
	}

	return false;
}

// Switch the favorite state of a country.
// If it already exists it is removed; otherwise it is added.
bool FavoritesManager::toggle(const CountryData & country) {
	if (contains(country.alpha3)) {
		return remove(country.alpha3);
	}

	return add(country);
}

// Provide read-only access to the complete favorites collection.
// Returning a reference avoids unnecessarily copying the vector.
const vector<CountryData> & FavoritesManager::getAll() const {
	return favorites;
}

// Load favorites from the local JSON file.
void FavoritesManager::load() {
	// Clear the current list before rebuilding it from the file.
	favorites.clear();

	// On first use the file may not exist yet.
	// Create an empty favorites file and continue normally.
	if (!ofFile::doesFileExist(filePath)) {
		save();
		return;
	}

	try {
		ofJson j = ofLoadJson(filePath);

		// Favorites are expected to be stored as a JSON array.
		if (!j.is_array()) return;

		// Convert each saved JSON item back into a CountryData object.
		for (const auto & item : j) {
			CountryData country = CountryData::fromJson(item);

			// Only restore valid country records.
			if (country.valid) {
				favorites.push_back(country);
			}
		}
	} catch (const std::exception & e) {
		// Log file or JSON errors without crashing the application.
		ofLogError("FavoritesManager") << e.what();
	}
}

// Save the current favorites collection to the local JSON file.
void FavoritesManager::save() const {
	try {
		// The file stores favorites as an array of country objects.
		ofJson j = ofJson::array();

		for (const auto & country : favorites) {
			// CountryData handles its own conversion into JSON.
			j.push_back(country.toJson());
		}

		// Pretty JSON keeps the local file readable during development.
		ofSavePrettyJson(filePath, j);
	} catch (const std::exception & e) {
		// Log any writing errors instead of terminating the application.
		ofLogError("FavoritesManager") << e.what();
	}
}
