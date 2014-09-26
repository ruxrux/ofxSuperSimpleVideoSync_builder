//
//  ofxSuperSimpleVideoSync.h
//  ofxSuperSimpleVideoSync_builder
//
//  Created by Rui.Pereira on 9/10/14.
//
//

#ifndef __ofxSuperSimpleVideoSync__
#define __ofxSuperSimpleVideoSync__

#include "ofMain.h"

#include "ofxOsc.h"
#include "ofxHapPlayer.h"

struct module {
    string  _iD;
    string  _address;
    int     _status;
};

class ofxSuperSimpleVideoSync {

public:
    
    void setup(string _path, bool _bIsServer, string _host, int _port, string _iD);
    void setupClient(string _path, int _port);
    void setupMedia(string _path);
    
    void clientCOMM();
//    void getAllClientsSTATUS();
    void clientCHECKIN();
    void getClientsDATA();
    bool sendClientMESSAGE(string _clientID, string _message, int _data);


    void update();
    void draw();
    
    void onExit();
    
    void keyCTRL(int _key);
    void showDebug(bool _bDebug);
    void goFullScreen(bool _bFS);
    
    void play();
    void stop();
    void setPaused(bool _pause);
    void gotoVideo(int _which);
    
    int videosNUM;
    int syncPOSITION;
    int frameTHRESHOLD = 10;
    
private:
    
    // C T R L S
    bool bDEBUG;
    bool bFULLSCREEN;
    
    // M E D I A
    ofDirectory dir;
    vector <ofxHapPlayer *> videoPlayer;
    bool bMultipleVideos;
    int videoID;
    bool bPlaying;
    
    // U I
    ofTrueTypeFont font;
    
    // CLIENT / SERVER DATA
    ofxOscReceiver  client;
    ofxOscSender    server;
    bool bIsServer;
    bool bHasCheckedIn = true;
    module modINFO;
    vector<module>clientLIST;
    bool bListClients = false;

    

    

};

#endif