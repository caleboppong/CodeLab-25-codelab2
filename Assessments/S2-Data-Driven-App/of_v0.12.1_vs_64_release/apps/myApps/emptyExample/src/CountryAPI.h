#pragma once

#include "ofMain.h"
#include "CountryData.h"

class CountryAPI {
public:
    bool setup(const string& configPath = "config/globelens.json");

    bool searchExact(
        const string& countryName,
        CountryData& result,
        string& errorMessage
    );

    const string& getLastRawResponse() const;

private:
    string apiKey;
    string lastRawResponse;

    string urlEncode(const string& text) const;
    bool parseCountryObject(
        const ofJson& country,
        CountryData& result,
        string& errorMessage
    ) const;

    string firstString(const ofJson& object, const vector<string>& keys) const;
};
