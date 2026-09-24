#pragma once

#include "ofMain.h"

struct MapPolygon {
	vector<glm::vec2> points; // normalized 0..1 map coordinates
};

struct MapCountry {
	string name;
	string alpha2;
	string alpha3;

	vector<MapPolygon> polygons;
};

class WorldMap {
public:
	bool setup(const string & geoJsonPath = "maps/countries.geojson");

	void setBounds(const ofRectangle & bounds);
	void update(int mouseX, int mouseY);

	void draw(
		const string & selectedAlpha3,
		ofTrueTypeFont & smallFont,
		ofTrueTypeFont & tinyFont);

	string countryAt(int mouseX, int mouseY) const;
	string hoveredName() const;
	string hoveredAlpha3() const;

	void mousePressed(int x, int y, int button);
	void mouseDragged(int x, int y, int button);
	void mouseReleased(int x, int y, int button);

	void mouseScrolled(
		float scrollY,
		int mouseX,
		int mouseY);

	void focusCountry(const string & alpha3);
	void resetView();

	bool isLoaded() const;

private:
	ofRectangle bounds;
	vector<MapCountry> countries;

	bool loaded = false;

	float zoom = 1.0f;

	// Pan is stored in screen pixels.
	glm::vec2 pan = glm::vec2(0.0f);

	bool dragging = false;

	glm::vec2 dragStartMouse;
	glm::vec2 dragStartPan;

	string hoverAlpha3;
	string hoverName;

	glm::vec2 lonLatToBase(
		double lon,
		double lat) const;

	glm::vec2 baseToScreen(
		const glm::vec2 & p) const;

	glm::vec2 screenToBase(
		float x,
		float y) const;

	bool pointInPolygon(
		const glm::vec2 & point,
		const vector<glm::vec2> & polygon) const;

	void parseGeometry(
		const ofJson & geometry,
		MapCountry & country);

	void addRing(
		const ofJson & ring,
		MapCountry & country);

	void clampPan();

	string getPropertyString(
		const ofJson & properties,
		const vector<string> & keys) const;
};
