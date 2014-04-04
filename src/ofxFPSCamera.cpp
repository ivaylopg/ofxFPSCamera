/*
 *
 *  ofxFPSCamera.cpp
 *
 *  by Ivaylo Getov, 2014
 *
 *
 *
 *  Derived from ofxGameCamera, created by James George and FlightPhase
 *
 */

#include "ofxFPSCamera.h"


ofxFPSCamera::ofxFPSCamera() {
	unsavedChanges = false;
	sensitivityX = 0.10f;
	sensitivityY = 0.10f;

	speed = 8.0f;
	
	lastMouse = ofVec2f(0,0);
    
	usemouse = true;
	autosavePosition = false;
	useArrowKeys = false;
	applyTranslation = true;
    
	justResetAngles = false;

	
	cameraPositionFile =  "_gameCameraPosition.xml";
    eventsRegistered = false;
    
    camHeight = 0.0;
    totUpDown = 0.0;
    
    upAngle = -30.0;  // must be negative
    downAngle = 30.0; // must be positive
    
    keepTurning = false;
    
    easeIn = true;
    speedMod = 0.1;
    accel = 0.3;
    
    movKey = false;
    forw = false;
    back = false;
    left = false;
    rigt = false;
    strafe = true;
}
ofxFPSCamera::~ofxFPSCamera(){
    if(eventsRegistered){
        ofAddListener(ofEvents().update, this, &ofxFPSCamera::update);
        ofAddListener(ofEvents().keyPressed, this, &ofxFPSCamera::keyPressed);
        ofAddListener(ofEvents().keyReleased , this, &ofxFPSCamera::keyReleased);
        eventsRegistered = false;
    }
}

void ofxFPSCamera::setup(){
    if(!eventsRegistered){
        ofAddListener(ofEvents().update, this, &ofxFPSCamera::update);
        ofAddListener(ofEvents().keyPressed, this, &ofxFPSCamera::keyPressed);
        ofAddListener(ofEvents().keyReleased , this, &ofxFPSCamera::keyReleased);
        eventsRegistered = true;
    }
    
    /***********************************************************/
    // Had to add this for some reason or it wouldn't let me start fresh without loading an xml
    // -ivaylo
    if (!autosavePosition) {
        reset();
    }
    /***********************************************************/
    
    
}

void ofxFPSCamera::update(ofEventArgs& args){
	bool rotationChanged = false;
	bool positionChanged = false;
	
	//lastRot == lastRot << this is nananers check
	if( (lastRot == lastRot && lastRot != getOrientationQuat()) ||
	    (lastPos == lastPos && lastPos != getPosition()))
	{
		movedManually();
	}

	if(applyTranslation){
		if(ofGetKeyPressed('w') || (useArrowKeys && ofGetKeyPressed(OF_KEY_UP)) ){
			if (easeIn) {
                dolly(-speedMod);
                speedMod+=accel;
                if (speedMod>speed) {speedMod=speed;}
            } else {
                dolly(-speed);
            }
            ofVec3f curPos(getPosition());                                      //Constant camera height
            curPos.y = camHeight;                                               //
            setPosition(curPos);                                                //
			positionChanged = true;
		}
		
		if(ofGetKeyPressed('s') || (useArrowKeys && ofGetKeyPressed(OF_KEY_DOWN)) ){
            if (easeIn) {
                dolly(speedMod);
                speedMod+=accel;
                if (speedMod>speed) {speedMod=speed;}
            } else {
                dolly(speed);
            }
            ofVec3f curPos(getPosition());                                      //Constant camera height
            curPos.y = camHeight;                                               //
            setPosition(curPos);                                                //
			positionChanged = true;
		}
		
		if((ofGetKeyPressed('a') || (useArrowKeys && ofGetKeyPressed(OF_KEY_LEFT))) && strafe ){
            if (easeIn) {
                truck(-speedMod);
                speedMod+=accel;
                if (speedMod>speed) {speedMod=speed;}
            } else {
                truck(-speed);
            }
			positionChanged = true;
		}
		
		if((ofGetKeyPressed('d') || (useArrowKeys && ofGetKeyPressed(OF_KEY_RIGHT))) && strafe ){
            if (easeIn) {
                truck(speedMod);
                speedMod+=accel;
                if (speedMod>speed) {speedMod=speed;}
            } else {
                truck(speed);
            }
			positionChanged = true;
		}
        
	}
	
	if(positionChanged){
		currLookTarget = getPosition() + getLookAtDir();
	}
    
    ofVec2f mouse;
    if (usemouse) {
        mouse = ofVec2f(ofGetMouseX(), ofGetMouseY());
    } else {
        mouse = ofVec2f(0, 0);
    }
    
    //ofVec2f mouse( ofGetMouseX(), ofGetMouseY() );
	if(!justResetAngles){
		if(usemouse){
            float dx = (mouse.x - lastMouse.x) * sensitivityX;
			float dy = (mouse.y - lastMouse.y) * sensitivityY;
            
            if (keepTurning && ofGetElapsedTimeMillis()>2000) {                 // when mouse hits edge of screen, keep turning in that direction
                if (dx==0 && mouse.x <= -1 * (ofGetWindowPositionX() - 5) ) {
                    dx = -speed/2 * sensitivityX;
                }
                if (dx==0 && mouse.x >= (ofGetScreenWidth() - ofGetWindowPositionX() - 5)) {
                    dx = speed/2 * sensitivityX;
                }
            }
            
            if (totUpDown+dy > downAngle) {                                     // Limit up/down look. Defaults to +/- 30deg
                //dy = 0;
                dy = downAngle - totUpDown;
                totUpDown+=dy;
            } else if (totUpDown+dy < upAngle) {
                //dy = 0;
                dy = upAngle - totUpDown;
                totUpDown+=dy;
            } else {
                totUpDown+=dy;
            }

			currLookTarget.rotate(-dx, getPosition(), currentUp);
			ofVec3f sideVec = (currentUp).getCrossed(currLookTarget - getPosition());
			lookAt(currLookTarget, currentUp);

			currLookTarget.rotate(dy, getPosition(), sideVec);
            currentUp = ofVec3f(0,1,0);                                         // Maintain UP direction relative to world coordinates
            lookAt(currLookTarget, currentUp);

			rotationChanged = true;
		}
	}
	lastMouse = mouse;
    justResetAngles = false;
	lastRot = getOrientationQuat();
	lastPos = getPosition();
    
    /***********************************************************/
    // Infinite mouse AKA don't limit mouse at ends of screen. This needs improvement.

    #if defined( TARGET_OSX )
    //cout << -1 * ofGetWindowPositionX() << " : " << ofGetMouseX() << " : " << (ofGetScreenWidth() - ofGetWindowPositionX()) << endl;
    if (usemouse && !keepTurning && ofGetMouseX() >= (ofGetScreenWidth() - ofGetWindowPositionX())-5 && ofGetElapsedTimeMillis()>2000) {
        //cout << "!!!!!!!!" << endl;
        CGPoint point;
        point.y = ofGetWindowPositionY() + ofGetMouseY();
        point.x = ofGetWindowPositionX() + ofGetWidth()/2;
        CGWarpMouseCursorPosition(point);
        CGAssociateMouseAndMouseCursorPosition(true);
        lastMouse.x = ofGetMouseX();
        mouse.x = ofGetMouseX();
        mouse.y = ofGetMouseY();
        justResetAngles = true;
    }
    if (usemouse && !keepTurning && ofGetMouseX() <= -1 * (ofGetWindowPositionX()-5) && ofGetElapsedTimeMillis()>2000) {
        //cout << ofGetMouseY() << endl;
        CGPoint point;
        point.y = ofGetWindowPositionY() + ofGetMouseY();
        point.x = ofGetWindowPositionX() + ofGetWidth()/2;
        CGWarpMouseCursorPosition(point);
        CGAssociateMouseAndMouseCursorPosition(true);
        lastMouse.x = ofGetMouseX();
        mouse.x = ofGetMouseX();
        mouse.y = ofGetMouseY();
        justResetAngles = true;
    }
    #endif
    
    #if defined( TARGET_WIN32 )
    // I haven't tested the code below on Windows yet.
    // CGWarpMouseCursorPosition(point) will not work, but I am told that SetCursorPos(x, y) should work instead...
    
//    if (usemouse && !keepTurning && ofGetMouseX() >= (ofGetScreenWidth() - ofGetWindowPositionX())-5 && ofGetElapsedTimeMillis()>2000) {
//        SetCursorPos((ofGetWindowPositionX() + ofGetWidth()/2), (ofGetWindowPositionY() + ofGetMouseY()));
//        lastMouse.x = ofGetMouseX();
//        mouse.x = ofGetMouseX();
//        mouse.y = ofGetMouseY();
//        justResetAngles = true;
//    }
//    if (usemouse && !keepTurning && ofGetMouseX() <= -1 * (ofGetWindowPositionX()-5) && ofGetElapsedTimeMillis()>2000) {
//        SetCursorPos((ofGetWindowPositionX() + ofGetWidth()/2), (ofGetWindowPositionY() + ofGetMouseY()));
//        lastMouse.x = ofGetMouseX();
//        mouse.x = ofGetMouseX();
//        mouse.y = ofGetMouseY();
//        justResetAngles = true;
//    }
    
    #endif
    
    #if defined( TARGET_LINUX )
        //Not sure how to do this here. Any ideas?
    #endif
    
    // End Infinite Mouse
    /***********************************************************/

    
	//did we make a change?
	unsavedChanges |= positionChanged || rotationChanged;
	
	//if we didnt make a change this frame, save. this helps not saving every frame when moving around
	if( !rotationChanged && !positionChanged && unsavedChanges && autosavePosition){
		saveCameraPosition();
	}
	/*
	 cout << "STATS:" << endl
	 << "	position" << getPosition() << endl
	 << "	current up vec " << currentUp << endl
	 << "	current look " << currLookTarget << endl
	 << "	vec to look " << (currLookTarget-getPosition()) << endl;
	 */
	
}

void ofxFPSCamera::keyPressed(ofKeyEventArgs& args){
    if ((args.key == 119 && !useArrowKeys) || (args.key == OF_KEY_UP && useArrowKeys)) {
        forw = true;
    }
    if ((args.key == 97 && !useArrowKeys) || (args.key == OF_KEY_DOWN && useArrowKeys)) {
        back = true;
    }
    if ((args.key == 115 && !useArrowKeys) || (args.key == OF_KEY_LEFT && useArrowKeys)) {
        left = true;
    }
    if ((args.key == 100 && !useArrowKeys) || (args.key == OF_KEY_RIGHT && useArrowKeys)) {
        rigt = true;
    }
    
    if (forw || back || left || rigt) {
        movKey = true;
    }
}

void ofxFPSCamera::keyReleased(ofKeyEventArgs& args){
    if (easeIn) {
        if ((args.key == 119 && !useArrowKeys) || (args.key == OF_KEY_UP && useArrowKeys)) {
            forw = false;
        }
        if ((args.key == 97 && !useArrowKeys) || (args.key == OF_KEY_DOWN && useArrowKeys)) {
            back = false;
        }
        if ((args.key == 115 && !useArrowKeys) || (args.key == OF_KEY_LEFT && useArrowKeys)) {
            left = false;
        }
        if ((args.key == 100 && !useArrowKeys) || (args.key == OF_KEY_RIGHT && useArrowKeys)) {
            rigt = false;
        }
        
        if (forw || back || left || rigt) {
            movKey = true;
        } else {
            movKey = false;
            speedMod = 0.1;
        }
    }
}

void ofxFPSCamera::enableStrafe(){
    strafe = true;
}

void ofxFPSCamera::disableStrafe(){
    strafe = false;
}


void ofxFPSCamera::movedManually(){
	currLookTarget = getPosition() + getLookAtDir();
    currentUp = ofVec3f(0,1,0);                             // Maintain UP direction relative to world coordinates
	justResetAngles = true;
}

void ofxFPSCamera::setCamHeight(float ch){
    camHeight = ch;
}

void ofxFPSCamera::setMinMaxY(float angleDown, float angleUp){
    downAngle = abs(angleDown);
    upAngle = -1 * abs(angleUp);
}

void ofxFPSCamera::reset(){
    camHeight = 0.0;
    setPosition(ofVec3f(0,0,0));
	currLookTarget = ofVec3f(0,0,1);
    currentUp = ofVec3f(0,1,0);
    lookAt(currLookTarget, currentUp);
    setOrientation(ofQuaternion());
	movedManually();
}

void ofxFPSCamera::reset(float h){
    camHeight = h;
	setPosition(ofVec3f(0,0,0));
	currLookTarget = ofVec3f(0,0,1);
    currentUp = ofVec3f(0,1,0);
    lookAt(currLookTarget, currentUp);
    //setOrientation(ofQuaternion());
	movedManually();
}

void ofxFPSCamera::reset(float x, float y, float z){
    camHeight = 0.0;
	setPosition(ofVec3f(x,y,z));
	currLookTarget = getPosition() + ofVec3f(0,0,1);
    currentUp = ofVec3f(0,1,0);
    lookAt(currLookTarget, currentUp);
    //setOrientation(ofQuaternion());
	movedManually();
}

void ofxFPSCamera::reset(float x, float y, float z, float h){
    camHeight = h;
	setPosition(ofVec3f(x,y,z));
	currLookTarget = getPosition() + ofVec3f(0,0,1);
    currentUp = ofVec3f(0,1,0);
    lookAt(currLookTarget, currentUp);
    //setOrientation(ofQuaternion());
	movedManually();
}

void ofxFPSCamera::reset(ofVec3f v){
    camHeight = 0.0;
	setPosition(v);
	currLookTarget = getPosition() + ofVec3f(0,0,1);
    currentUp = ofVec3f(0,1,0);
    lookAt(currLookTarget, currentUp);
    //setOrientation(ofQuaternion());
	movedManually();
}

void ofxFPSCamera::reset(ofVec3f v, float h){
    camHeight = h;
	setPosition(v);
	currLookTarget = getPosition() + ofVec3f(0,0,1);
    currentUp = ofVec3f(0,1,0);
    lookAt(currLookTarget, currentUp);
    //setOrientation(ofQuaternion());
	movedManually();
}

void ofxFPSCamera::target(ofVec3f v){
    currLookTarget = ofVec3f(v.x,v.y,v.z);
    lookAt(currLookTarget, currentUp);
    movedManually();
}

ofVec3f ofxFPSCamera::getTarget(){
    return currLookTarget;
}

void ofxFPSCamera::enableMove () {
    usemouse = true;
    applyTranslation = true;
}

void ofxFPSCamera::disableMove () {
    usemouse = false;
    applyTranslation = false;
}


// SAVE and LOAD functions below are unchanged from ofxGameCamera:

void ofxFPSCamera::saveCameraPosition()
{
	ofxXmlSettings savePosition;
	savePosition.addTag("camera");
	savePosition.pushTag("camera");

	savePosition.addTag("position");
	savePosition.pushTag("position");
	savePosition.addValue("X", getPosition().x);
	savePosition.addValue("Y", getPosition().y);
	savePosition.addValue("Z", getPosition().z);
	savePosition.popTag(); //pop position

	savePosition.addTag("up");
	savePosition.pushTag("up");
	savePosition.addValue("X", currentUp.x);
	savePosition.addValue("Y", currentUp.y);
	savePosition.addValue("Z", currentUp.z);
	savePosition.popTag(); //pop up

	savePosition.addTag("look");
	savePosition.pushTag("look");
	savePosition.addValue("X", currLookTarget.x);
	savePosition.addValue("Y", currLookTarget.y);
	savePosition.addValue("Z", currLookTarget.z);
	savePosition.popTag(); //pop look
	
	savePosition.addValue("FOV", getFov());

	savePosition.popTag(); //camera;

	savePosition.saveFile(cameraPositionFile);
	
	unsavedChanges = false;
}

void ofxFPSCamera::loadCameraPosition()
{
	ofxXmlSettings loadPosition;
	cout << "Camera pos " << cameraPositionFile << endl;
	if(loadPosition.loadFile(cameraPositionFile)){
//		reset();
//		loadPosition.pushTag("camera");
		cout << "loaded camera pos" << endl;
		// tig: defaulting with floats so as to get floats back.
		setPosition(ofVec3f(loadPosition.getValue("camera:position:X", 0.),
							loadPosition.getValue("camera:position:Y", 0.),
							loadPosition.getValue("camera:position:Z", 0.)));
		
		currentUp = ofVec3f(loadPosition.getValue("camera:up:X", 0.),
							loadPosition.getValue("camera:up:Y", 1.),
							loadPosition.getValue("camera:up:Z", 0.));

		currLookTarget = ofVec3f(loadPosition.getValue("camera:look:X", 0.),
								 loadPosition.getValue("camera:look:Y", 1.),
								 loadPosition.getValue("camera:look:Z", 0.));

		float fov = loadPosition.getValue("camera:FOV", -1.0);
		if(fov != -1.0){
			setFov(fov);
		}

		lookAt(currLookTarget, currentUp);
		movedManually();
		
	}
	else{
		ofLog(OF_LOG_ERROR, "ofxFPSCamera: couldn't load position!");
	}

}
