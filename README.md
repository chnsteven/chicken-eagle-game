## Assignment 1
### CWL: chsteven
### Name: Steven Chen
### Student number: 82507526

## Creative part
* 1. Summary:
	* Added two new assets along with all provided assets
	* Added feature to switch between "basic" and "advance" mode
* 2. Detail:
	* asset #1: Vortex
		* constant speed
		* spawns at random interval, random height from the right side of the screen.
		* has the component "Blower" and some other assets are set as "Blowable"
		* "Blower" can drag "Blowable" entities toward themselves
		* the drag is happening for every "BlowUpTimer"
		* During drag, "Blowable" is only allowed to rotate, not move.
		* disappears when vortex reaches left side of the screen. The drag also stops.
	* asset #2: Stone
		* constant speed
		* spawns at random interval, random width from the top side of the screen.
		* has the component "deadly" like the eagle
		* created for random scales
		* not affected by wind distortion
		* disappears when stone reaches bottom side of the screen.
