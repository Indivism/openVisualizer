#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"

const int kFontSize = 32; //Font size for player.
const float kFramerate = 60.0f; //Framerate of the player (60fps).
const int kFftSize = 8192; //Hz size for songs.
const int kBandSize = 1024; //Bands of frequency.
const int kDrawAmount = 128; //Amount of times to draw visualizer objects per draw() call.
const float kFftSmoothing = 0.965f; //Fast Fourier Transform smoothing constant.
const double kBassFrequency = 2.0; //Frequency for bass notes.

const int kBackgroundHsbValue = 175; //Default Hsb values of background.
const int kBarSize = 20; //Width of visualizer bars.
const int kBarSpacing = 50; //Size of spacing between visualizer bars.
const int kGUIPadding = 300; //Size of music player GUI padding.
const int kVerticalTitlePadding = 75; //Size of default Now Playing vertical padding.
const int kTitlePadding = 50; //Size of spacing between text in Now Playing.
const int kHueAmount = 256; //Range of hue values for visualizer gradient.
const int kParticleAmount = 360; //Amount of particles to draw for visualizer particle mode.
const int kSpectrumUpperBound = 50; //Upper bound for spectrum visualizer size in interactive spectrum mode.
const int kSpectrumLowerBound = 20; //Lower bound for spectrum visualizer size in interactive spectrum mode.
const double kTransitionAmount = 0.1; //Amount to increment during transition.
const int kFftMultiplier = 250; //Fft multiplier amount.
const int kFftSpiralMultiplier = 50; //Spiral Fft multiplier amount.
const int kFftParticleMultiplier = 10; //Spiral Fft multiplier amount.
const int kFftBloomMultiplier = 5; //Bloom Fft multiplier amount.

enum visualizerMode { //Denotes the visualizer modes.
	VISUALIZER_MODE_CIRCLE = 0,
	VISUALIZER_MODE_SPIRAL,
	VISUALIZER_MODE_SPECTRUM,
	VISUALIZER_MODE_PARTICLE,
	VISUALIZER_MODE_BLOOM
};

class openVisualizer : public ofBaseApp {
	
	string song_path; //Represents file path of song.
	string song_title; //Represents title of song.
	ofTrueTypeFont player_font; //Music player font (OpenSans-Light).
	ofSoundPlayer music_player; //Music player object.
	visualizerMode player_mode;
	int player_height;
	int player_width;
	
	ofxDatGui *player_gui;
	ofxDatGuiButton *load_song_button;
	ofxDatGuiColorPicker *visualizer_color_picker; //Color picker for setting visualizer color.
	ofxDatGuiColorPicker *bass_color_picker; //Color picker for setting bass color.
	ofxDatGuiSlider *spiral_speed_slider; //Slider for adjusting spiral visualizer speed.
	ofxDatGuiSlider *spectrum_bar_slider; //Slider for adjusting spectrum bar width.
	ofxDatGuiSlider *bass_frequency_slider; //Slider for adjusting bass frequency.
	ofxDatGuiToggle *background_mode; //Button for toggling between background colors.
	ofxDatGuiToggle *spectrum_mode; //Button for toggling between spectrum visualizer modes.

	float *fft_smooth; //Value that controls the intensity of the visualizer.
	bool is_playing;
	double hue_counter; //Counter for smooth color gradient transitions.
	ofColor background_color; //Background color.
	ofColor visualizer_color; //Visualizer bar/circle color.
	ofColor bass_color; //Color to change to once certain bass frequency is reached.
	double spiral_speed; //Represents the speed multiplier of the spiral visualizer.
	double spectrum_width; //Represents the width of the spectrum visualizer bars.
	double bass_frequency; //Represents the bass frequency value.
	bool draw_gradient; //Determines whether the background gradient is drawn or not.
	bool move_spectrum; //Determines whether the spectrum visualizer should move based on frequency.
	int particle_amount; //Determines the number of particles to draw.
	int particle_x_pos;
	int particle_y_pos;
	bool spectrum_increase = true;
	ofPolyline particle_connector;
	ofPolyline bloom_connector;
	ofPolyline top_spectrum_connector;
	ofPolyline bottom_spectrum_connector;

	public:
		void setup();
		void update();
		void draw();

		//Handles GUI input.
		void onButtonEvent(ofxDatGuiButtonEvent button_event);
		void onColorPickerEvent(ofxDatGuiColorPickerEvent color_picker_event);
		void onSliderEvent(ofxDatGuiSliderEvent slider_event);
		void onToggleEvent(ofxDatGuiToggleEvent toggle_event);

		void openSongSelection(ofFileDialogResult song_result); //Obtains song file path and title and loads the player with the song.
		void keyPressed(int key); //Handles music player controls.
		void windowResized(int w, int h); //Updates player_height and player_width based after resize.
		void drawNowPlaying(float x_position, float y_position); //Draws song information.
		void drawBarVisualizer(); //Draws bar visualizer.
		void drawCircleVisualizer(float x_position, float y_position, int fft_multiplier); //Draws circle visualizer.
		void drawSpiralVisualizer(float x_position, float y_position, int fft_multiplier); //Draws spiral visualizer.
		void drawSpectrumVisualizer(); //Draws audio spectrum visualizer.
		void drawParticleVisualizer(); //Draws particle visualizer.
		void drawBloomVisualizer(); //Draws bloom visualizer.
		void frequencyTransition(float fft_value); //Changes visualizer behavior when given frequency is reached.
};
