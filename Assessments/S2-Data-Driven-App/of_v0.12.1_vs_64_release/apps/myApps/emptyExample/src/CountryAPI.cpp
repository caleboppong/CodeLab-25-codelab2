#include "CountryAPI.h"

bool CountryAPI::setup(const string& configPath) {
    apiKey.clear();

    string fullPath = ofToDataPath(configPath, true);

    if (!ofFile::doesFileExist(fullPath)) {
        ofLogError("CountryAPI")
            << "Missing config file: " << fullPath;
        return false;
    }

    try {
        ofJson config = ofLoadJson(fullPath);

        if (config.contains("apiKey") && config["apiKey"].is_string()) {
            apiKey = config["apiKey"].get<string>();
        }
    }
    catch (const std::exception& e) {
        ofLogError("CountryAPI")
            << "Could not parse config file: " << e.what();
        return false;
    }

    if (apiKey.empty() || apiKey == "PASTE_YOUR_NEW_API_KEY_HERE") {
        ofLogWarning("CountryAPI")
            << "Add your REST Countries API key to bin/data/config/globelens.json";
        return false;
    }

    return true;
}

string CountryAPI::urlEncode(const string& text) const {
    string result;

    const char* hex = "0123456789ABCDEF";

    for (unsigned char c : text) {
        if (
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~'
        ) {
            result += (char)c;
        }
        else {
            result += '%';
            result += hex[(c >> 4) & 0x0F];
            result += hex[c & 0x0F];
        }
    }

    return result;
}

string CountryAPI::firstString(
    const ofJson& object,
    const vector<string>& keys) const {

    if (!object.is_object()) return "";

    for (const auto& key : keys) {
        if (object.contains(key) && object[key].is_string()) {
            return object[key].get<string>();
        }
    }

    return "";
}

bool CountryAPI::searchExact(
    const string& countryName,
    CountryData& result,
    string& errorMessage) {

    result.clear();
    errorMessage.clear();
    lastRawResponse.clear();

    if (countryName.empty()) {
        errorMessage = "Enter a country name.";
        return false;
    }

    if (apiKey.empty() || apiKey == "PASTE_YOUR_NEW_API_KEY_HERE") {
        errorMessage = "Add your API key to bin/data/config/globelens.json.";
        return false;
    }

    const string url =
        "https://api.restcountries.com/countries/v5/names.common/"
        + urlEncode(countryName);

    try {
        ofHttpRequest request;
        request.url = url;
        request.name = "GlobeLensCountryLookup";
        request.method = ofHttpRequest::GET;
        request.timeoutSeconds = 15;
        request.headers["Authorization"] = "Bearer " + apiKey;
        request.headers["Accept"] = "application/json";

        ofURLFileLoader loader;
        ofHttpResponse response = loader.handleRequest(request);

        lastRawResponse = response.data.getText();

        ofLogNotice("CountryAPI")
            << "HTTP status: " << response.status;

        if (response.status < 200 || response.status >= 300) {
            if (response.status == 401) errorMessage = "Invalid REST Countries API key.";
            else if (response.status == 403) errorMessage = "REST Countries denied this API key.";
            else if (response.status == 404) errorMessage = "Country not found.";
            else if (response.status == 429) errorMessage = "API rate limit reached. Try again shortly.";
            else if (response.status == -1) errorMessage = "Could not connect to REST Countries.";
            else errorMessage = "REST Countries error. HTTP " + ofToString(response.status);

            return false;
        }

        ofJson json = ofJson::parse(lastRawResponse);

        if (
            !json.contains("data") ||
            !json["data"].is_object() ||
            !json["data"].contains("objects") ||
            !json["data"]["objects"].is_array() ||
            json["data"]["objects"].empty()
        ) {
            errorMessage = "REST Countries returned no country record.";
            return false;
        }

        return parseCountryObject(
            json["data"]["objects"][0],
            result,
            errorMessage
        );
    }
    catch (const std::exception& e) {
        ofLogError("CountryAPI") << e.what();
        errorMessage = "Could not read the country service response.";
        return false;
    }
}

bool CountryAPI::parseCountryObject(
    const ofJson& country,
    CountryData& result,
    string& errorMessage) const {

    if (!country.is_object()) {
        errorMessage = "Country record had an invalid format.";
        return false;
    }

    // Names
    if (country.contains("names") && country["names"].is_object()) {
        const auto& names = country["names"];

        if (names.contains("common") && names["common"].is_string())
            result.commonName = names["common"].get<string>();

        if (names.contains("official") && names["official"].is_string())
            result.officialName = names["official"].get<string>();
    }

    // Codes
    if (country.contains("codes") && country["codes"].is_object()) {
        const auto& codes = country["codes"];

        if (codes.contains("alpha_2") && codes["alpha_2"].is_string())
            result.alpha2 = codes["alpha_2"].get<string>();

        if (codes.contains("alpha_3") && codes["alpha_3"].is_string())
            result.alpha3 = codes["alpha_3"].get<string>();
    }

    // Capital
    if (
        country.contains("capitals") &&
        country["capitals"].is_array() &&
        !country["capitals"].empty()
    ) {
        const auto& firstCapital = country["capitals"][0];

        if (firstCapital.is_object()) {
            result.capital = firstString(
                firstCapital,
                { "name", "common", "city" }
            );
        }
        else if (firstCapital.is_string()) {
            result.capital = firstCapital.get<string>();
        }
    }

    // Region / subregion
    if (country.contains("region") && country["region"].is_string())
        result.region = country["region"].get<string>();

    if (country.contains("subregion") && country["subregion"].is_string())
        result.subregion = country["subregion"].get<string>();

    // Population
    if (country.contains("population") && country["population"].is_number())
        result.population = country["population"].get<long long>();

    // Area
    if (country.contains("area")) {
        const auto& area = country["area"];

        if (area.is_number()) {
            result.areaKm2 = area.get<double>();
        }
        else if (area.is_object()) {
            if (area.contains("kilometers") && area["kilometers"].is_number())
                result.areaKm2 = area["kilometers"].get<double>();
            else if (area.contains("km2") && area["km2"].is_number())
                result.areaKm2 = area["km2"].get<double>();
        }
    }

    // Coordinates
    if (country.contains("coordinates") && country["coordinates"].is_object()) {
        const auto& coordinates = country["coordinates"];

        if (coordinates.contains("lat") && coordinates["lat"].is_number())
            result.latitude = coordinates["lat"].get<double>();

        if (coordinates.contains("lng") && coordinates["lng"].is_number())
            result.longitude = coordinates["lng"].get<double>();
    }

    // Landlocked
    if (country.contains("landlocked") && country["landlocked"].is_boolean())
        result.landlocked = country["landlocked"].get<bool>();

    // Currencies: intentionally flexible.
    if (country.contains("currencies")) {
        const auto& currencies = country["currencies"];

        if (currencies.is_object() && !currencies.empty()) {

            // Shape A: {"EUR": {"name":"Euro","symbol":"€"}}
            bool parsed = false;

            for (auto it = currencies.begin(); it != currencies.end(); ++it) {
                const string key = it.key();
                const auto& value = it.value();

                if (
                    key == "code" ||
                    key == "name" ||
                    key == "symbol" ||
                    key == "english_name" ||
                    key == "englishName"
                ) {
                    continue;
                }

                result.currencyCode = key;

                if (value.is_object()) {
                    result.currencyName = firstString(
                        value,
                        { "name", "english_name", "englishName", "label" }
                    );

                    result.currencySymbol = firstString(
                        value,
                        { "symbol", "sign" }
                    );
                }
                else if (value.is_string()) {
                    result.currencyName = value.get<string>();
                }

                parsed = true;
                break;
            }

            // Shape B: {"code":"EUR","name":"Euro","symbol":"€"}
            if (!parsed) {
                result.currencyCode = firstString(
                    currencies,
                    { "code", "iso", "iso_code" }
                );

                result.currencyName = firstString(
                    currencies,
                    { "name", "english_name", "englishName", "label" }
                );

                result.currencySymbol = firstString(
                    currencies,
                    { "symbol", "sign" }
                );
            }
        }
        else if (currencies.is_array() && !currencies.empty()) {
            const auto& firstCurrency = currencies[0];

            if (firstCurrency.is_object()) {
                result.currencyCode = firstString(
                    firstCurrency,
                    { "code", "iso", "iso_code" }
                );

                result.currencyName = firstString(
                    firstCurrency,
                    { "name", "english_name", "englishName", "label" }
                );

                result.currencySymbol = firstString(
                    firstCurrency,
                    { "symbol", "sign" }
                );
            }
            else if (firstCurrency.is_string()) {
                result.currencyCode = firstCurrency.get<string>();
            }
        }
    }

    // Languages
    if (country.contains("languages")) {
        const auto& languages = country["languages"];

        if (languages.is_array()) {
            for (const auto& lang : languages) {
                string name;

                if (lang.is_string()) {
                    name = lang.get<string>();
                }
                else if (lang.is_object()) {
                    name = firstString(
                        lang,
                        { "english_name", "englishName", "name", "label" }
                    );
                }

                if (!name.empty()) result.languages.push_back(name);
            }
        }
        else if (languages.is_object()) {
            for (auto it = languages.begin(); it != languages.end(); ++it) {
                if (it.value().is_string())
                    result.languages.push_back(it.value().get<string>());
            }
        }
    }

    // Borders
    if (country.contains("borders") && country["borders"].is_array()) {
        for (const auto& border : country["borders"]) {
            if (border.is_string()) result.borders.push_back(border.get<string>());
        }
    }

    // Time zones
    if (country.contains("timezones") && country["timezones"].is_array()) {
        for (const auto& zone : country["timezones"]) {
            if (zone.is_string()) result.timezones.push_back(zone.get<string>());
        }
    }

    // Calling codes
    if (country.contains("calling_codes") && country["calling_codes"].is_array()) {
        for (const auto& code : country["calling_codes"]) {
            if (code.is_string()) result.callingCodes.push_back(code.get<string>());
        }
    }

    if (result.commonName.empty()) {
        errorMessage = "Country response was missing a common name.";
        return false;
    }

    if (result.officialName.empty()) result.officialName = result.commonName;
    if (result.capital.empty()) result.capital = "Not available";
    if (result.region.empty()) result.region = "Not available";
    if (result.subregion.empty()) result.subregion = "Not available";

    if (result.currencyCode.empty()) result.currencyCode = "N/A";
    if (result.currencyName.empty()) result.currencyName = result.currencyCode;

    result.valid = true;
    return true;
}

const string& CountryAPI::getLastRawResponse() const {
    return lastRawResponse;
}
