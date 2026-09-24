#pragma once

#include "CountryData.h"
#include "ofMain.h"

// Handles communication between GlobeLens and the REST Countries API.
// This class is responsible for loading the API configuration,
// sending country searches and converting JSON responses into CountryData.
class CountryAPI {
public:
	// Load the API key from the local configuration file.
	// A default path is provided so setup() can be called without an argument.
	bool setup(const string & configPath = "config/globelens.json");

	// Search the API for a country by name.
	// The country information is returned through 'result',
	// while any problem is returned through 'errorMessage'.
	bool searchExact(
		const string & countryName,
		CountryData & result,
		string & errorMessage);

	// Return the raw JSON response from the most recent API request.
	// This is useful when checking or debugging API responses.
	const string & getLastRawResponse() const;

private:
	// API authentication key loaded from the local configuration file.
	string apiKey;

	// Stores the most recent unprocessed response from the API.
	string lastRawResponse;

	// Convert country names into a safe format for use inside a URL.
	string urlEncode(const string & text) const;

	// Convert one country JSON object into the CountryData structure
	// used by the rest of the application.
	bool parseCountryObject(
		const ofJson & country,
		CountryData & result,
		string & errorMessage) const;

	// Find the first valid string from a list of possible JSON keys.
	// This helps the parser deal with slightly different API structures.
	string firstString(
		const ofJson & object,
		const vector<string> & keys) const;
};
