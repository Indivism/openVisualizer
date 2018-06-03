#include "openVisualizer.h"

void openVisualizer::setup() {
	ofSetFrameRate(kFramerate); //Sets framrate to 60fps.
	background_color.setHsb(kBackgroundHsbValue, kBackgroundHsbValue, kBackgroundHsbValue); //Sets background hue, saturation, and brightness.

	//Sets up OpenVisualizer GUI with sliders, color pickers, and buttons.
	player_gui = new ofxDatGui();
	player_gui->setTheme(new ofxDatGuiThemeWireframe());
	player_gui->addHeader("OpenVisualizer Controls");
	player_gui->onButtonEvent(this, &openVisualizer::onButtonEvent);
	player_gui->onColorPickerEvent(this, &openVisualizer::onColorPickerEvent);
	player_gui->onSliderEvent(this, &openVisualizer::onSliderEvent);
	player_gui->onToggleEvent(this, &openVisualizer::onToggleEvent);
	
	load_song_button = player_gui->addButton("Load Song");
	visualizer_color_picker = player_gui->addColorPicker("Visualizer Color", ofColor::grey);
	bass_color_picker = player_gui->addColorPicker("Bass Color", ofColor::black);
	spiral_speed_slider = player_gui->addSlider("Spiral Speed", 0, 10, 2);
	spectrum_bar_slider = player_gui->addSlider("Spectrum Width", 5, 50, 10);
	bass_frequency_slider = player_gui->addSlider("Bass Frequency", 1, 5, kBassFrequency);
	background_mode = player_gui->addToggle("Background Gradient", true);
	spectrum_mode = player_gui->addToggle("Interactive Spectrum", true);

	//Sets class variables to default GUI values.
	spiral_speed = spiral_speed_slider->getValue();
	spectrum_width = spectrum_bar_slider->getValue();
	bass_frequency = bass_frequency_slider->getValue();
	draw_gradient = background_mode->getChecked();
	move_spectrum = spectrum_mode->getChecked();
	visualizer_color = visualizer_color_picker->getColor();
	bass_color = bass_color_picker->getColor();

	//Sets volume for song and loads music player font.
	song_title = "None (Use the Menu to Select a Song)";
	music_player.setVolume(1.0);
	player_font.loadFont("OpenSans-Light.ttf", kFontSize);

	fft_smooth = new float[kFftSize]; //Sets up fft_smooth as a float value with all zeroes.
	for (int i = 0; i < kFftSize; i++) {
		fft_smooth[i] = 0;
	}
}

void openVisualizer::onButtonEvent(ofxDatGuiButtonEvent button_event) {
	if (button_event.target->is("Load Song")) { //If the load song button is pressed, bring up file selection GUI.
		ofFileDialogResult openSongResult = ofSystemLoadDialog("Select a song");

		if (openSongResult.bSuccess) {
			openSongSelection(openSongResult);
		}
	}
}

void openVisualizer::onColorPickerEvent(ofxDatGuiColorPickerEvent color_picker_event) {
	//Updates visualizer_color or bass_color based on their color picker values.
	if (color_picker_event.target == visualizer_color_picker) {
		visualizer_color = visualizer_color_picker->getColor();
	}
	else if (color_picker_event.target == bass_color_picker) {
		bass_color = bass_color_picker->getColor();
	}
}

void openVisualizer::onSliderEvent(ofxDatGuiSliderEvent slider_event) {
	//Sets class variables below to their slider values.
	if (slider_event.target == spiral_speed_slider) {
		spiral_speed = spiral_speed_slider->getValue();
	}
	else if (slider_event.target == spectrum_bar_slider) {
		spectrum_width = spectrum_bar_slider->getValue();
	}
	else if (slider_event.target == bass_frequency_slider) {
		bass_frequency = bass_frequency_slider->getValue();
	}
}

void openVisualizer::onToggleEvent(ofxDatGuiToggleEvent toggle_event) {
	//Toggles draw_gradient based on GUI input.
	if (toggle_event.target == background_mode) {
		draw_gradient = background_mode->getChecked();
	}
	else if (toggle_event.target == spectrum_mode) {
		move_spectrum = spectrum_mode->getChecked();
	}
}

void openVisualizer::openSongSelection(ofFileDialogResult song_result) {
	ofFile song_file = (song_result.getPath()); //Gets the appropriate song file from song_result.

	if (ofToUpper(song_file.getExtension()) == "MP3" || ofToUpper(song_file.getExtension()) == "M4A") { //Checks if the file is an MP3 file.
		song_path = song_file.getAbsolutePath();
		song_title = song_file.getBaseName(); //Obtains song title for display in drawNowPlaying().
		music_player.load(song_path); //Loads the song based on its file path.
	}
}

void openVisualizer::update() {
	ofSoundUpdate(); //Updates the music player. Needed for continuous playback.

	float *value = ofSoundGetSpectrum(kBandSize); //Obtains the song spectrum and applies FFT Smoothing to remove noise.
	for (int i = 0; i < kBandSize; i++) {
		fft_smooth[i] *= kFftSmoothing;
		if (fft_smooth[i] < value[i]) {
			fft_smooth[i] = value[i];
		}
	}

	//Clears connectors to save memory.
	if (player_mode == VISUALIZER_MODE_PARTICLE) {
		particle_connector.clear();
	}

	else if (player_mode == VISUALIZER_MODE_SPECTRUM) {
		top_spectrum_connector.clear();
		bottom_spectrum_connector.clear();
	}

	else if (player_mode == VISUALIZER_MODE_BLOOM) {
		bloom_connector.clear();
	}
}

void openVisualizer::draw() {
	ofSetColor(visualizer_color); //Sets the color of the visualizer objects to slider values.

	if (draw_gradient) {
		ofBackgroundGradient(background_color, ofColor::black, OF_GRADIENT_CIRCULAR); //Draws the background with a color-changing gradient.
	}
	else {
		ofBackgroundGradient(visualizer_color, bass_color, OF_GRADIENT_CIRCULAR); //Draws the background with a grey gradient.
	}

	if (player_mode == VISUALIZER_MODE_SPECTRUM) {
		drawNowPlaying(player_width / 2, player_height / 2); //Draws the now playing information continuously centered in the middle.
	}
	else {
		drawNowPlaying(player_width / 2, kVerticalTitlePadding); //Draws the now playing information continuously centered in the middle-top.
	}

	ofSetColor(visualizer_color);

	switch (player_mode) {
		case VISUALIZER_MODE_CIRCLE:
			drawCircleVisualizer(player_width / 2, player_height / 2, kFftMultiplier); //Draws the circle visualizer if the player is in VISUALIZER_MODE_CIRCLE mode.
			drawBarVisualizer();
			break;
		case VISUALIZER_MODE_SPIRAL:
			drawSpiralVisualizer(player_width / 2, player_height / 2, kFftSpiralMultiplier); //Draws the spiral visuazlier if the player is in VISUALIZER_MODE_SPIRAL mode.
			drawBarVisualizer();
			break;
		case VISUALIZER_MODE_SPECTRUM:
			drawSpectrumVisualizer(); //Draws the spectrum visualizer if the player is in VISUALIZER_MODE_SPECTRUM mode.
			break;
		case VISUALIZER_MODE_PARTICLE:
			drawParticleVisualizer(); //Draws the particle visualizer if the player is in VISUALIZER_MODE_PARTICLE mode.
			break;
		case VISUALIZER_MODE_BLOOM:
			drawBloomVisualizer();
			break;
	}
}

void openVisualizer::drawNowPlaying(float x_position, float y_position) {
	ofSetColor(ofColor::white);
	string player_title = "Now Playing:";

	//Draws now playing information and centers text based on player width
	player_font.drawString(player_title, (x_position - (player_font.getStringBoundingBox(player_title, 0, 0).getRight() * 0.5f)), y_position);
	player_font.drawString(song_title, (x_position - (player_font.getStringBoundingBox(song_title, 0, 0).getRight() * 0.5f)), y_position + kTitlePadding);
}

void openVisualizer::drawBarVisualizer() {
	int fft_multiplier = player_width / 5; //Represents the multiplier to adjust bar height by to simulate beats in music.
	double curve_multiplier = 1.0; //Multiplier for rounding out bar visualizer.

	//Draws four bars on each corner of the screen based on bar height and spacing.
	
	ofNoFill();
	for (int i = 0; i <= player_height / 2 - kBarSize; i += kBarSpacing, fft_multiplier -= kBarSize, curve_multiplier -= 0.1) {
		for (int j = 0; j < kDrawAmount; j++) {
			ofDrawRectangle(ofRectangle(0, i, fft_smooth[j] * fft_multiplier * curve_multiplier, kBarSize));
			ofDrawRectangle(ofRectangle(player_width, i, -fft_smooth[j] * fft_multiplier * curve_multiplier, kBarSize));
			ofDrawRectangle(ofRectangle(0, player_height - i - kBarSize, fft_smooth[j] * fft_multiplier * curve_multiplier, kBarSize));
			ofDrawRectangle(ofRectangle(player_width, player_height - i - kBarSize, -fft_smooth[j] * fft_multiplier * curve_multiplier, kBarSize));
		}
	}
	ofFill();
}

void openVisualizer::drawCircleVisualizer(float x_position, float y_position, int fft_multiplier) {
	ofNoFill(); //Disables fill in order to show underlying circle visualizer objects.
	
	for (int i = 0; i < kDrawAmount; i++) {
		frequencyTransition(fft_smooth[i]);
		ofDrawCircle(x_position, y_position, fft_smooth[i] * fft_multiplier); //Draws circles to represent beats of the song.
	}
	
	ofFill(); //Enables fill again for bar visualizer objects.
}

void openVisualizer::drawSpiralVisualizer(float x_position, float y_position, int fft_multiplier) {
	for (int i = 0; i < kDrawAmount; i++) {
		//Represents x and y positions for points on the spiral that rotate in a circular path.
		//Rotate speed is adjusted with spiral speed slider.
		float x_pos = x_position + (y_position / 2 * (cos(ofGetElapsedTimef() * spiral_speed + fft_smooth[i])));
		float y_pos = y_position + (y_position / 2 * (sin(ofGetElapsedTimef() * spiral_speed + fft_smooth[i])));
		float x_pos_reversed = x_position + (y_position / 2 * (-cos(ofGetElapsedTimef() * spiral_speed + fft_smooth[i])));
		float y_pos_reversed = y_position + (y_position / 2 * (-sin(ofGetElapsedTimef() * spiral_speed + fft_smooth[i])));

		frequencyTransition(fft_smooth[i]);

		//Draws two spirals that rotate around the center of the window in opposite directions.
		ofNoFill();
		ofDrawCircle(x_pos, y_pos, fft_smooth[i] * fft_multiplier);
		ofDrawCircle(x_pos_reversed, y_pos_reversed, fft_smooth[i] * fft_multiplier);
		ofDrawCircle(x_pos, y_pos_reversed, fft_smooth[i] * fft_multiplier);
		ofDrawCircle(x_pos_reversed, y_pos, fft_smooth[i] * fft_multiplier);
		ofFill();
	}
}

void openVisualizer::drawSpectrumVisualizer() {
	for (int i = 0; i < player_width; i++) {
		ofDrawRectangle(ofRectangle(i * spectrum_width * 2, player_height, spectrum_width, -fft_smooth[i] * kFftMultiplier)); //Draws lower spectrum.
		ofDrawRectangle(ofRectangle(player_width - i * spectrum_width * 2, 0, spectrum_width, fft_smooth[i] * kFftMultiplier)); //Draws upper spectrum.

		//Adds vertices of rectangles to connectors so lines can be drawn.
		top_spectrum_connector.addVertex(player_width - i * spectrum_width * 2 + spectrum_width, fft_smooth[i] * kFftMultiplier);
		top_spectrum_connector.addVertex(player_width - i * spectrum_width * 2, fft_smooth[i] * kFftMultiplier);
		bottom_spectrum_connector.addVertex(i * spectrum_width * 2, player_height - fft_smooth[i] * kFftMultiplier);
		bottom_spectrum_connector.addVertex(i * spectrum_width * 2 + spectrum_width, player_height - fft_smooth[i] * kFftMultiplier);

		frequencyTransition(fft_smooth[i]);
	}

	//Draws connecting lines.
	top_spectrum_connector.draw();
	bottom_spectrum_connector.draw();
}

void openVisualizer::drawParticleVisualizer() {
	ofSetColor(background_color);
	for (int i = 0; i < kParticleAmount; i++) {
		float particle_x_pos = ofGetWidth() / 2 + (ofGetHeight() / 2.75) * fft_smooth[i] * kFftParticleMultiplier * cos(i); //x position of particle.
		float particle_y_pos = ofGetHeight() / 2 + (ofGetHeight() / 2.75) * fft_smooth[i] * kFftParticleMultiplier * sin(i); //y position of particle.

		frequencyTransition(fft_smooth[i]);
		ofDrawCircle(particle_x_pos, particle_y_pos, 5);
		background_color.setBrightness(kBackgroundHsbValue);
		
		//Adds vertices of particles to connectors so lines can be drawn.
		particle_connector.addVertex(ofPoint(particle_x_pos, particle_y_pos));
	}

	//Draws connecting lines.
	particle_connector.draw();

}

void openVisualizer::drawBloomVisualizer() {
	ofSetColor(background_color);
	for (int i = 0; i < kParticleAmount; i++) {
		float bloom_x_pos = ofGetWidth() / 2 + (ofGetHeight() / 2.75) * fft_smooth[i] * kFftBloomMultiplier * cos((double)(i * PI / 90)); //x position of particle.
		float bloom_y_pos = ofGetHeight() / 2 + (ofGetHeight() / 2.75) * fft_smooth[i] * kFftBloomMultiplier * sin((double)(i * PI / 90)); //y position of particle.

		frequencyTransition(fft_smooth[i]);
		background_color.setBrightness(kBackgroundHsbValue);

		//Adds vertices of particles to connectors so lines can be drawn.
		ofDrawCircle(ofPoint(bloom_x_pos, bloom_y_pos), 5);
		bloom_connector.addVertex(ofPoint(bloom_x_pos, bloom_y_pos));
	}
	bloom_connector.close();

	//Draws connecting lines.
	bloom_connector.draw();
}

void openVisualizer::frequencyTransition(float fft_value) {
	if (fft_value > bass_frequency) { //Switches color and changes gradient if bass frequency is reached.
		if (player_mode == VISUALIZER_MODE_CIRCLE || player_mode == VISUALIZER_MODE_SPIRAL) {
			ofSetColor(bass_color);
		}
		else if (player_mode == VISUALIZER_MODE_SPECTRUM) { //Alternates between kSpectrumUpperBound and kSpectrumLowerBound according to bass frequency and changes spectrum color.
			ofSetColor(background_color);

			if (move_spectrum) {
				if (spectrum_increase) {
					spectrum_width += kTransitionAmount;
					if (spectrum_width >= kSpectrumUpperBound) {
						spectrum_increase = false;
					}
				}
				else {
					spectrum_width -= kTransitionAmount;
					if (spectrum_width <= kSpectrumLowerBound) {
						spectrum_increase = true;
					}
				}
			}
		}
		else { //Changes particle color to be slightly lighter.
			background_color.setBrightness(255);
			ofSetColor(background_color);
		}

		background_color.setHue(hue_counter);
		hue_counter += kTransitionAmount;

		if (hue_counter >= kHueAmount) { //Resets the hue counter to 0 after the limit is reached to prevent integer overflow.
			hue_counter = 0;
		}
	}
}

void openVisualizer::keyPressed(int key) {
	switch (key) {
		case OF_KEY_F12: //Toggles fullscreen mode if F12 is pressed.
			ofToggleFullscreen();
			break;
		case '1': //Plays the song from the beginning.
			if (!music_player.isLoaded()) {
				break;
			}

			music_player.play();
			is_playing = true;
			break;
		case '2': //Toggles Play/Pause according to is_playing boolean flag.
			if (is_playing) {
				music_player.setPaused(true);
				is_playing = false;
			}
			else {
				music_player.setPaused(false);
				is_playing = true;
			}
			break;
		case '3': //Sets visualizer mode to circle.
			player_mode = VISUALIZER_MODE_CIRCLE;
			//Hides spectrum and spiral controls.
			spiral_speed_slider->setVisible(false);
			spectrum_bar_slider->setVisible(false);
			spectrum_mode->setVisible(false);
			break;
		case '4': //Sets visualizer mode to spiral.
			player_mode = VISUALIZER_MODE_SPIRAL;
			//Hides spectrum controls.
			spiral_speed_slider->setVisible(true);
			spectrum_bar_slider->setVisible(false);
			spectrum_mode->setVisible(false);
			break;
		case '5': //Sets visualizer mode to spectrum.
			player_mode = VISUALIZER_MODE_SPECTRUM;
			//Hides spiral controls.
			spectrum_bar_slider->setVisible(true);
			spectrum_mode->setVisible(true);
			spiral_speed_slider->setVisible(false);
			break;
		case '6': //Sets visualizer mode to particle.
			player_mode = VISUALIZER_MODE_PARTICLE;
			//Hides spectrum and spiral controls.
			spectrum_bar_slider->setVisible(false);
			spectrum_mode->setVisible(false);
			spiral_speed_slider->setVisible(false);
			break;
		case '7': //Sets visualizer mode to bloom.
			player_mode = VISUALIZER_MODE_BLOOM;
			spectrum_bar_slider->setVisible(false);
			spectrum_mode->setVisible(false);
			spiral_speed_slider->setVisible(false);
			break;
	}
}

void openVisualizer::windowResized(int w, int h) {
	//Adjusts player_height and player_width according to dimensions after resizing.
	player_height = h;
	player_width = w;
	player_gui->setPosition(player_width - kGUIPadding, player_height / 2);
}
