#pragma once

#include "ofMain.h"

struct CountryData {
    string commonName;
    string officialName;
    string alpha2;
    string alpha3;

    string capital;
    string region;
    string subregion;

    string currencyName;
    string currencyCode;
    string currencySymbol;

    vector<string> languages;
    vector<string> borders;
    vector<string> timezones;
    vector<string> callingCodes;

    long long population = 0;
    double areaKm2 = 0.0;
    double latitude = 0.0;
    double longitude = 0.0;
    bool landlocked = false;

    bool valid = false;

    void clear();

    string displayLanguages(int maxItems = 3) const;
    string displayTimezones(int maxItems = 2) const;
    string displayBorders(int maxItems = 5) const;
    string displayCallingCodes() const;

    double populationDensity() const;

    ofJson toJson() const;
    static CountryData fromJson(const ofJson& j);
};
