#ofxFPSCamera#
---

ofxFPSCamera creates a camera object that behaves like the camera in a first-person shooter, controlled by either the W, A, S, D keys (or the arrow keys) and the mouse.

It is derived from [ofxGameCamera by James George](https://github.com/Flightphase/ofxGameCamera) and initially it was a fork of that project, but I think its intent is different enough that it should be a standalone addon.

The major differences between ofxFPSCamera and ofxGameCamera are that ofxFPSCamera:

* Defaults to using the mouse (you do not need to click)
* Got rid of ability to rotate on camera Z-axis
* *ALWAYS maintain UP direction relative to world coordinates* (camera will always maintain a level horizon)
* Got rid of direct control of boom up/down (camera height can be set by your app via a method, but cannot be directly controlled by the user pressing a key)
* Limits up/down look range. (Defaults to +/- 30deg)
* "Infinite mouse" lets user keep looking left or right forever (has not been tested on Windows)

See included example for complete list of methods and variables.


----
Requires __ofxXmlSettings__ to make use of the original ofxGameCamera save/load capability.