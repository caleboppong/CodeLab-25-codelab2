#pragma once

#include "ofMain.h"

#include "CountryAPI.h"
#include "CountryData.h"
#include "FavoritesManager.h"
#include "WorldMap.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);

	void mousePressed(int x, int y, int button);
	void mouseDragged(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(ofMouseEventArgs & args);

	void windowResized(int w, int h);

private:
	enum Page {
		PAGE_EXPLORE = 0,
		PAGE_COMPARE = 1,
		PAGE_FAVORITES = 2,
		PAGE_INSIGHTS = 3
	};

	CountryAPI api;
	WorldMap worldMap;
	FavoritesManager favorites;

	CountryData currentCountry;
	CountryData compareA;
	CountryData compareB;

	vector<string> recentSearches;
	vector<ofRectangle> recentButtons;
	vector<ofRectangle> favoriteCards;
	vector<ofRectangle> favoriteRemoveButtons;

	int currentPage = PAGE_EXPLORE;

	string searchText;
	string statusMessage;

	bool searchFocused = true;
	bool searching = false;
	bool searchError = false;

	// 0 = population, 1 = population density
	int insightMetric = 0;

	ofImage currentFlag;
	bool flagLoaded = false;

	ofTrueTypeFont logoFont;
	ofTrueTypeFont titleFont;
	ofTrueTypeFont headingFont;
	ofTrueTypeFont bodyFont;
	ofTrueTypeFont smallFont;
	ofTrueTypeFont tinyFont;

	// Responsive layout
	float sidebarWidth = 250.0f;
	float topBarHeight = 110.0f;
	float outerMargin = 28.0f;
	float cardGap = 18.0f;

	ofRectangle contentBounds;
	ofRectangle heroRect;
	ofRectangle detailsRect;
	ofRectangle mapPanelRect;

	ofRectangle navExplore;
	ofRectangle navCompare;
	ofRectangle navFavorites;
	ofRectangle navInsights;

	ofRectangle searchBox;
	ofRectangle searchButton;

	ofRectangle compareButton;
	ofRectangle favoriteButton;
	ofRectangle resetMapButton;
	ofRectangle clearCompareButton;
	ofRectangle insightPopulationButton;
	ofRectangle insightDensityButton;

	ofRectangle mapBounds;

	void updateLayout();

	void performSearch(const string & countryName);
	void loadFlag(const CountryData & country);
	string trimSearchText(const string & text) const;
	string resolveCountryAlias(const string & text) const;

	void drawSidebar();
	void drawTopBar();

	void drawExplore();
	void drawCompare();
	void drawFavorites();
	void drawInsights();

	void drawCountryHero();
	void drawMetricCards();
	void drawDetailsCard();
	void drawRecentSearches();

	void drawNavButton(
		const ofRectangle & rect,
		const string & title,
		int page);

	void drawMetricCard(
		float x,
		float y,
		float w,
		float h,
		const string & label,
		const string & value);

	void drawCompareCountryCard(
		const CountryData & c,
		float x,
		float y,
		float w,
		const string & slotName);

	string formatNumber(long long value) const;
	string shortNumber(long long value) const;
	string formatDouble(double value, int decimals = 1) const;

	bool isInsideMap(int x, int y) const;
};
