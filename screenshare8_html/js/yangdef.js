
//
// Copyright (c) 2019-2023 yanggaofeng
//


function makeid() {
	var text = "";
	var possible = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

	for (var i = 0; i < 12; i++)	text += possible.charAt(Math.floor(Math.random() * possible.length));
	return text; //+text;
}
var yang_clientId=makeid();
var yang_deviceId=makeid();
var yang_subTopic=yang_clientId;//"metaIpc/client";
var yang_pubTopic="metaIpc/server";

var yang_url=null;
var yang_uid=-1;
var yang_sfu_isHtpps=false;
var yang_mqtt_isWss=false;
var yang_isPlaying=false;
var yang_isSetinterval=false;
var yang_playType=0;
var sdk = null;
var yangInterval;
const yangIntervalTime             = 5000;
const Yang_ConnectType_P2p         = 0;       
const Yang_ConnectType_Srs         = 1;    
const Yang_ConnectType_Zlm         = 2;      
const Yang_ConnectType_Datachannel = 3;  
const Yang_Request_isConnected     = -3;
const Yang_Request_NotLogin        = -2;
const Yang_Request_Error           = -1;
const Yang_Request_Login	 	   = 0;
const Yang_Request_LoginResponse   = 1;
const Yang_Request_Connect 	       = 2;
const Yang_Request_ConnectSdp      = 3;
const Yang_Request_Answer          = 4;
const Yang_Request_Candidate       = 5;
const Yang_Request_SfuSuccess      = 6;
const Yang_Request_SfuFail         = 7;
const Yang_Request_Close           = 8;
const Yang_Request_Event           = 9;
const Yang_Request_Ping            = 10;

var yang_mediaServerType=Yang_ConnectType_P2p;


var yang_iceServer =[
    {
        urls: ["stun:127.0.0.1:3478"]
    }, {
        urls: ["turn:127.0.0.1:3478"],
        username: "metaRTC",
        credential: "metaRTC"
    }
];

