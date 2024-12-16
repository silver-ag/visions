# Visions

Visions is a VCV Rack 2 plugin for video synthesis.

## Modules

### VideoOut

The VideoOut module draws input data on the screen. It has two sets of inputs - a position input and a colour input, and in each frame it draws a pixel of the specified colour at the specified point. These input sets can come seperately or all together in a polyphonic cable (if both are connected, the polyphonic input takes precedence). All values are expected to be bipolar, -5v to 5v. The switch under the colour input labels picks whether to interpret the colour inputs as RGB or HSV. The 'clear' button resets the screen to grey, and the 'resolution' knob adjusts the resolution of the display.

### ImageIn

The ImageIn module reads sound data from an image file. The image file is selected by pressing the load button. It has one set of inputs - a position, and one set of outputs - a colour. Both can be used seperately or polyphonically (if both inputs are connected, the polyphonic input takes precedence). The module reads the colour of the pixel at the given position and gives that data as output, -5v to 5v.

The ImageIn module only supports PNG images, and only some PNG images. If VCV crashes while trying to load an image, you can safely restart without clearing the patch, as the problem is only with that file. Even some images that load safely come out mangled.

## PathGenerator [Unimplemented]

The PathGenerator module is a kind of pair of LFOs configured to cover every possible pair of values up to the specified resolution. Its simplest intended purpose is to be plugged into the position inputs of both an ImageIn and a VideoOut, to scan across both in sync.
