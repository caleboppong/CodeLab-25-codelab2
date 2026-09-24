#include "ofApp.h"
#include "ofMain.h"

int main() {
	// Configure the main GlobeLens application window.
	ofGLFWWindowSettings settings;

	// Set the starting window size and allow the user to resize it.
	settings.setSize(1440, 900);
	settings.resizable = true;

	// Create the window and start the GlobeLens application.
	auto window = ofCreateWindow(settings);
	ofRunApp(window, make_shared<ofApp>());

	// Keep the application running until the window is closed.
	ofRunMainLoop();
}
