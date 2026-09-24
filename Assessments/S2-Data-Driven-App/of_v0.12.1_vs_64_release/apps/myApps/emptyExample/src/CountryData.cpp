#include "CountryData.h"

// Reset all country information to its default state.
// This is used before loading new data so old country values
// are not accidentally carried into a new result.
void CountryData::clear() {
	commonName.clear();
	officialName.clear();
	alpha2.clear();
	alpha3.clear();

	capital.clear();
	region.clear();
	subregion.clear();

	currencyName.clear();
	currencyCode.clear();
	currencySymbol.clear();

	languages.clear();
	borders.clear();
	timezones.clear();
	callingCodes.clear();

	population = 0;
	areaKm2 = 0.0;
	latitude = 0.0;
	longitude = 0.0;
	landlocked = false;

	valid = false;
}

// Helper function used to turn a vector of strings into a
// readable list while limiting how many items appear on screen.
static string joinLimited(const vector<string> & items, int maxItems) {
	if (items.empty()) return "Not available";

	string result;

	// Only display up to the requested number of items.
	int count = std::min((int)items.size(), maxItems);

	for (int i = 0; i < count; ++i) {
		if (i > 0) result += ", ";
		result += items[i];
	}

	// Show how many additional items exist without making
	// the interface too crowded.
	if ((int)items.size() > count) {
		result += " +" + ofToString((int)items.size() - count);
	}

	return result;
}

// Return a shortened, readable list of languages.
string CountryData::displayLanguages(int maxItems) const {
	return joinLimited(languages, maxItems);
}

// Return a shortened, readable list of time zones.
string CountryData::displayTimezones(int maxItems) const {
	return joinLimited(timezones, maxItems);
}

// Return neighbouring country codes.
// Countries with no land borders are displayed as "None".
string CountryData::displayBorders(int maxItems) const {
	if (borders.empty()) return "None";
	return joinLimited(borders, maxItems);
}

// Format international calling codes for display.
// A plus sign is added when the API value does not already contain one.
string CountryData::displayCallingCodes() const {
	if (callingCodes.empty()) return "Not available";

	string result;

	for (size_t i = 0; i < callingCodes.size(); ++i) {
		if (i > 0) result += ", ";

		string code = callingCodes[i];

		if (!code.empty() && code[0] != '+') {
			code = "+" + code;
		}

		result += code;
	}

	return result;
}

// Calculate population density using population divided by land area.
// The area is checked first to avoid division by zero.
double CountryData::populationDensity() const {
	if (areaKm2 <= 0.0) return 0.0;

	return (double)population / areaKm2;
}

// Convert the CountryData object into JSON.
// This allows country information to be saved locally,
// for example when storing favorite countries.
ofJson CountryData::toJson() const {
	ofJson j;

	j["commonName"] = commonName;
	j["officialName"] = officialName;
	j["alpha2"] = alpha2;
	j["alpha3"] = alpha3;

	j["capital"] = capital;
	j["region"] = region;
	j["subregion"] = subregion;

	j["currencyName"] = currencyName;
	j["currencyCode"] = currencyCode;
	j["currencySymbol"] = currencySymbol;

	j["languages"] = languages;
	j["borders"] = borders;
	j["timezones"] = timezones;
	j["callingCodes"] = callingCodes;

	j["population"] = population;
	j["areaKm2"] = areaKm2;
	j["latitude"] = latitude;
	j["longitude"] = longitude;
	j["landlocked"] = landlocked;
	j["valid"] = valid;

	return j;
}

// Rebuild a CountryData object from previously saved JSON.
// Default values are used if optional properties are missing.
CountryData CountryData::fromJson(const ofJson & j) {
	CountryData c;

	// Restore basic country information.
	c.commonName = j.value("commonName", "");
	c.officialName = j.value("officialName", "");
	c.alpha2 = j.value("alpha2", "");
	c.alpha3 = j.value("alpha3", "");

	c.capital = j.value("capital", "");
	c.region = j.value("region", "");
	c.subregion = j.value("subregion", "");

	c.currencyName = j.value("currencyName", "");
	c.currencyCode = j.value("currencyCode", "");
	c.currencySymbol = j.value("currencySymbol", "");

	// Restore list-based information only when valid arrays are present.
	if (j.contains("languages") && j["languages"].is_array()) {
		for (const auto & v : j["languages"])
			if (v.is_string())
				c.languages.push_back(v.get<string>());
	}

	if (j.contains("borders") && j["borders"].is_array()) {
		for (const auto & v : j["borders"])
			if (v.is_string())
				c.borders.push_back(v.get<string>());
	}

	if (j.contains("timezones") && j["timezones"].is_array()) {
		for (const auto & v : j["timezones"])
			if (v.is_string())
				c.timezones.push_back(v.get<string>());
	}

	if (j.contains("callingCodes") && j["callingCodes"].is_array()) {
		for (const auto & v : j["callingCodes"])
			if (v.is_string())
				c.callingCodes.push_back(v.get<string>());
	}

	// Restore numerical and boolean country information.
	c.population = j.value("population", 0LL);
	c.areaKm2 = j.value("areaKm2", 0.0);
	c.latitude = j.value("latitude", 0.0);
	c.longitude = j.value("longitude", 0.0);
	c.landlocked = j.value("landlocked", false);

	// If an older saved record does not contain the valid property,
	// the presence of a country name is used as a sensible fallback.
	c.valid = j.value("valid", !c.commonName.empty());

	return c;
}
