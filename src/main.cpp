#include "ofMain.h"
#include "openVisualizer.h"

//========================================================================
int main(){
	ofSetupOpenGL(1600,900,OF_WINDOW);			// <-------- Setup OpenGL

	// Runs app in 1600x900 resolution.
	// Can be run in OF_WINDOW or OF_FULLSCREEN.
	ofRunApp(new openVisualizer());

}
