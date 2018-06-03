# openVisualizer Development

### __openVisualizer Objectives__
- [x] __Circle Visualization Mode__ (Completed 4/10/18)

- [x] __Spiral Visualization Mode__ (Completed 4/15/18)

- [x] __Basic GUI Functionality__ (Completed 4/18/18)

- [x] __Spectrum Visualization Mode__ (Completed 4/24/18)

- [x] __Support for Importing Music Files__ (Completed 4/26/18)

- [x] __Particle Visualization Mode__ (Completed 5/2/18)

- [x] __Bloom Visualization Mode__ (Completed 5/30/18)

- [x] __GUI Migration to ofxDatGui__ (Completed 4/27/18)

- [x] __Additional GUI Functionality__ (Completed 5/2/18)

### __openVisualizer Future Goals__

- [ ] __Equalizer Support__

- [ ] __3D Visualization Modes__

- [ ] __Additional Color Features__

### __Issues and Solutions__
- __Issue:__ Fullscreen mode causes massive lag. 

  - __Fix:__ Changed getHeight() and getWidth() calls to point to local variable that is updated when the window is resized.
  
- __Issue:__ Spirals will not rotate backwards. 

  - __Fix:__ Changed directions successfully by setting sin and cos to negative.
  
- __Issue:__ Player text not centered correctly when resizing. 

  - __Fix:__ Used local player width variable to center text based on current player width.
  
- __Issue:__ Visualizer spectrum mode does not extend/resize based on window resizing. 

  - __Fix:__ Changed spectrum loop to be dependent on window width instead of band size.
  
- __Issue:__ Spectrum bars' heights do not vary as a normal visualizer should. 

  - __Fix:__ Changed spectrum draw method to draw a bar for certain frequency values using a loop, creating bars of variable heights.
  
- __Issue:__ Bar/Circle colors do not change when using GUI. 

  - __Fix:__ Placed ofSetColor calls based on GUI values after drawNowPlaying, which had set all colors to white.
  
- __Issue:__ Gradient sometimes changes too fast when hitting certain frequencies. 

  - __Fix:__ Smoothed out gradient by reducing counter increment value to 0.5.

- __Issue:__ Counter to determine hue color reaches integer overflow on long songs.

  - __Fix:__ Added in code to reset the counter to zero once it reaches the hue amount limit.

- __Issue:__ Particle connector for lower spectrum visualizer does not display connecting lines.

  - __Fix:__ Changed location of drawn lines to properly align with lower visualizer bars.
