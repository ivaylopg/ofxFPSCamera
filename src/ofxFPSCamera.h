/*
 *
 *  ofxFPSCamera.h
 *
 *  by Ivaylo Getov, 2014
 *
 *
 *
 *  Derived from ofxGameCamera, created by James George and FlightPhase
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

class ofxFPSCamera : public ofCamera {
  public:
	
	ofxFPSCamera();
    ~ofxFPSCamera();
    
	void setup();
    void update(ofEventArgs& args);
    void keyPressed(ofKeyEventArgs& args);
    void keyReleased(ofKeyEventArgs& args);
	
	//tweak controls
	float sensitivityX;
	float sensitivityY;
	float speed;
	
	//enable, disable
	bool usemouse;
	bool useArrowKeys;
	bool applyTranslation;
	
	//saving loading
	bool autosavePosition;
	void saveCameraPosition();
	void loadCameraPosition();

	//adjusting through code manually
	void movedManually();               //call this whenever you update orientation or position manually
    void reset();                       //sets the camera back to zero
    void reset(float h);
    void reset(float x, float y, float z);
    void reset(float x, float y, float z, float h);
    void reset(ofVec3f v);
    void reset(ofVec3f v, float h);
    void target(ofVec3f v);
    ofVec3f getTarget();
    void enableMove();
    void disableMove();
    
    void enableStrafe();
    void disableStrafe();
    

    
    void setCamHeight(float ch);
    float camHeight;
    
    void setMinMaxY(float angleDown, float angleUp);
    float totUpDown;
    float upAngle;
    float downAngle;
    
    bool keepTurning;
    float accel;
    float speedMod;
    bool easeIn;
    
    
protected:
    
	bool eventsRegistered;
    bool strafe;
    
	ofVec3f currentUp;
	ofVec3f currLookTarget;
	
	ofQuaternion lastRot;
	ofVec3f lastPos;

	void updateRotation();

	ofVec2f lastMouse;
	bool justResetAngles;
	bool unsavedChanges;
	
	string cameraPositionFile;
    
    bool movKey;
    bool forw;
    bool back;
    bool left;
    bool rigt;
	
};
