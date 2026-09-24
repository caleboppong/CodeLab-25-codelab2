#include "ofMain.h"
#include "ofApp.h"

int main() {
    ofGLFWWindowSettings settings;

    settings.setSize(1440, 900);
    settings.resizable = true;

    auto window = ofCreateWindow(settings);
    ofRunApp(window, make_shared<ofApp>());
    ofRunMainLoop();
}
