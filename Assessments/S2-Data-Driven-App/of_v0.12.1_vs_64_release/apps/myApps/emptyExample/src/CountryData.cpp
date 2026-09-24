#include "CountryData.h"

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

static string joinLimited(const vector<string>& items, int maxItems) {
    if (items.empty()) return "Not available";

    string result;
    int count = std::min((int)items.size(), maxItems);

    for (int i = 0; i < count; ++i) {
        if (i > 0) result += ", ";
        result += items[i];
    }

    if ((int)items.size() > count) {
        result += " +" + ofToString((int)items.size() - count);
    }

    return result;
}

string CountryData::displayLanguages(int maxItems) const {
    return joinLimited(languages, maxItems);
}

string CountryData::displayTimezones(int maxItems) const {
    return joinLimited(timezones, maxItems);
}

string CountryData::displayBorders(int maxItems) const {
    if (borders.empty()) return "None";
    return joinLimited(borders, maxItems);
}

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

double CountryData::populationDensity() const {
    if (areaKm2 <= 0.0) return 0.0;
    return (double)population / areaKm2;
}

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

CountryData CountryData::fromJson(const ofJson& j) {
    CountryData c;

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

    if (j.contains("languages") && j["languages"].is_array()) {
        for (const auto& v : j["languages"]) if (v.is_string()) c.languages.push_back(v.get<string>());
    }

    if (j.contains("borders") && j["borders"].is_array()) {
        for (const auto& v : j["borders"]) if (v.is_string()) c.borders.push_back(v.get<string>());
    }

    if (j.contains("timezones") && j["timezones"].is_array()) {
        for (const auto& v : j["timezones"]) if (v.is_string()) c.timezones.push_back(v.get<string>());
    }

    if (j.contains("callingCodes") && j["callingCodes"].is_array()) {
        for (const auto& v : j["callingCodes"]) if (v.is_string()) c.callingCodes.push_back(v.get<string>());
    }

    c.population = j.value("population", 0LL);
    c.areaKm2 = j.value("areaKm2", 0.0);
    c.latitude = j.value("latitude", 0.0);
    c.longitude = j.value("longitude", 0.0);
    c.landlocked = j.value("landlocked", false);
    c.valid = j.value("valid", !c.commonName.empty());

    return c;
}
