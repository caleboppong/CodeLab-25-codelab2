#include "WorldMap.h"

// Load the GeoJSON world map and convert its country features
// into the internal polygon structure used by GlobeLens.
bool WorldMap::setup(
	const string & geoJsonPath) {

	// Remove any previously loaded map data and reset the view.
	countries.clear();
	loaded = false;
	resetView();

	// Convert the relative data path into the full file location.
	const string fullPath = ofToDataPath(
		geoJsonPath,
		true);

	// The map cannot be displayed without the GeoJSON file.
	if (
		!ofFile::doesFileExist(
			fullPath)) {
		ofLogError("WorldMap")
			<< "Missing GeoJSON: "
			<< fullPath;

		return false;
	}

	try {
		// Load the GeoJSON document.
		ofJson geo = ofLoadJson(fullPath);

		// A valid GeoJSON FeatureCollection should contain
		// an array called "features".
		if (
			!geo.contains("features") || !geo["features"].is_array()) {
			ofLogError("WorldMap")
				<< "GeoJSON does not contain a FeatureCollection.";

			return false;
		}

		// Process every country feature in the GeoJSON file.
		for (
			const auto & feature :
			geo["features"]) {

			if (
				!feature.is_object() || !feature.contains("geometry")) {
				continue;
			}

			MapCountry country;

			// Read the country's name and ISO codes.
			// Several possible property names are supported because
			// GeoJSON datasets do not always use identical naming.
			if (
				feature.contains("properties") && feature["properties"].is_object()) {

				const auto & p = feature["properties"];

				country.name = getPropertyString(
					p,
					{ "name",
						"NAME",
						"ADMIN",
						"admin" });

				country.alpha2 = getPropertyString(
					p,
					{ "ISO3166-1-Alpha-2",
						"ISO_A2",
						"iso_a2",
						"alpha2",
						"cca2" });

				country.alpha3 = getPropertyString(
					p,
					{ "ISO3166-1-Alpha-3",
						"ISO_A3",
						"iso_a3",
						"ADM0_A3",
						"alpha3",
						"cca3" });
			}

			// Convert the GeoJSON geometry into drawable polygons.
			parseGeometry(
				feature["geometry"],
				country);

			// Only keep countries that contain usable polygon data.
			if (
				!country.polygons.empty()) {
				countries.push_back(
					country);
			}
		}

		loaded = !countries.empty();

		ofLogNotice("WorldMap")
			<< "Loaded map countries/features: "
			<< countries.size();

		return loaded;
	} catch (
		const std::exception & e) {

		// Report invalid or unreadable GeoJSON without crashing the app.
		ofLogError("WorldMap")
			<< e.what();

		return false;
	}
}

// Update the screen area in which the map should be displayed.
void WorldMap::setBounds(
	const ofRectangle & newBounds) {

	bounds = newBounds;
}

// Report whether valid map data has been loaded.
bool WorldMap::isLoaded() const {
	return loaded;
}

// Return the map to its original world view.
void WorldMap::resetView() {
	zoom = 1.0f;
	pan = glm::vec2(0.0f);
}

// Convert geographical longitude and latitude into normalized
// map coordinates between 0 and 1.
glm::vec2 WorldMap::lonLatToBase(
	double lon,
	double lat) const {

	// Normalized coordinates make the map independent
	// of the current application window size.
	float x = ofMap(
		(float)lon,
		-180.0f,
		180.0f,
		0.0f,
		1.0f,
		true);

	// Latitude is reversed because screen coordinates increase
	// downwards while geographical latitude increases northwards.
	float y = ofMap(
		(float)lat,
		90.0f,
		-90.0f,
		0.0f,
		1.0f,
		true);

	return glm::vec2(
		x,
		y);
}

// Convert normalized map coordinates into actual screen coordinates,
// including the current zoom and pan transformations.
glm::vec2 WorldMap::baseToScreen(
	const glm::vec2 & p) const {

	glm::vec2 center(
		bounds.width * 0.5f,
		bounds.height * 0.5f);

	glm::vec2 local(
		p.x * bounds.width,
		p.y * bounds.height);

	// Scale around the centre of the map and then apply panning.
	local = center
		+ (local - center) * zoom
		+ pan;

	return glm::vec2(
		bounds.x + local.x,
		bounds.y + local.y);
}

// Convert a mouse/screen position back into normalized map coordinates.
// This is the inverse of baseToScreen() and is needed for hit-testing.
glm::vec2 WorldMap::screenToBase(
	float x,
	float y) const {

	glm::vec2 center(
		bounds.width * 0.5f,
		bounds.height * 0.5f);

	glm::vec2 local(
		x - bounds.x,
		y - bounds.y);

	// Reverse the current pan and zoom transformations.
	glm::vec2 unzoomed = center
		+ (local - center - pan)
			/ zoom;

	return glm::vec2(
		bounds.width > 0.0f
			? unzoomed.x / bounds.width
			: 0.0f,

		bounds.height > 0.0f
			? unzoomed.y / bounds.height
			: 0.0f);
}

// Determine whether a point lies inside a polygon.
// This uses the ray-casting algorithm, which checks how many
// times an imaginary horizontal ray crosses the polygon edges.
bool WorldMap::pointInPolygon(
	const glm::vec2 & point,
	const vector<glm::vec2> & polygon) const {

	// A polygon needs at least three points.
	if (polygon.size() < 3) {
		return false;
	}

	bool inside = false;

	size_t j = polygon.size() - 1;

	for (
		size_t i = 0;
		i < polygon.size();
		++i) {

		const auto & a = polygon[i];
		const auto & b = polygon[j];

		float dy = b.y - a.y;

		// Avoid division by zero for horizontal polygon edges.
		if (
			std::abs(dy) < 0.000001f) {
			dy = 0.000001f;
		}

		bool intersect = ((a.y > point.y)
							 != (b.y > point.y))
			&& (point.x < (b.x - a.x)
						* (point.y - a.y)
						/ dy
					+ a.x);

		// Each crossing switches between outside and inside.
		if (intersect) {
			inside = !inside;
		}

		j = i;
	}

	return inside;
}

// Return the first available string from a selection of possible
// GeoJSON property names.
string WorldMap::getPropertyString(
	const ofJson & properties,
	const vector<string> & keys) const {

	for (
		const auto & key :
		keys) {

		if (
			properties.contains(key) && properties[key].is_string()) {

			return properties[key]
				.get<string>();
		}
	}

	return "";
}

// Convert one GeoJSON coordinate ring into a drawable map polygon.
void WorldMap::addRing(
	const ofJson & ring,
	MapCountry & country) {

	if (
		!ring.is_array() || ring.size() < 3) {
		return;
	}

	MapPolygon polygon;

	// Each GeoJSON coordinate contains longitude followed by latitude.
	for (
		const auto & coordinate :
		ring) {

		if (
			coordinate.is_array() && coordinate.size() >= 2 && coordinate[0].is_number() && coordinate[1].is_number()) {

			double lon = coordinate[0]
							 .get<double>();

			double lat = coordinate[1]
							 .get<double>();

			// Store normalized coordinates rather than fixed screen positions.
			polygon.points.push_back(
				lonLatToBase(
					lon,
					lat));
		}
	}

	// Only store valid polygons.
	if (
		polygon.points.size() >= 3) {
		country.polygons.push_back(
			polygon);
	}
}

// Interpret GeoJSON Polygon and MultiPolygon geometry.
// MultiPolygon support is important for countries made up of
// separated land areas or islands.
void WorldMap::parseGeometry(
	const ofJson & geometry,
	MapCountry & country) {

	if (!geometry.is_object()) {
		return;
	}

	if (
		!geometry.contains("type") || !geometry["type"].is_string() || !geometry.contains("coordinates")) {
		return;
	}

	string type = geometry["type"]
					  .get<string>();

	const auto & coordinates = geometry["coordinates"];

	// A normal Polygon contains one outer coordinate ring.
	if (type == "Polygon") {
		if (
			coordinates.is_array() && !coordinates.empty()) {

			addRing(
				coordinates[0],
				country);
		}
	}
	// A MultiPolygon contains several separate polygon areas.
	else if (
		type == "MultiPolygon") {

		if (
			!coordinates.is_array()) {
			return;
		}

		for (
			const auto & polygon :
			coordinates) {

			if (
				polygon.is_array() && !polygon.empty()) {

				addRing(
					polygon[0],
					country);
			}
		}
	}
}

// Update which country is currently underneath the mouse pointer.
void WorldMap::update(
	int mouseX,
	int mouseY) {

	// Clear the previous hover result before checking again.
	hoverAlpha3.clear();
	hoverName.clear();

	if (
		!loaded || !bounds.inside(mouseX, mouseY)) {
		return;
	}

	// Convert the mouse position back into the normalized
	// coordinate system used by the country polygons.
	glm::vec2 point = screenToBase(
		(float)mouseX,
		(float)mouseY);

	// Test the mouse position against every country polygon.
	for (
		const auto & country :
		countries) {

		for (
			const auto & polygon :
			country.polygons) {

			if (
				pointInPolygon(
					point,
					polygon.points)) {

				hoverAlpha3 = country.alpha3;
				hoverName = country.name;

				// Only one country can be hovered at a time.
				return;
			}
		}
	}
}

// Return the country name underneath a supplied mouse position.
// This is used when the user clicks a country on the map.
string WorldMap::countryAt(
	int mouseX,
	int mouseY) const {

	if (
		!loaded || !bounds.inside(mouseX, mouseY)) {
		return "";
	}

	glm::vec2 point = screenToBase(
		(float)mouseX,
		(float)mouseY);

	for (
		const auto & country :
		countries) {

		for (
			const auto & polygon :
			country.polygons) {

			if (
				pointInPolygon(
					point,
					polygon.points)) {

				return country.name;
			}
		}
	}

	return "";
}

// Return the name of the country currently under the mouse.
string WorldMap::hoveredName() const {
	return hoverName;
}

// Return the alpha-3 code of the country currently under the mouse.
string WorldMap::hoveredAlpha3() const {
	return hoverAlpha3;
}

// Draw the interactive world map.
// The selected country and hovered country receive different colours.
void WorldMap::draw(
	const string & selectedAlpha3,
	ofTrueTypeFont & smallFont,
	ofTrueTypeFont & tinyFont) {

	ofPushStyle();

	// Draw the map panel background.
	ofSetColor(18, 27, 39);

	ofDrawRectRounded(
		bounds,
		12);

	// Display a useful message if the GeoJSON data could not be loaded.
	if (!loaded) {
		ofSetColor(235);

		smallFont.drawString(
			"Map data missing",
			bounds.x + 24,
			bounds.y + 38);

		ofSetColor(125, 145, 170);

		tinyFont.drawString(
			"Put countries.geojson in bin/data/maps/",
			bounds.x + 24,
			bounds.y + 62);

		ofPopStyle();
		return;
	}

	// Clip all map rendering to its panel.
	// This prevents zoomed countries from drawing over
	// other interface areas such as the sidebar and cards.
	glEnable(GL_SCISSOR_TEST);

	glScissor(
		(GLint)bounds.x,
		(GLint)(ofGetHeight()
			- bounds.y
			- bounds.height),
		(GLsizei)bounds.width,
		(GLsizei)bounds.height);

	// Draw a subtle reference grid behind the countries.
	ofSetColor(36, 49, 66);

	for (int i = 1; i < 6; ++i) {
		float x = bounds.x
			+ bounds.width
				* ((float)i / 6.0f);

		ofDrawLine(
			x,
			bounds.y,
			x,
			bounds.getBottom());
	}

	for (int i = 1; i < 4; ++i) {
		float y = bounds.y
			+ bounds.height
				* ((float)i / 4.0f);

		ofDrawLine(
			bounds.x,
			y,
			bounds.getRight(),
			y);
	}

	// Draw the filled shape of every country.
	for (
		const auto & country :
		countries) {

		// Compare ISO codes without considering letter case.
		bool selected = !selectedAlpha3.empty()
			&& ofToLower(country.alpha3)
				== ofToLower(selectedAlpha3);

		bool hovered = !hoverAlpha3.empty()
			&& ofToLower(country.alpha3)
				== ofToLower(hoverAlpha3);

		// Selected and hovered countries use different colours
		// to provide visual feedback to the user.
		if (selected) {
			ofSetColor(
				69,
				142,
				255);
		} else if (hovered) {
			ofSetColor(
				79,
				111,
				153);
		} else {
			ofSetColor(
				49,
				66,
				88);
		}

		for (
			const auto & polygon :
			country.polygons) {

			if (
				polygon.points.size() < 3) {
				continue;
			}

			ofBeginShape();

			// Transform every normalized point into its current
			// zoomed and panned screen position.
			for (
				const auto & basePoint :
				polygon.points) {

				glm::vec2 p = baseToScreen(
					basePoint);

				ofVertex(
					p.x,
					p.y);
			}

			ofEndShape(true);
		}
	}

	// Draw country outlines over the filled polygons.
	ofNoFill();
	ofSetLineWidth(1.0f);
	ofSetColor(72, 91, 116);

	for (
		const auto & country :
		countries) {

		for (
			const auto & polygon :
			country.polygons) {

			if (
				polygon.points.size() < 3) {
				continue;
			}

			ofBeginShape();

			for (
				const auto & basePoint :
				polygon.points) {

				glm::vec2 p = baseToScreen(
					basePoint);

				ofVertex(
					p.x,
					p.y);
			}

			ofEndShape(true);
		}
	}

	ofFill();

	// End clipping before drawing the tooltip.
	// The tooltip can then remain readable near the edge of the panel.
	glDisable(GL_SCISSOR_TEST);

	// Show the country name while the user hovers over the map.
	if (!hoverName.empty()) {

		float tooltipW = smallFont.stringWidth(
							 hoverName)
			+ 28;

		// Keep the tooltip inside the visible map area.
		float x = ofClamp(
			(float)ofGetMouseX()
				+ 14,
			bounds.x + 5,
			bounds.getRight()
				- tooltipW
				- 5);

		float y = ofClamp(
			(float)ofGetMouseY()
				- 34,
			bounds.y + 5,
			bounds.getBottom()
				- 34);

		ofSetColor(
			10,
			16,
			24,
			235);

		ofDrawRectRounded(
			x,
			y,
			tooltipW,
			30,
			7);

		ofSetColor(240);

		smallFont.drawString(
			hoverName,
			x + 14,
			y + 20);
	}

	// Display map interaction instructions.
	ofSetColor(115, 135, 160);

	tinyFont.drawString(
		"Wheel: zoom   Drag: pan   Right-click: reset",
		bounds.x + 16,
		bounds.getBottom() - 12);

	ofPopStyle();
}

// Begin map dragging with the left mouse button,
// or reset the map with the right mouse button.
void WorldMap::mousePressed(
	int x,
	int y,
	int button) {

	// Ignore mouse actions outside the map panel.
	if (
		!bounds.inside(
			x,
			y)) {
		return;
	}

	if (
		button == OF_MOUSE_BUTTON_LEFT) {

		dragging = true;

		// Remember where the drag started so the new pan
		// position can be calculated relative to it.
		dragStartMouse = glm::vec2(
			(float)x,
			(float)y);

		dragStartPan = pan;
	} else if (
		button == OF_MOUSE_BUTTON_RIGHT) {

		resetView();
	}
}

// Pan the map while the user drags with the left mouse button.
void WorldMap::mouseDragged(
	int x,
	int y,
	int button) {

	if (
		!dragging || button != OF_MOUSE_BUTTON_LEFT) {
		return;
	}

	glm::vec2 now(
		(float)x,
		(float)y);

	// Move the map by the same distance as the mouse movement.
	pan = dragStartPan
		+ (now - dragStartMouse);

	// Prevent the map from being dragged completely out of view.
	clampPan();
}

// Stop dragging when the mouse button is released.
void WorldMap::mouseReleased(
	int x,
	int y,
	int button) {

	dragging = false;
}

// Zoom the map in or out around the current mouse position.
void WorldMap::mouseScrolled(
	float scrollY,
	int mouseX,
	int mouseY) {

	// Only zoom when the mouse is inside the map.
	if (
		!bounds.inside(
			mouseX,
			mouseY)) {
		return;
	}

	float oldZoom = zoom;

	// Mouse wheel direction controls zoom in or zoom out.
	if (scrollY > 0) {
		zoom *= 1.15f;
	} else if (scrollY < 0) {
		zoom /= 1.15f;
	}

	// Limit zoom to a useful range.
	zoom = ofClamp(
		zoom,
		1.0f,
		7.0f);

	// No further calculation is needed if the zoom was already
	// at its minimum or maximum value.
	if (
		oldZoom == zoom) {
		return;
	}

	glm::vec2 cursorLocal(
		mouseX - bounds.x,
		mouseY - bounds.y);

	glm::vec2 center(
		bounds.width * 0.5f,
		bounds.height * 0.5f);

	// Find the point that was underneath the mouse before zooming.
	glm::vec2 baseUnderCursor = center
		+ (cursorLocal
			  - center
			  - pan)
			/ oldZoom;

	// Adjust the pan so the same map location remains underneath
	// the mouse after the zoom level changes.
	pan = cursorLocal
		- center
		- (baseUnderCursor
			  - center)
			* zoom;

	clampPan();
}

// Restrict map panning so the zoomed map cannot be moved
// completely outside its visible panel.
void WorldMap::clampPan() {

	// At normal zoom there is no need for panning.
	if (zoom <= 1.0f) {
		pan = glm::vec2(0.0f);
		return;
	}

	// Calculate the maximum movement allowed by the current zoom.
	float maxX = bounds.width
		* (zoom - 1.0f)
		* 0.5f;

	float maxY = bounds.height
		* (zoom - 1.0f)
		* 0.5f;

	pan.x = ofClamp(
		pan.x,
		-maxX,
		maxX);

	pan.y = ofClamp(
		pan.y,
		-maxY,
		maxY);
}

// Automatically zoom and pan the map towards a selected country.
void WorldMap::focusCountry(
	const string & alpha3) {

	if (alpha3.empty()) {
		return;
	}

	// Find the map country using its unique alpha-3 code.
	for (
		const auto & country :
		countries) {

		if (
			ofToLower(country.alpha3)
			!= ofToLower(alpha3)) {
			continue;
		}

		if (country.polygons.empty()) {
			return;
		}

		// Use the polygon with the most points as the main landmass.
		// This prevents small remote islands from pulling the camera
		// away from the area the user expects to see.
		const MapPolygon * mainPolygon = &country.polygons[0];

		for (
			const auto & polygon :
			country.polygons) {

			if (
				polygon.points.size()
				> mainPolygon->points.size()) {

				mainPolygon = &polygon;
			}
		}

		if (
			mainPolygon->points.empty()) {
			return;
		}

		// Find the bounding box of the country's main polygon.
		float minX = 1.0f;
		float maxX = 0.0f;
		float minY = 1.0f;
		float maxY = 0.0f;

		for (
			const auto & p :
			mainPolygon->points) {

			minX = std::min(
				minX,
				p.x);

			maxX = std::max(
				maxX,
				p.x);

			minY = std::min(
				minY,
				p.y);

			maxY = std::max(
				maxY,
				p.y);
		}

		// Prevent extremely small countries from producing
		// an excessive zoom level.
		float countryW = std::max(
			0.015f,
			maxX - minX);

		float countryH = std::max(
			0.015f,
			maxY - minY);

		// Calculate suitable horizontal and vertical zoom levels.
		float targetZoomX = 0.38f / countryW;

		float targetZoomY = 0.48f / countryH;

		// Use the smaller value so the country remains visible,
		// then restrict it to the allowed focus range.
		zoom = ofClamp(
			std::min(
				targetZoomX,
				targetZoomY),
			1.35f,
			6.0f);

		// Find the centre of the country's bounding box.
		glm::vec2 target(
			(minX + maxX) * 0.5f,
			(minY + maxY) * 0.5f);

		glm::vec2 targetPx(
			target.x * bounds.width,
			target.y * bounds.height);

		glm::vec2 center(
			bounds.width * 0.5f,
			bounds.height * 0.5f);

		// Calculate the pan needed to place the selected
		// country in the centre of the map panel.
		pan = center
			- (center
				+ (targetPx - center)
					* zoom);

		clampPan();
		return;
	}
}
