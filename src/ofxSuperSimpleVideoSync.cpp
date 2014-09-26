//
//  ofxSuperSimpleVideoSync.cpp
//  ofxSuperSimpleVideoSync_builder
//
//  Created by Rui.Pereira on 9/10/14.
//
//

#include "ofxSuperSimpleVideoSync.h"

string statusFeedback;


void ofxSuperSimpleVideoSync::setup(string _path, bool _bIsServer, string _host, int _port, string _iD){
    
    
    // - - - - - - - - - - - - - -  COMM SETUP - - - - - - - - - -
    bIsServer = _bIsServer;

    if (bIsServer) {
        server.setup(_host, _port);
        modINFO._iD = "0";
        
        //test client status receiver
        client.setup(12121);
        
    } else{
        client.setup(_port);
        modINFO._iD = _iD;
        
        // test client status
        server.setup(_host, 12121);
    }
    
    // - - - - - - - - - - - - - -  MEDIA SETUP - - - - - - - - - -
    setupMedia(_path);              // setup media and loading
    bPlaying = true;
    
    font.loadFont("UIxtras/Lato-Light.ttf", 300);

}

// ------------------------------------
void ofxSuperSimpleVideoSync::setupClient(string _path, int _port){
    
    
    // - - - - - - - - - - - - - -  COMM SETUP - - - - - - - - - -
    client.setup(_port);
    bIsServer = false;
    
    // - - - - - - - - - - - - - -  MEDIA SETUP - - - - - - - - - -
    setupMedia(_path);              // setup media and loading
    bPlaying = true;
    
    font.loadFont("UIxtras/Lato-Light.ttf", 300);
    
}

// ------------------------------------
void ofxSuperSimpleVideoSync::setupMedia(string _path){
    
    dir.open(_path);
    dir.allowExt("mov");
    dir.listDir();
    
    if (dir.size() == 1) {
        
        ofxHapPlayer * temp = new ofxHapPlayer();
        temp->loadMovie(dir.getPath(0));
        videoPlayer.push_back(temp);
        
        bMultipleVideos = false;
        videoID = 0;
        cout << "single video"<<endl;
        
        
    } else {
        
        videosNUM = dir.size();
        
        for (int i = 0; i < dir.size(); i++) {
            
            ofxHapPlayer * temp = new ofxHapPlayer();
            temp->loadMovie(dir.getPath(i));
            videoPlayer.push_back(temp);
            
            bMultipleVideos = false;

            cout << "multiple videos"<<endl;
            cout <<dir.getPath(i)<<endl;
            
        }
    }
    
}

// ------------------------------------
void ofxSuperSimpleVideoSync::update(){
    
    videoPlayer[videoID]->update();

    if (bIsServer) {
        
        if ( videoPlayer[videoID]->isFrameNew() ) {

            ofxOscMessage fr;
            fr.setAddress("/movie/frame");
            fr.addIntArg(videoPlayer[videoID]->getCurrentFrame());
            server.sendMessage(fr);
        }
        
        // status feedback info
        if (bListClients)   getClientsDATA();       // get clients data and pull their ids into a list if its the first time running
        
    } else {
    
        // handle client data control coming from server
        clientCOMM();
    
        int diff = abs(videoPlayer[videoID]->getCurrentFrame() - syncPOSITION);
        
        if (diff > frameTHRESHOLD)      videoPlayer[videoID]->setFrame(syncPOSITION);           // sync video players if difference between server and client frames is larger than threshold
       
        
        if ( videoPlayer[videoID]->isFrameNew() ) {                                             // send status message (client iD  + current frame on client's video player)
            
            ofxOscMessage status;
            status.setAddress("/clientStatus/iD");
            status.addStringArg(modINFO._iD);
            status.addIntArg(videoPlayer[videoID]->getCurrentFrame());
            server.sendMessage(status);

        }
        
        // client check in
        clientCHECKIN();
    }

}

// ------------------------------------
void ofxSuperSimpleVideoSync::draw(){

    ofSetColor(255);
    
    videoPlayer[videoID]->draw(0, 0);
    
    if (bDEBUG) {
        string num;
        if (bIsServer)      num = ofToString(videoPlayer[videoID]->getCurrentFrame());
        else                num = ofToString(syncPOSITION);
        font.drawString(num, (ofGetWidth()-font.stringWidth(num))*.5, (ofGetHeight() - font.stringHeight(num)*.5));
        
        ofSetColor(255, 255, 0);
        ofDrawBitmapString(statusFeedback, 10,10);
    } 

}

// ------------------------------------
void ofxSuperSimpleVideoSync::keyCTRL(int _key){
    if (bIsServer) {
        
        switch(_key) {
                
            case 'p':
            {
                bPlaying =! bPlaying;
                if(!bPlaying)   play();
                else            stop();
                break;
            }
                
            case 'f':
            {
                bFULLSCREEN =! bFULLSCREEN;
                goFullScreen(bFULLSCREEN);
                
                break;
            }
                
            case 'd':
            {
                bDEBUG =! bDEBUG;
                showDebug(bDEBUG);

            }
                break;
                
            case 'l':
            {
                ofxOscMessage status;
                status.setAddress("clientStatus/setCheckin");
                status.addIntArg(0);
                server.sendMessage(status);
                bListClients =! bListClients;
                
            }
                break;
            case 'k':
                for (int i =0; i<clientLIST.size(); i++) cout << i << "   " << clientLIST[i]._iD <<endl;
                break;
        }
    }
    
}

// ------------------------------------
void ofxSuperSimpleVideoSync::showDebug(bool _bDebug){
   
    if (bIsServer) {
        
        ofxOscMessage ctrl;
        ctrl.setAddress("/ctrl/debug");
        ctrl.addIntArg(_bDebug);
        server.sendMessage(ctrl);
    }
    
    bDEBUG = _bDebug;
}

// ------------------------------------
void ofxSuperSimpleVideoSync::goFullScreen(bool _bFS){
    
    if (bIsServer) {
        
        ofxOscMessage ctrl;
        ctrl.setAddress("/ctrl/fs");
        ctrl.addIntArg(_bFS);
        server.sendMessage(ctrl);
    }
    
    ofSetFullscreen(_bFS);

}

// ------------------------------------
void ofxSuperSimpleVideoSync::onExit(){
    
    for (int i=0; i<videoPlayer.size(); i++) {
        videoPlayer[i]->close();
        delete videoPlayer[i];
        videoPlayer.erase(videoPlayer.begin()+i);
    }
    
    
}

// ------------------------------------
void ofxSuperSimpleVideoSync::clientCOMM(){

    // check for waiting messages
	while(client.hasWaitingMessages()){
        
		ofxOscMessage m;
		client.getNextMessage(&m);
        
		if(m.getAddress() == "/movie/frame"){                   // current frame data
            
            syncPOSITION = m.getArgAsInt32(0);
            
        } else if (m.getAddress() == "/movie/state"){           // movie state data
            
            if (m.getArgAsInt32(0) == 1 && !videoPlayer[videoID]->isPlaying())  play();
            else if(m.getArgAsInt32(0) == 0)                                    stop();
            
			//bPlaying = m.getArgAsInt32(0);
            
        } else if (m.getAddress() == "/ctrl/fs"){               // fullscreen control data
            
            ofSetFullscreen(m.getArgAsInt32(0));
            
        } else if (m.getAddress() == "/ctrl/debug"){            // debug info data
            
            bDEBUG = m.getArgAsInt32(0);
            
        } else if (m.getAddress() == "clientStatus/setCheckin"){            // debug info data
            
            bHasCheckedIn = false;
            
        } else {
			// unrecognized message: display on the bottom of the screen
			string msg_string;
			msg_string = m.getAddress();
			msg_string += ": ";
			for(int i = 0; i < m.getNumArgs(); i++){
				// get the argument type
				msg_string += m.getArgTypeName(i);
				msg_string += ":";
				// display the argument - make sure we get the right type
				if(m.getArgType(i) == OFXOSC_TYPE_INT32){
					msg_string += ofToString(m.getArgAsInt32(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
					msg_string += ofToString(m.getArgAsFloat(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
					msg_string += m.getArgAsString(i);
				}
				else{
					msg_string += "unknown";
				}
			}
            
		}
        
	}
    
    
}


// ------------------------------------
void ofxSuperSimpleVideoSync::gotoVideo(int _index){
    
    if( _index <= videosNUM && _index >= 0 ){
        stop();
        videoID = _index;
        play();
    } else {
        ofLog(OF_LOG_NOTICE, "the video index doesn't exist!  index requested:" + ofToString(_index) + " max index: "+ ofToString(videosNUM) );
    }

}

// ------------------------------------
void ofxSuperSimpleVideoSync::play(){
    
    if (!videoPlayer[videoID]->isPlaying()){
        if (bIsServer) {
        
            ofxOscMessage pl;
            pl.setAddress("/movie/state");
            pl.addIntArg(1);
            server.sendMessage(pl);
        }
        videoPlayer[videoID]->play();
        ofLog(OF_LOG_NOTICE, "P L A Y");

    } else {
    
        ofLog(OF_LOG_NOTICE, "already Playing");
    }

}
    
// ------------------------------------
void ofxSuperSimpleVideoSync::stop(){
    if (bIsServer) {
        ofxOscMessage pl;
        pl.setAddress("/movie/state");
        pl.addIntArg(0);
        server.sendMessage(pl);
    }
    videoPlayer[videoID]->stop();
    videoPlayer[videoID]->setFrame(0);
    
    ofLog(OF_LOG_NOTICE, "S T O P");
}

// ------------------------------------
void ofxSuperSimpleVideoSync::setPaused(bool _pause){
    videoPlayer[videoID]->setPaused(_pause);
}
    

// ------------------------------------
void ofxSuperSimpleVideoSync::getClientsDATA(){                 // get clients data and pull their ids into a list if its the first time running
    
    // check for waiting messages
	while(client.hasWaitingMessages()){
        
		ofxOscMessage cID;
		client.getNextMessage(&cID);
        
		if(cID.getAddress() == "/clientStatus/ping"){                                                           // status and id - have 2 params - 1 id 2 frame/status
            
            string pingID = cID.getArgAsString(0);
            int pingSTATUS = cID.getArgAsInt32(1);
            
            //ofLog(OF_LOG_NOTICE, "le Server ping'd by :"+ pingID);
            
            if (clientLIST.size()==0) {                                                                                 // if it's the first add to the ClientLIST  vector
                
                module temp;
                temp._iD = pingID;
                temp._status = pingSTATUS;
                clientLIST.push_back(temp);
                
                cout << " temp iD " <<temp._iD <<"//  " << "0" <<" //  c-iD" << clientLIST[0]._iD << endl;
            
            } else {
               
                for (int i = 0; i <clientLIST.size(); i++) {                                                            // else check if it exists in the vector and if not add it
                
                    if (clientLIST[i]._iD != pingID) {
                        
                        module temp;
                        temp._iD = pingID;
                        temp._status = pingSTATUS;
                        clientLIST.push_back(temp);
                        cout << " temp iD " <<temp._iD <<"//  " << i <<" //  c-iD" << clientLIST[i]._iD << endl;
                        
                    } else if(clientLIST[i]._iD == pingID) {                                                             // if it already exists just update its status
                        
                        clientLIST[i]._status = pingSTATUS;
                        cout << " c-iD" << clientLIST[i]._iD << " - :: "<< clientLIST[i]._status << endl;

                    }
                    
                }
            
            }
            
            
		}
        
	}
    
}

// ------------------------------------
void ofxSuperSimpleVideoSync::clientCHECKIN(){
    
    if (!bHasCheckedIn) {
        
        ofxOscMessage ping;
        ping.setAddress("/clientStatus/ping");
        ping.addStringArg(modINFO._iD);
        ping.addIntArg(modINFO._status);
        server.sendMessage(ping);
        
        
        bHasCheckedIn = true;
        
        ofLog(OF_LOG_NOTICE, "client "+ modINFO._iD + " ping'ing le Server");
        
    }
}

// ------------------------------------
bool ofxSuperSimpleVideoSync::sendClientMESSAGE(string _clientID, string _message, int _data){
    
    ofxOscMessage msg;
    msg.setAddress(_clientID + "__" + _message);
    msg.addIntArg(_data);
    server.sendMessage(msg);

    
}
    
