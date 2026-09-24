#include "WorldMap.h"

bool WorldMap::setup(
	const string & geoJsonPath) {

	countries.clear();
	loaded = false;
	resetView();

	const string fullPath = ofToDataPath(
		geoJsonPath,
		true);

	if (
		!ofFile::doesFileExist(
			fullPath)) {
		ofLogError("WorldMap")
			<< "Missing GeoJSON: "
			<< fullPath;

		return false;
	}

	try {
		ofJson geo = ofLoadJson(fullPath);

		if (
			!geo.contains("features") || !geo["features"].is_array()) {
			ofLogError("WorldMap")
				<< "GeoJSON does not contain a FeatureCollection.";

			return false;
		}

		for (
			const auto & feature :
			geo["features"]) {
			if (
				!feature.is_object() || !feature.contains("geometry")) {
				continue;
			}

			MapCountry country;

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

			parseGeometry(
				feature["geometry"],
				country);

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
		ofLogError("WorldMap")
			<< e.what();

		return false;
	}
}

void WorldMap::setBounds(
	const ofRectangle & newBounds) {

	bounds = newBounds;
}

bool WorldMap::isLoaded() const {
	return loaded;
}

void WorldMap::resetView() {
	zoom = 1.0f;
	pan = glm::vec2(0.0f);
}

glm::vec2 WorldMap::lonLatToBase(
	double lon,
	double lat) const {

	// Store map coordinates normalized to 0..1.
	// This makes the map independent of the window size.
	float x = ofMap(
		(float)lon,
		-180.0f,
		180.0f,
		0.0f,
		1.0f,
		true);

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

glm::vec2 WorldMap::baseToScreen(
	const glm::vec2 & p) const {

	glm::vec2 center(
		bounds.width * 0.5f,
		bounds.height * 0.5f);

	glm::vec2 local(
		p.x * bounds.width,
		p.y * bounds.height);

	local = center
		+ (local - center) * zoom
		+ pan;

	return glm::vec2(
		bounds.x + local.x,
		bounds.y + local.y);
}

glm::vec2 WorldMap::screenToBase(
	float x,
	float y) const {

	glm::vec2 center(
		bounds.width * 0.5f,
		bounds.height * 0.5f);

	glm::vec2 local(
		x - bounds.x,
		y - bounds.y);

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

bool WorldMap::pointInPolygon(
	const glm::vec2 & point,
	const vector<glm::vec2> & polygon) const {

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

		if (intersect) {
			inside = !inside;
		}

		j = i;
	}

	return inside;
}

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

void WorldMap::addRing(
	const ofJson & ring,
	MapCountry & country) {

	if (
		!ring.is_array() || ring.size() < 3) {
		return;
	}

	MapPolygon polygon;

	for (
		const auto & coordinate :
		ring) {
		if (
			coordinate.is_array() && coordinate.size() >= 2 && coordinate[0].is_number() && coordinate[1].is_number()) {
			double lon = coordinate[0]
							 .get<double>();

			double lat = coordinate[1]
							 .get<double>();

			polygon.points.push_back(
				lonLatToBase(
					lon,
					lat));
		}
	}

	if (
		polygon.points.size() >= 3) {
		country.polygons.push_back(
			polygon);
	}
}

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

	if (type == "Polygon") {
		if (
			coordinates.is_array() && !coordinates.empty()) {
			addRing(
				coordinates[0],
				country);
		}
	} else if (
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

void WorldMap::update(
	int mouseX,
	int mouseY) {

	hoverAlpha3.clear();
	hoverName.clear();

	if (
		!loaded || !bounds.inside(mouseX, mouseY)) {
		return;
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
				hoverAlpha3 = country.alpha3;

				hoverName = country.name;

				return;
			}
		}
	}
}

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

string WorldMap::hoveredName() const {
	return hoverName;
}

string WorldMap::hoveredAlpha3() const {
	return hoverAlpha3;
}

void WorldMap::draw(
	const string & selectedAlpha3,
	ofTrueTypeFont & smallFont,
	ofTrueTypeFont & tinyFont) {

	ofPushStyle();

	ofSetColor(18, 27, 39);
	ofDrawRectRounded(
		bounds,
		12);

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

	// IMPORTANT:
	// Clip all map rendering to its panel.
	// This prevents zoomed countries from drawing over the sidebar/cards.
	glEnable(GL_SCISSOR_TEST);

	glScissor(
		(GLint)bounds.x,
		(GLint)(ofGetHeight()
			- bounds.y
			- bounds.height),
		(GLsizei)bounds.width,
		(GLsizei)bounds.height);

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

	for (
		const auto & country :
		countries) {
		bool selected = !selectedAlpha3.empty()
			&& ofToLower(country.alpha3)
				== ofToLower(selectedAlpha3);

		bool hovered = !hoverAlpha3.empty()
			&& ofToLower(country.alpha3)
				== ofToLower(hoverAlpha3);

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
	glDisable(GL_SCISSOR_TEST);

	if (!hoverName.empty()) {
		float tooltipW = smallFont.stringWidth(
							 hoverName)
			+ 28;

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

	ofSetColor(115, 135, 160);

	tinyFont.drawString(
		"Wheel: zoom   Drag: pan   Right-click: reset",
		bounds.x + 16,
		bounds.getBottom() - 12);

	ofPopStyle();
}

void WorldMap::mousePressed(
	int x,
	int y,
	int button) {

	if (
		!bounds.inside(
			x,
			y)) {
		return;
	}

	if (
		button == OF_MOUSE_BUTTON_LEFT) {
		dragging = true;

		dragStartMouse = glm::vec2(
			(float)x,
			(float)y);

		dragStartPan = pan;
	} else if (
		button == OF_MOUSE_BUTTON_RIGHT) {
		resetView();
	}
}

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

	pan = dragStartPan
		+ (now - dragStartMouse);

	clampPan();
}

void WorldMap::mouseReleased(
	int x,
	int y,
	int button) {

	dragging = false;
}

void WorldMap::mouseScrolled(
	float scrollY,
	int mouseX,
	int mouseY) {

	if (
		!bounds.inside(
			mouseX,
			mouseY)) {
		return;
	}

	float oldZoom = zoom;

	if (scrollY > 0) {
		zoom *= 1.15f;
	} else if (scrollY < 0) {
		zoom /= 1.15f;
	}

	zoom = ofClamp(
		zoom,
		1.0f,
		7.0f);

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

	glm::vec2 baseUnderCursor = center
		+ (cursorLocal
			  - center
			  - pan)
			/ oldZoom;

	pan = cursorLocal
		- center
		- (baseUnderCursor
			  - center)
			* zoom;

	clampPan();
}

void WorldMap::clampPan() {
	if (zoom <= 1.0f) {
		pan = glm::vec2(0.0f);
		return;
	}

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

void WorldMap::focusCountry(
	const string & alpha3) {

	if (alpha3.empty()) {
		return;
	}

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
		// This avoids small remote islands pulling the camera away
		// from the country the user expects to see.
		const MapPolygon * mainPolygon = &country.polygons[0];

		for (const auto & polygon : country.polygons) {
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

		float countryW = std::max(
			0.015f,
			maxX - minX);

		float countryH = std::max(
			0.015f,
			maxY - minY);

		float targetZoomX = 0.38f / countryW;

		float targetZoomY = 0.48f / countryH;

		zoom = ofClamp(
			std::min(
				targetZoomX,
				targetZoomY),
			1.35f,
			6.0f);

		glm::vec2 target(
			(minX + maxX) * 0.5f,
			(minY + maxY) * 0.5f);

		glm::vec2 targetPx(
			target.x * bounds.width,
			target.y * bounds.height);

		glm::vec2 center(
			bounds.width * 0.5f,
			bounds.height * 0.5f);

		pan = center
			- (center
				+ (targetPx - center)
					* zoom);

		clampPan();
		return;
	}
}
