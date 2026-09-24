#pragma once

#include "ofMain.h"

// Represents one polygon that forms part of a country's shape.
// Points are stored as normalized 0..1 coordinates so the map
// can resize without changing the original geographical data.
struct MapPolygon {
	vector<glm::vec2> points;
};

// Represents one country on the world map.
// A country can contain several polygons because some countries
// include islands or separate land areas.
struct MapCountry {
	string name;
	string alpha2;
	string alpha3;

	vector<MapPolygon> polygons;
};

// Handles loading, drawing and interacting with the GlobeLens world map.
class WorldMap {
public:
	// Load and process country shapes from the GeoJSON map file.
	bool setup(const string & geoJsonPath = "maps/countries.geojson");

	// Set the area of the application window used by the map.
	void setBounds(const ofRectangle & bounds);

	// Update the country currently underneath the mouse pointer.
	void update(int mouseX, int mouseY);

	// Draw the world map and visually identify the selected
	// and currently hovered countries.
	void draw(
		const string & selectedAlpha3,
		ofTrueTypeFont & smallFont,
		ofTrueTypeFont & tinyFont);

	// Return information about the country underneath the mouse.
	string countryAt(int mouseX, int mouseY) const;
	string hoveredName() const;
	string hoveredAlpha3() const;

	// Handle mouse interaction for dragging and resetting the map.
	void mousePressed(int x, int y, int button);
	void mouseDragged(int x, int y, int button);
	void mouseReleased(int x, int y, int button);

	// Handle mouse-wheel zooming around the cursor position.
	void mouseScrolled(
		float scrollY,
		int mouseX,
		int mouseY);

	// Automatically zoom and centre the map on a selected country.
	void focusCountry(const string & alpha3);

	// Return the map to its default zoom and position.
	void resetView();

	// Report whether the GeoJSON map data loaded successfully.
	bool isLoaded() const;

private:
	// Screen area currently occupied by the map.
	ofRectangle bounds;

	// All countries and their polygon data loaded from GeoJSON.
	vector<MapCountry> countries;

	bool loaded = false;

	// Current map zoom level.
	float zoom = 1.0f;

	// Pan is stored in screen pixels.
	glm::vec2 pan = glm::vec2(0.0f);

	// Tracks whether the user is currently dragging the map.
	bool dragging = false;

	// Starting mouse and map positions used to calculate dragging.
	glm::vec2 dragStartMouse;
	glm::vec2 dragStartPan;

	// Information about the country currently under the mouse pointer.
	string hoverAlpha3;
	string hoverName;

	// Convert longitude and latitude into normalized map coordinates.
	glm::vec2 lonLatToBase(
		double lon,
		double lat) const;

	// Convert normalized map coordinates into screen coordinates,
	// including the current zoom and pan.
	glm::vec2 baseToScreen(
		const glm::vec2 & p) const;

	// Reverse the screen transformation for mouse hit-testing.
	glm::vec2 screenToBase(
		float x,
		float y) const;

	// Test whether a point lies inside a country's polygon.
	bool pointInPolygon(
		const glm::vec2 & point,
		const vector<glm::vec2> & polygon) const;

	// Convert GeoJSON Polygon or MultiPolygon geometry
	// into the internal map structure.
	void parseGeometry(
		const ofJson & geometry,
		MapCountry & country);

	// Convert one GeoJSON coordinate ring into a MapPolygon.
	void addRing(
		const ofJson & ring,
		MapCountry & country);

	// Restrict panning so the map cannot move completely out of view.
	void clampPan();

	// Search several possible GeoJSON property names
	// and return the first valid string.
	string getPropertyString(
		const ofJson & properties,
		const vector<string> & keys) const;
};
