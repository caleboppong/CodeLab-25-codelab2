#include "ofApp.h"

// Initialise the GlobeLens window, fonts, data services and interactive map.
void ofApp::setup() {
	ofSetWindowTitle("GlobeLens 3.2");
	ofSetWindowShape(1440, 900);
	ofSetFrameRate(60);
	ofEnableAntiAliasing();

	// Use Windows Segoe UI fonts to keep the dashboard typography consistent.
	const string regular = "C:/Windows/Fonts/segoeui.ttf";
	const string bold = "C:/Windows/Fonts/segoeuib.ttf";

	logoFont.load(bold, 24, true, true);
	titleFont.load(bold, 31, true, true);
	headingFont.load(bold, 19, true, true);
	bodyFont.load(regular, 16, true, true);
	smallFont.load(regular, 13, true, true);
	tinyFont.load(regular, 11, true, true);

	// Initialise the API service and load locally saved favourites.
	api.setup();
	favorites.setup("favorites.json");

	updateLayout();

	// Load GeoJSON boundaries used by the interactive world map.
	worldMap.setBounds(mapBounds);
	worldMap.setup("maps/countries.geojson");

	statusMessage = "Search for a country, or click a country on the map.";
}

// Recalculate interface positions and sizes so the layout responds to window size changes.
void ofApp::updateLayout() {
	const float w = (float)ofGetWidth();
	const float h = (float)ofGetHeight();

	sidebarWidth = ofClamp(w * 0.16f, 210.0f, 250.0f);
	topBarHeight = 110.0f;
	outerMargin = ofClamp(w * 0.02f, 18.0f, 32.0f);
	cardGap = ofClamp(w * 0.012f, 12.0f, 20.0f);

	contentBounds.set(
		sidebarWidth + outerMargin,
		topBarHeight + outerMargin,
		std::max(300.0f, w - sidebarWidth - outerMargin * 2.0f),
		std::max(300.0f, h - topBarHeight - outerMargin * 2.0f));

	const float navX = 16.0f;
	const float navW = sidebarWidth - 32.0f;

	navExplore.set(navX, 150, navW, 52);
	navCompare.set(navX, 212, navW, 52);
	navFavorites.set(navX, 274, navW, 52);
	navInsights.set(navX, 336, navW, 52);

	const float rightMargin = outerMargin;
	const float buttonW = ofClamp(w * 0.095f, 115.0f, 145.0f);
	const float searchGap = 14.0f;
	const float searchW = ofClamp(w * 0.34f, 330.0f, 560.0f);

	searchButton.set(
		w - rightMargin - buttonW,
		30,
		buttonW,
		54);

	searchBox.set(
		searchButton.x - searchGap - searchW,
		30,
		searchW,
		54);

	const float contentX = contentBounds.x;
	const float contentW = contentBounds.width;

	heroRect.set(
		contentX,
		140,
		contentW,
		205);

	const float actionW = ofClamp(contentW * 0.13f, 112.0f, 140.0f);

	favoriteButton.set(
		heroRect.getRight() - actionW,
		heroRect.y + 150,
		actionW,
		42);

	compareButton.set(
		favoriteButton.x - cardGap - actionW,
		heroRect.y + 150,
		actionW,
		42);

	const float lowerY = 510.0f;
	const float availableLowerW = contentW;

	// Switch to a stacked map/details layout when horizontal space becomes limited.
	const bool stacked = availableLowerW < 760.0f;

	if (!stacked) {
		const float detailsW = ofClamp(
			availableLowerW * 0.31f,
			300.0f,
			390.0f);

		const float mapW = availableLowerW - detailsW - cardGap;

		mapPanelRect.set(
			contentX,
			lowerY,
			mapW,
			std::max(240.0f, h - lowerY - 90.0f));

		detailsRect.set(
			mapPanelRect.getRight() + cardGap,
			lowerY,
			detailsW,
			mapPanelRect.height);
	} else {
		const float lowerH = std::max(220.0f, h - lowerY - 90.0f);

		mapPanelRect.set(
			contentX,
			lowerY,
			availableLowerW,
			lowerH * 0.58f);

		detailsRect.set(
			contentX,
			mapPanelRect.getBottom() + cardGap,
			availableLowerW,
			lowerH * 0.42f - cardGap);
	}

	mapBounds = mapPanelRect;

	resetMapButton.set(
		mapBounds.getRight() - 88,
		mapBounds.y + 8,
		76,
		30);

	worldMap.setBounds(mapBounds);
}

// Rebuild the responsive layout whenever the application window is resized.
void ofApp::windowResized(int w, int h) {
	updateLayout();
}

// Update responsive positions and the map hover state once per frame.
void ofApp::update() {
	updateLayout();
	worldMap.update(ofGetMouseX(), ofGetMouseY());
}

// Draw the shared navigation and then render the currently selected page.
void ofApp::draw() {
	ofBackground(14, 20, 30);

	drawSidebar();
	drawTopBar();

	if (currentPage == PAGE_EXPLORE)
		drawExplore();
	else if (currentPage == PAGE_COMPARE)
		drawCompare();
	else if (currentPage == PAGE_FAVORITES)
		drawFavorites();
	else if (currentPage == PAGE_INSIGHTS)
		drawInsights();
}

// Draw the permanent left navigation used to move between GlobeLens pages.
void ofApp::drawSidebar() {
	ofSetColor(19, 27, 39);
	ofDrawRectangle(0, 0, sidebarWidth, ofGetHeight());

	ofSetColor(65, 135, 255);
	ofDrawCircle(43, 51, 17);

	ofSetColor(255);
	headingFont.drawString("G", 35, 59);

	ofSetColor(245, 248, 255);
	logoFont.drawString("GlobeLens", 72, 59);

	ofSetColor(100, 120, 145);
	tinyFont.drawString("WORLD INTELLIGENCE", 73, 82);

	ofSetColor(40, 50, 66);
	ofDrawRectangle(20, 110, sidebarWidth - 40, 1);

	drawNavButton(navExplore, "Explore", PAGE_EXPLORE);
	drawNavButton(navCompare, "Compare", PAGE_COMPARE);
	drawNavButton(navFavorites, "Favorites", PAGE_FAVORITES);
	drawNavButton(navInsights, "Insights", PAGE_INSIGHTS);

	ofSetColor(100, 115, 135);
	tinyFont.drawString("GLOBELENS 3.2", 24, ofGetHeight() - 62);
	tinyFont.drawString("Responsive country intelligence", 24, ofGetHeight() - 38);
}

// Draw one sidebar navigation button with selected and hover feedback.
void ofApp::drawNavButton(
	const ofRectangle & rect,
	const string & title,
	int page) {

	bool selected = currentPage == page;
	bool hovered = rect.inside(ofGetMouseX(), ofGetMouseY());

	if (selected)
		ofSetColor(48, 84, 137);
	else if (hovered)
		ofSetColor(31, 42, 58);
	else
		ofSetColor(19, 27, 39);

	ofDrawRectRounded(rect, 9);

	if (selected) {
		ofSetColor(70, 145, 255);
		ofDrawRectRounded(rect.x, rect.y + 10, 4, rect.height - 20, 2);
	}

	ofSetColor(selected ? 255 : 185);
	bodyFont.drawString(title, rect.x + 25, rect.y + 33);
}

// Draw the search field, animated text cursor, search button and status feedback.
void ofApp::drawTopBar() {
	ofSetColor(17, 23, 34);
	ofDrawRectangle(
		sidebarWidth,
		0,
		ofGetWidth() - sidebarWidth,
		topBarHeight);

	ofSetColor(
		searchFocused
			? ofColor(35, 49, 70)
			: ofColor(29, 38, 53));

	ofDrawRectRounded(searchBox, 10);

	if (searchFocused) {
		ofNoFill();
		ofSetColor(70, 140, 245);
		ofSetLineWidth(2);
		ofDrawRectRounded(searchBox, 10);
		ofFill();
	}

	if (searchText.empty()) {
		ofSetColor(120, 135, 155);
		bodyFont.drawString(
			"Search a country...",
			searchBox.x + 20,
			searchBox.y + 34);
	} else {
		ofSetColor(240, 245, 255);
		bodyFont.drawString(
			searchText,
			searchBox.x + 20,
			searchBox.y + 34);
	}

	if (searchFocused && ((ofGetElapsedTimeMillis() / 500) % 2 == 0)) {

		float width = searchText.empty()
			? 0.0f
			: bodyFont.stringWidth(searchText);

		ofSetColor(255);

		ofDrawRectangle(
			searchBox.x + 20 + width + 3,
			searchBox.y + 14,
			2,
			28);
	}

	bool searchHover = searchButton.inside(
		ofGetMouseX(),
		ofGetMouseY());

	ofSetColor(
		searchHover
			? ofColor(83, 151, 255)
			: ofColor(60, 125, 225));

	ofDrawRectRounded(searchButton, 10);

	const string buttonText = searching ? "WAIT..." : "SEARCH";

	float textW = bodyFont.stringWidth(buttonText);

	ofSetColor(255);

	bodyFont.drawString(
		buttonText,
		searchButton.x
			+ (searchButton.width - textW) * 0.5f,
		searchButton.y + 34);

	ofSetColor(
		searchError
			? ofColor(255, 110, 115)
			: ofColor(115, 140, 170));

	smallFont.drawString(
		statusMessage,
		searchBox.x,
		101);
}

// Validate the search, resolve common aliases, request API data and update the current country.
void ofApp::performSearch(const string & countryName) {
	if (searching) return;

	string cleaned = trimSearchText(countryName);

	if (cleaned.empty()) {
		searchError = true;
		statusMessage = "Enter a country name.";
		return;
	}

	string resolved = resolveCountryAlias(cleaned);

	searching = true;
	searchError = false;

	if (ofToLower(resolved) != ofToLower(cleaned)) {
		statusMessage = "Recognised \""
			+ cleaned
			+ "\" as "
			+ resolved
			+ ". Loading...";
	} else {
		statusMessage = "Loading country data...";
	}

	// Store the API result separately so a failed search does not replace valid data already on screen.
	CountryData result;
	string error;

	bool ok = api.searchExact(
		resolved,
		result,
		error);

	searching = false;

	if (!ok) {
		searchError = true;

		if (error == "Country not found." || error == "REST Countries returned no country record.") {
			statusMessage = "No country matched \""
				+ cleaned
				+ "\". Try the full country name.";
		} else {
			statusMessage = error;
		}

		return;
	}

	currentCountry = result;
	searchText = currentCountry.commonName;

	statusMessage = "Live data loaded for "
		+ currentCountry.commonName
		+ ".";

	// Keep recent searches unique. If a country is searched again,
	// move it to the end rather than creating a duplicate chip.
	for (auto it = recentSearches.begin();
		it != recentSearches.end();) {

		if (ofToLower(*it) == ofToLower(currentCountry.commonName)) {
			it = recentSearches.erase(it);
		} else {
			++it;
		}
	}

	recentSearches.push_back(
		currentCountry.commonName);

	while (recentSearches.size() > 6) {
		recentSearches.erase(
			recentSearches.begin());
	}

	// Centre and scale the map around the newly loaded country.
	worldMap.focusCountry(
		currentCountry.alpha3);

	loadFlag(currentCountry);

	currentPage = PAGE_EXPLORE;
}

// Remove unnecessary whitespace and collapse repeated spaces before sending a search.
string ofApp::trimSearchText(
	const string & text) const {

	if (text.empty()) return "";

	size_t first = text.find_first_not_of(
		" \t\r\n");

	if (first == string::npos) {
		return "";
	}

	size_t last = text.find_last_not_of(
		" \t\r\n");

	string result = text.substr(
		first,
		last - first + 1);

	// Collapse repeated spaces inside the search.
	string collapsed;
	bool previousSpace = false;

	for (char c : result) {
		bool isSpace = c == ' ' || c == '\t';

		if (isSpace) {
			if (!previousSpace) {
				collapsed += ' ';
			}
		} else {
			collapsed += c;
		}

		previousSpace = isSpace;
	}

	return collapsed;
}

// Convert common country abbreviations into names that the API can search reliably.
string ofApp::resolveCountryAlias(
	const string & text) const {

	string key = ofToLower(
		trimSearchText(text));

	// Friendly abbreviations that users commonly type.
	if (key == "uae" || key == "u.a.e.") {
		return "United Arab Emirates";
	}

	if (key == "uk" || key == "u.k." || key == "gb" || key == "gbr") {
		return "United Kingdom";
	}

	if (key == "usa" || key == "u.s.a." || key == "us" || key == "u.s.") {
		return "United States";
	}

	if (key == "south korea" || key == "republic of korea") {
		return "South Korea";
	}

	if (key == "north korea" || key == "dprk") {
		return "North Korea";
	}

	if (key == "russia" || key == "russian federation") {
		return "Russia";
	}

	return trimSearchText(text);
}

// Download the selected country flag using its two-letter country code.
void ofApp::loadFlag(const CountryData & country) {
	currentFlag.clear();
	flagLoaded = false;

	if (country.alpha2.empty()) return;

	string code = ofToLower(country.alpha2);

	string url = "https://flags.restcountries.com/v5/w320/"
		+ code
		+ ".png";

	try {
		ofHttpResponse response = ofLoadURL(url);

		if (
			response.status >= 200 && response.status < 300) {
			flagLoaded = ofLoadImage(
				currentFlag,
				response.data);
		}
	} catch (...) {
		flagLoaded = false;
	}
}

// Draw the Explore page, including the country profile, metrics, map and details.
void ofApp::drawExplore() {
	if (!currentCountry.valid) {
		ofSetColor(245, 248, 255);
		titleFont.drawString(
			"Explore the world",
			contentBounds.x,
			170);

		ofSetColor(135, 150, 172);
		bodyFont.drawString(
			"Search above or click a country directly on the interactive map.",
			contentBounds.x,
			207);

		mapBounds.set(
			contentBounds.x,
			255,
			contentBounds.width,
			std::max(
				300.0f,
				ofGetHeight() - 255.0f - 90.0f));

		worldMap.setBounds(mapBounds);

		worldMap.draw(
			"",
			smallFont,
			tinyFont);

		drawRecentSearches();
		return;
	}

	worldMap.setBounds(mapBounds);

	drawCountryHero();
	drawMetricCards();

	ofSetColor(235, 241, 250);

	headingFont.drawString(
		"Interactive World Map",
		mapBounds.x,
		mapBounds.y - 18);

	worldMap.draw(
		currentCountry.alpha3,
		smallFont,
		tinyFont);

	bool resetHover = resetMapButton.inside(
		ofGetMouseX(),
		ofGetMouseY());

	ofSetColor(
		resetHover
			? ofColor(56, 80, 110)
			: ofColor(38, 52, 72));

	ofDrawRectRounded(
		resetMapButton,
		7);

	ofSetColor(220);

	tinyFont.drawString(
		"RESET",
		resetMapButton.x + 18,
		resetMapButton.y + 20);

	drawDetailsCard();
	drawRecentSearches();
}

// Draw the main country profile card and its Compare and Save actions.
void ofApp::drawCountryHero() {
	float x = heroRect.x;
	float y = heroRect.y;
	float w = heroRect.width;
	float h = heroRect.height;

	ofSetColor(24, 33, 47);
	ofDrawRectRounded(heroRect, 16);

	ofSetColor(63, 137, 245);
	ofDrawRectRounded(
		x,
		y,
		8,
		h,
		4);

	float textX = x + 35;

	if (flagLoaded) {
		float fw = 92;
		float fh = 58;

		ofSetColor(255);

		currentFlag.draw(
			textX,
			y + 26,
			fw,
			fh);

		textX += 118;
	}

	ofSetColor(125, 145, 170);
	smallFont.drawString(
		"COUNTRY PROFILE",
		textX,
		y + 38);

	ofSetColor(245, 249, 255);
	titleFont.drawString(
		currentCountry.commonName,
		textX,
		y + 84);

	ofSetColor(145, 162, 185);
	bodyFont.drawString(
		currentCountry.officialName,
		textX,
		y + 116);

	ofSetColor(95, 170, 255);
	bodyFont.drawString(
		currentCountry.capital
			+ "  /  "
			+ currentCountry.region,
		textX,
		y + 158);

	ofSetColor(38, 53, 73);
	ofDrawRectRounded(
		textX,
		y + 173,
		80,
		24,
		6);

	ofSetColor(180, 200, 225);

	tinyFont.drawString(
		currentCountry.alpha3.empty()
			? "---"
			: currentCountry.alpha3,
		textX + 18,
		y + 190);

	bool compareHover = compareButton.inside(
		ofGetMouseX(),
		ofGetMouseY());

	ofSetColor(
		compareHover
			? ofColor(65, 133, 235)
			: ofColor(42, 64, 92));

	ofDrawRectRounded(
		compareButton,
		8);

	ofSetColor(240);
	smallFont.drawString(
		"Compare",
		compareButton.x + 30,
		compareButton.y + 27);

	bool saved = favorites.contains(
		currentCountry.alpha3);

	bool favoriteHover = favoriteButton.inside(
		ofGetMouseX(),
		ofGetMouseY());

	if (saved) {
		ofSetColor(
			favoriteHover
				? ofColor(89, 153, 238)
				: ofColor(55, 112, 190));
	} else {
		ofSetColor(
			favoriteHover
				? ofColor(65, 133, 235)
				: ofColor(42, 64, 92));
	}

	ofDrawRectRounded(
		favoriteButton,
		8);

	ofSetColor(240);
	smallFont.drawString(
		saved ? "Saved" : "Save",
		favoriteButton.x
			+ (saved ? 38 : 43),
		favoriteButton.y + 27);
}

// Display the four headline statistics for the selected country.
void ofApp::drawMetricCards() {
	const float y = 365.0f;
	const float h = 105.0f;
	const float totalW = contentBounds.width;

	const int count = 4;
	const float w = (totalW - cardGap * (count - 1))
		/ (float)count;

	const float x = contentBounds.x;

	drawMetricCard(
		x,
		y,
		w,
		h,
		"POPULATION",
		formatNumber(
			currentCountry.population));

	drawMetricCard(
		x + (w + cardGap),
		y,
		w,
		h,
		"AREA",
		formatNumber(
			(long long)currentCountry.areaKm2)
			+ " km2");

	drawMetricCard(
		x + (w + cardGap) * 2,
		y,
		w,
		h,
		"CURRENCY",
		currentCountry.currencyCode);

	drawMetricCard(
		x + (w + cardGap) * 3,
		y,
		w,
		h,
		"DENSITY",
		formatDouble(
			currentCountry.populationDensity(),
			1)
			+ " / km2");
}

// Draw a reusable statistic card and shorten values that are too wide to fit.
void ofApp::drawMetricCard(
	float x,
	float y,
	float w,
	float h,
	const string & label,
	const string & value) {

	ofRectangle r(
		x,
		y,
		w,
		h);

	ofSetColor(
		r.inside(
			ofGetMouseX(),
			ofGetMouseY())
			? ofColor(31, 43, 60)
			: ofColor(23, 31, 44));

	ofDrawRectRounded(r, 12);

	ofSetColor(105, 125, 150);

	tinyFont.drawString(
		label,
		x + 20,
		y + 30);

	ofSetColor(235, 241, 250);

	string visibleValue = value;

	if (
		headingFont.stringWidth(visibleValue)
		> w - 35) {
		if (visibleValue.size() > 14) {
			visibleValue = visibleValue.substr(0, 13)
				+ "...";
		}
	}

	headingFont.drawString(
		visibleValue,
		x + 20,
		y + 70);
}

// Draw additional country information in a compact key/value list.
void ofApp::drawDetailsCard() {
	const float x = detailsRect.x;
	const float y = detailsRect.y;
	const float w = detailsRect.width;
	const float h = detailsRect.height;

	ofSetColor(23, 31, 44);
	ofDrawRectRounded(
		detailsRect,
		14);

	ofSetColor(230, 237, 248);

	headingFont.drawString(
		"Country Details",
		x + 25,
		y + 38);

	vector<pair<string, string>> rows = {
		{ "Capital", currentCountry.capital },
		{ "Subregion", currentCountry.subregion },
		{ "Currency", currentCountry.currencyName + " (" + currentCountry.currencyCode + ")" },
		{ "Languages", currentCountry.displayLanguages(2) },
		{ "Calling code", currentCountry.displayCallingCodes() },
		{ "Time zones", currentCountry.displayTimezones(1) },
		{ "Borders", currentCountry.displayBorders(3) },
		{ "Landlocked", currentCountry.landlocked ? "Yes" : "No" }
	};

	float rowY = y + 74;

	float gap = std::max(
		22.0f,
		std::min(
			29.0f,
			(h - 95.0f)
				/ (float)rows.size()));

	float labelW = std::min(
		125.0f,
		w * 0.34f);

	for (size_t i = 0; i < rows.size(); ++i) {
		ofSetColor(110, 130, 155);

		smallFont.drawString(
			rows[i].first,
			x + 25,
			rowY + gap * i);

		string value = rows[i].second;

		float available = w - labelW - 60.0f;

		while (
			value.size() > 4 && smallFont.stringWidth(value) > available) {
			value.pop_back();
		}

		if (
			value != rows[i].second && value.size() > 3) {
			value = value.substr(
						0,
						value.size() - 3)
				+ "...";
		}

		ofSetColor(225, 232, 243);

		smallFont.drawString(
			value,
			x + 25 + labelW,
			rowY + gap * i);
	}
}

// Draw recent-search chips so previously viewed countries can be reopened quickly.
void ofApp::drawRecentSearches() {
	recentButtons.clear();

	if (recentSearches.empty()) return;

	float y = ofGetHeight() - 34.0f;

	ofSetColor(115, 135, 160);

	tinyFont.drawString(
		"RECENT",
		contentBounds.x,
		y);

	float x = contentBounds.x + 75.0f;

	const float rightEdge = contentBounds.getRight();

	for (const auto & name : recentSearches) {
		float w = smallFont.stringWidth(name)
			+ 30;

		if (x + w > rightEdge) break;

		ofRectangle chip(
			x,
			y - 21,
			w,
			31);

		recentButtons.push_back(chip);

		ofSetColor(
			chip.inside(
				ofGetMouseX(),
				ofGetMouseY())
				? ofColor(48, 67, 91)
				: ofColor(30, 41, 57));

		ofDrawRectRounded(
			chip,
			7);

		ofSetColor(195, 210, 230);

		smallFont.drawString(
			name,
			x + 15,
			y);

		x += w + 10;
	}
}

// Draw one side of the country comparison using the same reusable card layout.
void ofApp::drawCompareCountryCard(
	const CountryData & c,
	float x,
	float y,
	float w,
	const string & slotName) {

	float cardH = std::min(
		430.0f,
		ofGetHeight() - y - 180.0f);

	ofSetColor(23, 31, 44);
	ofDrawRectRounded(
		x,
		y,
		w,
		cardH,
		14);

	ofSetColor(105, 125, 150);
	tinyFont.drawString(
		slotName,
		x + 25,
		y + 31);

	if (!c.valid) {
		ofSetColor(235);
		headingFont.drawString(
			"No country selected",
			x + 25,
			y + 85);

		ofSetColor(125, 145, 170);
		smallFont.drawString(
			"Open Explore, search for a country,\nthen press Compare.",
			x + 25,
			y + 120);

		return;
	}

	ofSetColor(245);
	titleFont.drawString(
		c.commonName,
		x + 25,
		y + 82);

	ofSetColor(125, 145, 170);
	bodyFont.drawString(
		c.region
			+ " / "
			+ c.subregion,
		x + 25,
		y + 115);

	vector<pair<string, string>> rows = {
		{ "Capital", c.capital },
		{ "Population", formatNumber(c.population) },
		{ "Area", formatNumber((long long)c.areaKm2) + " km2" },
		{ "Density", formatDouble(c.populationDensity(), 1) + " / km2" },
		{ "Currency", c.currencyCode },
		{ "Languages", c.displayLanguages(2) },
		{ "Borders", c.displayBorders(3) }
	};

	float rowY = y + 165;
	float gap = 36;

	for (size_t i = 0; i < rows.size(); ++i) {
		ofSetColor(105, 125, 150);
		smallFont.drawString(
			rows[i].first,
			x + 25,
			rowY + gap * i);

		ofSetColor(235);

		string value = rows[i].second;

		while (
			value.size() > 4 && smallFont.stringWidth(value) > w - 190) {
			value.pop_back();
		}

		smallFont.drawString(
			value,
			x + 155,
			rowY + gap * i);
	}
}

// Draw two selected countries and calculate simple comparison insights.
void ofApp::drawCompare() {
	ofSetColor(245, 248, 255);

	titleFont.drawString(
		"Compare Countries",
		contentBounds.x,
		170);

	ofSetColor(135, 150, 172);

	bodyFont.drawString(
		"Add two different countries using the Compare button on Explore.",
		contentBounds.x,
		207);

	clearCompareButton.set(
		contentBounds.getRight() - 125,
		157,
		125,
		38);

	bool clearHover = clearCompareButton.inside(
		ofGetMouseX(),
		ofGetMouseY());

	ofSetColor(
		clearHover
			? ofColor(68, 82, 103)
			: ofColor(39, 50, 67));

	ofDrawRectRounded(
		clearCompareButton,
		8);

	ofSetColor(220);

	smallFont.drawString(
		"CLEAR",
		clearCompareButton.x + 39,
		clearCompareButton.y + 25);

	const float x = contentBounds.x;
	const float y = 255.0f;
	const float totalW = contentBounds.width;
	const float cardW = (totalW - cardGap) * 0.5f;

	drawCompareCountryCard(
		compareA,
		x,
		y,
		cardW,
		"COUNTRY A");

	drawCompareCountryCard(
		compareB,
		x + cardW + cardGap,
		y,
		cardW,
		"COUNTRY B");

	if (
		compareA.valid && compareB.valid) {
		float insightY = std::min(
			ofGetHeight() - 145.0f,
			y + 455.0f);

		ofSetColor(23, 31, 44);

		ofDrawRectRounded(
			x,
			insightY,
			totalW,
			120,
			14);

		const CountryData * largerPopulation = compareA.population >= compareB.population
			? &compareA
			: &compareB;

		const CountryData * smallerPopulation = largerPopulation == &compareA
			? &compareB
			: &compareA;

		double populationRatio = smallerPopulation->population > 0
			? (double)largerPopulation->population
				/ (double)smallerPopulation->population
			: 0.0;

		const CountryData * largerArea = compareA.areaKm2 >= compareB.areaKm2
			? &compareA
			: &compareB;

		const CountryData * denser = compareA.populationDensity()
				>= compareB.populationDensity()
			? &compareA
			: &compareB;

		string populationMessage = largerPopulation->commonName
			+ " has "
			+ formatDouble(populationRatio, 2)
			+ "x the population of "
			+ smallerPopulation->commonName
			+ ".";

		string areaMessage = largerArea->commonName
			+ " has the larger land area.";

		string densityMessage = denser->commonName
			+ " has the higher population density.";

		ofSetColor(105, 175, 150);

		headingFont.drawString(
			"Comparison insights",
			x + 25,
			insightY + 36);

		ofSetColor(225);

		smallFont.drawString(
			populationMessage,
			x + 25,
			insightY + 66);

		smallFont.drawString(
			areaMessage
				+ "  "
				+ densityMessage,
			x + 25,
			insightY + 94);
	}
}

// Draw countries saved locally and provide controls to open or remove each one.
void ofApp::drawFavorites() {
	favoriteCards.clear();
	favoriteRemoveButtons.clear();

	ofSetColor(245, 248, 255);

	titleFont.drawString(
		"Saved Countries",
		contentBounds.x,
		170);

	ofSetColor(135, 150, 172);

	bodyFont.drawString(
		"Favorites are saved locally. Open a card or remove it from your collection.",
		contentBounds.x,
		207);

	const auto & items = favorites.getAll();

	if (items.empty()) {
		ofSetColor(23, 31, 44);

		ofDrawRectRounded(
			contentBounds.x,
			255,
			contentBounds.width,
			220,
			16);

		ofSetColor(220);

		headingFont.drawString(
			"No favorites yet",
			contentBounds.x + 40,
			325);

		ofSetColor(125, 145, 170);

		bodyFont.drawString(
			"Open a country in Explore and press Save.",
			contentBounds.x + 40,
			365);

		return;
	}

	int columns = contentBounds.width >= 950
		? 3
		: (contentBounds.width >= 620 ? 2 : 1);

	const float gapX = cardGap;
	const float gapY = 22;
	const float cardH = 155;

	float cardW = (contentBounds.width
					  - gapX * (columns - 1))
		/ (float)columns;

	for (size_t i = 0; i < items.size(); ++i) {
		int col = (int)i % columns;

		int row = (int)i / columns;

		float x = contentBounds.x
			+ col * (cardW + gapX);

		float y = 255
			+ row * (cardH + gapY);

		ofRectangle card(
			x,
			y,
			cardW,
			cardH);

		ofRectangle removeButton(
			card.getRight() - 82,
			card.y + 16,
			64,
			28);

		favoriteCards.push_back(card);
		favoriteRemoveButtons.push_back(
			removeButton);

		ofSetColor(
			card.inside(
				ofGetMouseX(),
				ofGetMouseY())
				? ofColor(31, 43, 60)
				: ofColor(23, 31, 44));

		ofDrawRectRounded(card, 12);

		ofSetColor(245);

		headingFont.drawString(
			items[i].commonName,
			x + 22,
			y + 38);

		ofSetColor(105, 125, 150);

		smallFont.drawString(
			items[i].region,
			x + 22,
			y + 66);

		ofSetColor(195, 210, 230);

		smallFont.drawString(
			shortNumber(
				items[i].population)
				+ " people   "
				+ items[i].currencyCode,
			x + 22,
			y + 102);

		ofSetColor(95, 170, 255);

		tinyFont.drawString(
			"OPEN",
			x + 22,
			y + 132);

		bool removeHover = removeButton.inside(
			ofGetMouseX(),
			ofGetMouseY());

		ofSetColor(
			removeHover
				? ofColor(126, 65, 72)
				: ofColor(64, 48, 57));

		ofDrawRectRounded(
			removeButton,
			7);

		ofSetColor(235, 205, 210);

		tinyFont.drawString(
			"REMOVE",
			removeButton.x + 8,
			removeButton.y + 19);
	}
}

// Visualise population or population density for saved/current countries using proportional bars.
void ofApp::drawInsights() {
	ofSetColor(245, 248, 255);

	titleFont.drawString(
		"Insights",
		contentBounds.x,
		170);

	ofSetColor(135, 150, 172);

	bodyFont.drawString(
		"Compare population or population density across your saved countries.",
		contentBounds.x,
		207);

	insightPopulationButton.set(
		contentBounds.getRight() - 250,
		158,
		112,
		36);

	insightDensityButton.set(
		contentBounds.getRight() - 126,
		158,
		126,
		36);

	auto drawInsightToggle =
		[&](const ofRectangle & rect,
			const string & text,
			bool selected) {
			bool hovered = rect.inside(
				ofGetMouseX(),
				ofGetMouseY());

			if (selected) {
				ofSetColor(
					hovered
						? ofColor(76, 149, 245)
						: ofColor(61, 128, 222));
			} else {
				ofSetColor(
					hovered
						? ofColor(48, 63, 83)
						: ofColor(34, 45, 61));
			}

			ofDrawRectRounded(
				rect,
				8);

			ofSetColor(235);

			float textW = smallFont.stringWidth(text);

			smallFont.drawString(
				text,
				rect.x
					+ (rect.width - textW) * 0.5f,
				rect.y + 24);
		};

	drawInsightToggle(
		insightPopulationButton,
		"Population",
		insightMetric == 0);

	drawInsightToggle(
		insightDensityButton,
		"Density",
		insightMetric == 1);

	// Start with saved countries, then include the currently open country if it is not already present.
	vector<CountryData> items = favorites.getAll();

	if (currentCountry.valid) {
		bool exists = false;

		for (const auto & c : items) {
			if (
				ofToLower(c.alpha3)
				== ofToLower(
					currentCountry.alpha3)) {
				exists = true;
				break;
			}
		}

		if (!exists) {
			items.push_back(
				currentCountry);
		}
	}

	if (items.empty()) {
		ofSetColor(23, 31, 44);

		ofDrawRectRounded(
			contentBounds.x,
			255,
			contentBounds.width,
			230,
			14);

		ofSetColor(225);

		headingFont.drawString(
			"Nothing to chart yet",
			contentBounds.x + 40,
			325);

		ofSetColor(125, 145, 170);

		bodyFont.drawString(
			"Save countries or open one in Explore.",
			contentBounds.x + 40,
			365);

		return;
	}

	// Find the largest value so every chart bar can be scaled proportionally.
	double maxValue = 1.0;

	for (const auto & c : items) {
		double value = insightMetric == 0
			? (double)c.population
			: c.populationDensity();

		maxValue = std::max(
			maxValue,
			value);
	}

	float x = contentBounds.x;
	float y = 255;
	float w = contentBounds.width;
	float h = std::max(
		350.0f,
		ofGetHeight() - 320.0f);

	ofSetColor(23, 31, 44);

	ofDrawRectRounded(
		x,
		y,
		w,
		h,
		14);

	ofSetColor(235);

	headingFont.drawString(
		insightMetric == 0
			? "Population comparison"
			: "Population density comparison",
		x + 25,
		y + 40);

	int visible = std::min(
		(int)items.size(),
		10);

	float labelX = x + 25;

	float barX = x + std::min(190.0f, w * 0.22f);

	float barMaxW = w - (barX - x) - 40;

	float startY = y + 90;

	float rowGap = std::max(
		30.0f,
		std::min(
			42.0f,
			(h - 120.0f)
				/ std::max(1, visible)));

	for (int i = 0; i < visible; ++i) {
		const auto & c = items[i];

		double value = insightMetric == 0
			? (double)c.population
			: c.populationDensity();

		float ratio = maxValue > 0.0
			? (float)(value / maxValue)
			: 0.0f;

		float barW = std::max(
			2.0f,
			barMaxW * ratio);

		ofSetColor(175, 190, 210);

		smallFont.drawString(
			c.commonName,
			labelX,
			startY
				+ i * rowGap
				+ 17);

		ofSetColor(38, 52, 72);

		ofDrawRectRounded(
			barX,
			startY + i * rowGap,
			barMaxW,
			24,
			6);

		ofSetColor(67, 139, 241);

		ofDrawRectRounded(
			barX,
			startY + i * rowGap,
			barW,
			24,
			6);

		ofSetColor(235);

		string valueText = insightMetric == 0
			? shortNumber(c.population)
			: formatDouble(
				  c.populationDensity(),
				  1)
				+ " / km2";

		tinyFont.drawString(
			valueText,
			barX + 10,
			startY
				+ i * rowGap
				+ 17);
	}
}

// Format large whole numbers with comma separators for readability.
string ofApp::formatNumber(
	long long value) const {

	string number = ofToString(value);

	string result;
	int count = 0;

	for (
		int i = (int)number.size() - 1;
		i >= 0;
		--i) {
		result = number[i]
			+ result;

		count++;

		if (
			count % 3 == 0 && i != 0) {
			result = ","
				+ result;
		}
	}

	return result;
}

// Convert large values to compact K, M or B labels for cards and charts.
string ofApp::shortNumber(
	long long value) const {

	if (value >= 1000000000LL) {
		return formatDouble(
				   (double)value / 1000000000.0,
				   1)
			+ "B";
	}

	if (value >= 1000000LL) {
		return formatDouble(
				   (double)value / 1000000.0,
				   1)
			+ "M";
	}

	if (value >= 1000LL) {
		return formatDouble(
				   (double)value / 1000.0,
				   1)
			+ "K";
	}

	return ofToString(value);
}

// Format a decimal value using the requested number of decimal places.
string ofApp::formatDouble(
	double value,
	int decimals) const {

	return ofToString(
		value,
		decimals);
}

// Check whether a mouse position is inside the current map panel.
bool ofApp::isInsideMap(
	int x,
	int y) const {

	return mapBounds.inside(
		x,
		y);
}

// Handle typing, Enter, Backspace and Escape while the search box has focus.
void ofApp::keyPressed(int key) {
	if (!searchFocused) return;

	if (key == OF_KEY_RETURN) {
		performSearch(searchText);
		return;
	}

	if (key == OF_KEY_BACKSPACE) {
		if (!searchText.empty()) {
			searchText.pop_back();
		}

		return;
	}

	if (key == OF_KEY_ESC) {
		searchText.clear();
		searchError = false;
		statusMessage = "Search cleared.";
		return;
	}

	if (
		key >= 32 && key <= 126 && searchText.size() < 50) {
		searchText += (char)key;
	}
}

// Required openFrameworks keyboard callback; no release behaviour is needed here.
void ofApp::keyReleased(int key) {
}

// Route mouse clicks to navigation, search, compare, favorites, insights and map controls.
void ofApp::mousePressed(
	int x,
	int y,
	int button) {

	// Sidebar navigation is checked first so page changes respond immediately.
	if (navExplore.inside(x, y)) {
		currentPage = PAGE_EXPLORE;
		return;
	}

	if (navCompare.inside(x, y)) {
		currentPage = PAGE_COMPARE;
		return;
	}

	if (navFavorites.inside(x, y)) {
		currentPage = PAGE_FAVORITES;
		return;
	}

	if (navInsights.inside(x, y)) {
		currentPage = PAGE_INSIGHTS;
		return;
	}

	if (
		currentPage == PAGE_COMPARE && clearCompareButton.inside(x, y)) {
		compareA.clear();
		compareB.clear();

		statusMessage = "Comparison cleared.";

		return;
	}

	if (
		currentPage == PAGE_INSIGHTS && insightPopulationButton.inside(x, y)) {
		insightMetric = 0;
		statusMessage = "Insights showing population.";
		return;
	}

	if (
		currentPage == PAGE_INSIGHTS && insightDensityButton.inside(x, y)) {
		insightMetric = 1;
		statusMessage = "Insights showing population density.";
		return;
	}

	if (searchBox.inside(x, y)) {
		searchFocused = true;
		return;
	}

	if (searchButton.inside(x, y)) {
		searchFocused = true;
		performSearch(searchText);
		return;
	}

	if (
		currentPage == PAGE_EXPLORE && currentCountry.valid && compareButton.inside(x, y)) {
		bool sameAsA = compareA.valid && ofToLower(compareA.alpha3) == ofToLower(currentCountry.alpha3);

		bool sameAsB = compareB.valid && ofToLower(compareB.alpha3) == ofToLower(currentCountry.alpha3);

		if (sameAsA || sameAsB) {
			statusMessage = currentCountry.commonName
				+ " is already in the comparison.";
		} else if (!compareA.valid) {
			compareA = currentCountry;

			statusMessage = currentCountry.commonName
				+ " added as Country A. Add another country.";
		} else if (!compareB.valid) {
			compareB = currentCountry;

			statusMessage = currentCountry.commonName
				+ " added as Country B.";
		} else {
			// Keep Country A and replace Country B so the workflow
			// remains predictable after two countries are selected.
			compareB = currentCountry;

			statusMessage = "Country B replaced with "
				+ currentCountry.commonName
				+ ".";
		}

		currentPage = PAGE_COMPARE;

		return;
	}

	if (
		currentPage == PAGE_EXPLORE && currentCountry.valid && favoriteButton.inside(x, y)) {
		bool nowSaved;

		if (
			favorites.contains(
				currentCountry.alpha3)) {
			favorites.remove(
				currentCountry.alpha3);

			nowSaved = false;
		} else {
			favorites.add(
				currentCountry);

			nowSaved = true;
		}

		statusMessage = nowSaved
			? currentCountry.commonName
				+ " saved to Favorites."
			: currentCountry.commonName
				+ " removed from Favorites.";

		return;
	}

	if (
		currentPage == PAGE_EXPLORE && resetMapButton.inside(x, y)) {
		worldMap.resetView();
		return;
	}

	if (
		currentPage == PAGE_EXPLORE && isInsideMap(x, y)) {
		string clickedCountry = worldMap.countryAt(x, y);

		worldMap.mousePressed(
			x,
			y,
			button);

		if (
			button == OF_MOUSE_BUTTON_LEFT && !clickedCountry.empty()) {
			performSearch(
				clickedCountry);
		}

		return;
	}

	if (
		currentPage == PAGE_FAVORITES) {
		const auto & items = favorites.getAll();

		for (
			size_t i = 0;
			i < favoriteRemoveButtons.size() && i < items.size();
			++i) {
			if (
				favoriteRemoveButtons[i]
					.inside(x, y)) {
				string removedName = items[i].commonName;

				string removedCode = items[i].alpha3;

				favorites.remove(
					removedCode);

				statusMessage = removedName
					+ " removed from Favorites.";

				return;
			}
		}

		for (
			size_t i = 0;
			i < favoriteCards.size() && i < items.size();
			++i) {
			if (
				favoriteCards[i]
					.inside(x, y)) {
				currentCountry = items[i];

				searchText = currentCountry
								 .commonName;

				worldMap.focusCountry(
					currentCountry.alpha3);

				loadFlag(
					currentCountry);

				currentPage = PAGE_EXPLORE;

				statusMessage = "Opened saved country.";

				return;
			}
		}
	}

	for (
		size_t i = 0;
		i < recentButtons.size() && i < recentSearches.size();
		++i) {
		if (
			recentButtons[i]
				.inside(x, y)) {
			performSearch(
				recentSearches[i]);

			return;
		}
	}

	searchFocused = false;
}

// Pass drag movement to the map so the user can pan it.
void ofApp::mouseDragged(
	int x,
	int y,
	int button) {

	if (
		currentPage == PAGE_EXPLORE && isInsideMap(x, y)) {
		worldMap.mouseDragged(
			x,
			y,
			button);
	}
}

// End any active map dragging when the mouse button is released.
void ofApp::mouseReleased(
	int x,
	int y,
	int button) {

	worldMap.mouseReleased(
		x,
		y,
		button);
}

// Pass the mouse wheel to the map to control zoom on the Explore page.
void ofApp::mouseScrolled(
	ofMouseEventArgs & args) {

	if (
		currentPage == PAGE_EXPLORE) {
		worldMap.mouseScrolled(
			args.scrollY,
			(int)args.x,
			(int)args.y);
	}
}
