
//
// Copyright (c) 2019-2023 yanggaofeng
//

'use strict';


var datachannel=null;
var yang_hasDatachannel=false;

function yang_trace(msg){
		console.log("trace:"+msg);
}
function handleAnswer(msg){

		
}


function yang_fill_query(query_string, obj) {
            // pure user query object.
            obj.user_query = {};

            if (query_string.length === 0) {
                return;
            }

            // split again for angularjs.
            if (query_string.indexOf("?") >= 0) {
                query_string = query_string.split("?")[1];
            }

            var queries = query_string.split("&");
            for (var i = 0; i < queries.length; i++) {
                var elem = queries[i];

                var query = elem.split("=");
                obj[query[0]] = query[1];
                obj.user_query[query[0]] = query[1];
            }

            // alias domain for vhost.
            if (obj.domain) {
                obj.vhost = obj.domain;
            }
 }

function yang_url_parse(url) {
            // @see: http://stackoverflow.com/questions/10469575/how-to-use-location-object-to-parse-url-without-redirecting-the-page-in-javascri
            var a = document.createElement("a");
            a.href = url.replace("rtmp://", "http://")
                .replace("webrtc://", "https://")
                .replace("rtc://", "http://");

            var vhost = a.hostname.replace("[","").replace("]","");
            var app = a.pathname.substr(1, a.pathname.lastIndexOf("/") - 1);
            var stream = a.pathname.substr(a.pathname.lastIndexOf("/") + 1);
			//console.log("host: ", vhost);
            // parse the vhost in the params of app, that srs supports.
            app = app.replace("...vhost...", "?vhost=");
            if (app.indexOf("?") >= 0) {
                var params = app.substr(app.indexOf("?"));
                app = app.substr(0, app.indexOf("?"));

                if (params.indexOf("vhost=") > 0) {
                    vhost = params.substr(params.indexOf("vhost=") + "vhost=".length);
                    if (vhost.indexOf("&") > 0) {
                        vhost = vhost.substr(0, vhost.indexOf("&"));
                    }
                }
            }

            // when vhost equals to server, and server is ip,
            // the vhost is __defaultVhost__
            if (a.hostname === vhost) {
                var re = /^(\d+)\.(\d+)\.(\d+)\.(\d+)$/;
                if (re.test(a.hostname)) {
                    vhost = "__defaultVhost__";
                }
            }

            // parse the schema
            var schema = "rtmp";
            if (url.indexOf("://") > 0) {
                schema = url.substr(0, url.indexOf("://"));
            }

            var port = a.port;
            if (!port) {
                if (schema === 'http') {
                    port = 80;
                } else if (schema === 'https') {
                    port = 443;
                } else if (schema === 'rtmp') {
                    port = 1935;
                }
            }

            var ret = {
                url: url,
                schema: schema,
                server: a.hostname, port: port,
                vhost: vhost, app: app, stream: stream
            };
            yang_fill_query(a.search, ret);

            // For webrtc API, we use 443 if page is https, or schema specified it.
            if (!ret.port) {
                if (schema === 'webrtc' || schema === 'rtc') {
                    if (ret.user_query.schema === 'https') {
                        ret.port = 443;
                    } else if (window.location.href.indexOf('https://') === 0) {
                        ret.port = 443;
                    } else {
                        // For WebRTC, SRS use 1985 as default API port.
                        ret.port = 1985;
                    }
                }
            }

            return ret;
        }

// Depends on adapter-7.4.0.min.js from https://github.com/webrtc/adapter
// Async-await-promise based SRS RTC Player.
function SrsRtcPlayerAsync() {	
    var self = {};
    if(yang_mediaServerType==Yang_ConnectType_P2p)
		self.pc = new RTCPeerConnection({iceServers: yang_iceServer});
	else
		self.pc = new RTCPeerConnection(null);
	self.pc.onconnectionstatechange=function(event){
		console.log("connection state change: ", self.pc.connectionState);
	
	}
	
	
	self.pc.onicecandidate = async (ev) => {
		//console.log("pc.localDescription====\n"+self.pc.localDescription.sdp);
       // console.log('=======>' + JSON.stringify(ev.candidate));
        if(ev.candidate==null) return;   
		if(yang_mediaServerType==Yang_ConnectType_P2p){
			var candidateStr=JSON.stringify(ev.candidate);
			if(candidateStr.indexOf(".local")!=-1){
				console.log("=======>mdns ip,please config ice server ");
				return;
			}else{
				 console.log("=======>"+JSON.stringify(ev.candidate));
				sendCandidate(candidateStr);
			}
		}
    };
	
	//console.log("yang_mediaServerType==",yang_mediaServerType);
	if(yang_mediaServerType==Yang_ConnectType_Srs) yang_hasDatachannel=false;
	if(yang_hasDatachannel){
	
			datachannel=self.pc.createDataChannel('chat');
			datachannel.onopen = function(event) {
			console.log("datachannel onopen");
			}
			datachannel.onmessage = function(event) {
			  console.log("receive message: ", event.data);
			  $('#datachannel_recv').val(event.data);
			}
			datachannel.onerror=function(event) {
			  console.log("datachannel error: ", event.data);
			}
			datachannel.onclose=function(event) {
			  console.log("datachannel close: ");
			}
			
	}

	

	self.answerPlay = function(remoteSdp) {
        self.pc.addTransceiver("audio", {direction: "recvonly"});
        self.pc.addTransceiver("video", {direction: "recvonly"});	
		self.pc.setRemoteDescription(new RTCSessionDescription({type: 'offer', sdp: remoteSdp}));
  
        sdk.pc.createAnswer().then(
            function(description) {
                self.pc.setLocalDescription(description);
                sendMqttMsg(Yang_Request_Answer,null,description.sdp+"");
            }, yang_trace
        );
      
    };


	self.playP2p = async function() {   
        self.pc.addTransceiver("audio", {direction: "sendrecv"});
        self.pc.addTransceiver("video", {direction: "recvonly"});
		
		//var stream = await navigator.mediaDevices.getUserMedia({ video: false, audio: {
		//	sampleRate: 8000,
		//	echoCancellation: true, noiseSuppression: true, 
		//	autoGainControl: true,channelCount: 1
		//	}
		//});
		var stream = await navigator.mediaDevices.getUserMedia({ video: false, audio: true});
		stream.getTracks().forEach(function (track) {
            self.pc.addTrack(track,stream);
        });
		
        var offer = await self.pc.createOffer();
        await self.pc.setLocalDescription(offer);
        var session = await new Promise(function(resolve, reject) {    	 
			console.log("sdp"+offer.sdp);
           sendMqttMsg(Yang_Request_ConnectSdp,null,offer.sdp);	
        });
        
        session.simulator = conf.schema + '//' + conf.urlObject.server + ':' + conf.port + '/rtc/v1/nack/';

        return session;
    };
    

    // Close the player.
    self.close = function() {
		if(datachannel) {
			datachannel.close();
			datachannel=null;
		}
        self.pc && self.pc.close();
        self.pc = null;
        
    };

    // The callback when got remote track.
    // Note that the onaddstream is deprecated, @see https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/onaddstream
    self.ontrack = function (event) {
        // https://webrtc.org/getting-started/remote-streams
        self.stream.addTrack(event.track);
    };

    // Internal APIs.
    self.__internal = {
        defaultPath: '/rtc/v1/play/',
        prepareUrl: function (webrtcUrl) {
            var urlObject = yang_url_parse(webrtcUrl);
			var schema=chk_https.checked?"https:":"http:";
            var port = urlObject.port || 1985;
            if (schema === 'https:') {
                port = urlObject.port || 443;
            }

            // @see https://github.com/rtcdn/rtcdn-draft
            var api = urlObject.user_query.play || self.__internal.defaultPath;
            if (api.lastIndexOf('/') !== api.length - 1) {
                api += '/';
            }

            apiUrl = schema + '//' + urlObject.server + ':' + port + api;
            for (var key in urlObject.user_query) {
                if (key !== 'api' && key !== 'play') {
                    apiUrl += '&' + key + '=' + urlObject.user_query[key];
                }
            }
            // Replace /rtc/v1/play/&k=v to /rtc/v1/play/?k=v
            var apiUrl = apiUrl.replace(api + '&', api + '?');

            var streamUrl = urlObject.url;

            return {
                apiUrl: apiUrl, streamUrl: streamUrl, schema: schema, urlObject: urlObject, port: port,
                tid: Number(parseInt(new Date().getTime()*Math.random()*100)).toString(16).substr(0, 7)
            };
        }        
    };



    // Create a stream to add track to the stream, @see https://webrtc.org/getting-started/remote-streams
    self.stream = new MediaStream();

    // https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/ontrack
    self.pc.ontrack = function(event) {
        if (self.ontrack) {
            self.ontrack(event);
        }
    };

    return self;
}

// Format the codec of RTCRtpSender, kind(audio/video) is optional filter.
// https://developer.mozilla.org/en-US/docs/Web/Media/Formats/WebRTC_codecs#getting_the_supported_codecs
function SrsRtcFormatSenders(senders, kind) {
    var codecs = [];
    senders.forEach(function (sender) {
        var params = sender.getParameters();
        params && params.codecs && params.codecs.forEach(function(c) {
            if (kind && sender.track.kind !== kind) {
                return;
            }

            if (c.mimeType.indexOf('/red') > 0 || c.mimeType.indexOf('/rtx') > 0 || c.mimeType.indexOf('/fec') > 0) {
                return;
            }

            var s = '';

            s += c.mimeType.replace('audio/', '').replace('video/', '');
            s += ', ' + c.clockRate + 'HZ';
            if (sender.track.kind === "audio") {
                s += ', channels: ' + c.channels;
            }
            s += ', pt: ' + c.payloadType;

            codecs.push(s);
        });
    });
    return codecs.join(", ");
}

