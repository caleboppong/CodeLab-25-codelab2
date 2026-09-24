#pragma once

#include "ofMain.h"

// Stores all information about a single country.
// The same structure is used for API results, comparisons,
// favorites, map information and insights.
struct CountryData {

	// Country names and ISO identification codes.
	string commonName;
	string officialName;
	string alpha2;
	string alpha3;

	// Geographical information.
	string capital;
	string region;
	string subregion;

	// Currency information.
	string currencyName;
	string currencyCode;
	string currencySymbol;

	// Country information that can contain multiple values.
	vector<string> languages;
	vector<string> borders;
	vector<string> timezones;
	vector<string> callingCodes;

	// Numerical and geographical values.
	long long population = 0;
	double areaKm2 = 0.0;
	double latitude = 0.0;
	double longitude = 0.0;

	// Indicates whether the country has no coastline.
	bool landlocked = false;

	// Shows whether this object contains successfully loaded country data.
	bool valid = false;

	// Reset all stored information back to its default values.
	void clear();

	// Create shorter readable versions of list-based information
	// so large amounts of data do not overcrowd the interface.
	string displayLanguages(int maxItems = 3) const;
	string displayTimezones(int maxItems = 2) const;
	string displayBorders(int maxItems = 5) const;
	string displayCallingCodes() const;

	// Calculate population per square kilometre.
	double populationDensity() const;

	// Convert the object to JSON for local storage.
	ofJson toJson() const;

	// Rebuild a CountryData object from saved JSON.
	static CountryData fromJson(const ofJson & j);
};
