#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
   
    string id = "el 2";
//    
//    serverTEST.setup("videos", false, HOST, 12345, id);
//    ofSetWindowTitle("le Client!  (local)  :: " + ofToString(id));

//    
     serverTEST.setup("videos", true, HOST, 12345,"0");
     ofSetWindowTitle("le Server!  (local)");

     
    
}

//--------------------------------------------------------------
void ofApp::update(){
    serverTEST.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    serverTEST.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    serverTEST.keyCTRL(key);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::exit(){
    serverTEST.onExit();
    cout<< "quitting"<<endl;
}