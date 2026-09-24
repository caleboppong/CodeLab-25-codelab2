#pragma once

#include "ofMain.h"

#include "CountryAPI.h"
#include "CountryData.h"
#include "FavoritesManager.h"
#include "WorldMap.h"

// Main application class for GlobeLens.
// It controls the interface, navigation, country searches,
// comparisons, favorites, insights and user interaction.
class ofApp : public ofBaseApp {
public:
	// Main openFrameworks application functions.
	void setup();
	void update();
	void draw();

	// Keyboard input.
	void keyPressed(int key);
	void keyReleased(int key);

	// Mouse interaction used by buttons and the interactive map.
	void mousePressed(int x, int y, int button);
	void mouseDragged(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(ofMouseEventArgs & args);

	// Recalculates the interface when the window size changes.
	void windowResized(int w, int h);

private:
	// Represents the four main sections of the application.
	enum Page {
		PAGE_EXPLORE = 0,
		PAGE_COMPARE = 1,
		PAGE_FAVORITES = 2,
		PAGE_INSIGHTS = 3
	};

	// Main application components.
	// Each class has a separate responsibility to keep the program modular.
	CountryAPI api;
	WorldMap worldMap;
	FavoritesManager favorites;

	// Stores the active country and the two countries selected for comparison.
	CountryData currentCountry;
	CountryData compareA;
	CountryData compareB;

	// Stores recent searches and clickable areas used by dynamic UI elements.
	vector<string> recentSearches;
	vector<ofRectangle> recentButtons;
	vector<ofRectangle> favoriteCards;
	vector<ofRectangle> favoriteRemoveButtons;

	// The application starts on the Explore page.
	int currentPage = PAGE_EXPLORE;

	// Search input and feedback shown to the user.
	string searchText;
	string statusMessage;

	bool searchFocused = true;
	bool searching = false;
	bool searchError = false;

	// Controls which measurement is displayed on the Insights page.
	// 0 = population, 1 = population density.
	int insightMetric = 0;

	// Stores the flag image for the currently selected country.
	ofImage currentFlag;
	bool flagLoaded = false;

	// Fonts used to create a clear visual hierarchy in the interface.
	ofTrueTypeFont logoFont;
	ofTrueTypeFont titleFont;
	ofTrueTypeFont headingFont;
	ofTrueTypeFont bodyFont;
	ofTrueTypeFont smallFont;
	ofTrueTypeFont tinyFont;

	// Responsive layout values are recalculated when the window changes size.
	float sidebarWidth = 250.0f;
	float topBarHeight = 110.0f;
	float outerMargin = 28.0f;
	float cardGap = 18.0f;

	// Main content areas.
	ofRectangle contentBounds;
	ofRectangle heroRect;
	ofRectangle detailsRect;
	ofRectangle mapPanelRect;

	// Sidebar navigation buttons.
	ofRectangle navExplore;
	ofRectangle navCompare;
	ofRectangle navFavorites;
	ofRectangle navInsights;

	// Search controls.
	ofRectangle searchBox;
	ofRectangle searchButton;

	// Action buttons used across the different pages.
	ofRectangle compareButton;
	ofRectangle favoriteButton;
	ofRectangle resetMapButton;
	ofRectangle clearCompareButton;
	ofRectangle insightPopulationButton;
	ofRectangle insightDensityButton;

	// Defines the interactive area occupied by the world map.
	ofRectangle mapBounds;

	// Calculates positions and sizes for the responsive interface.
	void updateLayout();

	// Search helpers.
	void performSearch(const string & countryName);
	void loadFlag(const CountryData & country);
	string trimSearchText(const string & text) const;
	string resolveCountryAlias(const string & text) const;

	// Draw the main interface sections.
	void drawSidebar();
	void drawTopBar();

	// Draw the content for each application page.
	void drawExplore();
	void drawCompare();
	void drawFavorites();
	void drawInsights();

	// Draw reusable parts of the Explore interface.
	void drawCountryHero();
	void drawMetricCards();
	void drawDetailsCard();
	void drawRecentSearches();

	// Draw a reusable sidebar navigation button.
	void drawNavButton(
		const ofRectangle & rect,
		const string & title,
		int page);

	// Draw a reusable statistics card.
	void drawMetricCard(
		float x,
		float y,
		float w,
		float h,
		const string & label,
		const string & value);

	// Draw one country card on the Compare page.
	void drawCompareCountryCard(
		const CountryData & c,
		float x,
		float y,
		float w,
		const string & slotName);

	// Formatting helpers used to make numerical data easier to read.
	string formatNumber(long long value) const;
	string shortNumber(long long value) const;
	string formatDouble(double value, int decimals = 1) const;

	// Checks whether a mouse position is inside the interactive map.
	bool isInsideMap(int x, int y) const;
};
