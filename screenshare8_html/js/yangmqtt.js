
//
// Copyright (c) 2019-2023 yanggaofeng
//

'use strict';


	var yang_client = null;
	var yang_connected = false;	
	var yang_isLogon = false;
	
	function setMediaServer(mstype){
	yang_playType=mstype;
		//switch(mstype){
		//case 0:yang_mediaServerType=Yang_ConnectType_P2p; yang_set_showdiv(0);break;	
		//case 1:yang_mediaServerType=Yang_ConnectType_Srs; yang_set_showdiv(1);break;	
		//case 2:yang_mediaServerType=Yang_ConnectType_Zlm; yang_set_showdiv(1);break;	
		//}
		return true;
	
	}

	function sendLoginMsg(){
		if(!yang_connected) return;
		  var data = {
		 cid: yang_clientId,connectType:Yang_ConnectType_P2p,uid:yang_uid,
		   requestType: Yang_Request_Login, url:yang_url,isHttps:yang_sfu_isHtpps,
		   data: null
		};
 
		//text/plain application/json
		yang_publish(JSON.stringify(data));
	}
	function sendMqttPing(){
	
		if(!yang_connected) return;
		  var data = {
		 cid: yang_clientId,connectType:Yang_ConnectType_P2p,uid:yang_uid,
		   requestType: Yang_Request_Ping, url:yang_url,isHttps:yang_sfu_isHtpps,
		   data: null
		};
 
		//text/plain application/json
		yang_publish(JSON.stringify(data));
		
	}
	function sendMqttMsg(prequestType,url,sdp){
		if(!yang_connected) return;
		  var data = {
		 cid: yang_clientId,connectType:yang_mediaServerType,uid:yang_uid,
		   requestType: prequestType, url:yang_url,isHttps:yang_sfu_isHtpps,
		   data: sdp
		};	
		
		//text/plain application/json
		yang_publish(JSON.stringify(data));
	}
	
	function sendCandidate(candidateStr){
		if(!yang_connected || yang_mediaServerType!=Yang_ConnectType_P2p) return;
		  var data = {
		 cid: yang_clientId,connectType:Yang_ConnectType_P2p,uid:yang_uid,
		   requestType: Yang_Request_Candidate, url:yang_url,isHttps:yang_sfu_isHtpps,
		   data: candidateStr
		};
 
		//text/plain application/json
		yang_publish(JSON.stringify(data));
	}
	
	function sendDataMsg(msg){
		if(!yang_connected) return;
		  var data = {
		 cid: yang_clientId,connectType:Yang_ConnectType_Datachannel,uid:yang_uid,
		   requestType: Yang_Request_Event, url:null,
		   data: msg
		};
 
		//text/plain application/json
		yang_publish(JSON.stringify(data));
	}

	// called when the client connects
	function yang_onConnect() {
	// Once a connection has been made, make a subscription and send a message.
		console.log("onConnect");
		yang_client.subscribe(yang_subTopic);
		yang_connected=true;
		sendLoginMsg();
		
	}

	// called when the client loses its connection
	function yang_onConnectionLost(responseObject) {
			yang_connected=false;
			console.warn("A lost connection will automatically reconnect",responseObject);
	}

	// called when a message arrives
	function yang_onMessageArrived(message) {
		let msg = JSON.parse(message.payloadString);
		
		if(msg.requestType<0){
		
			if(msg.requestType==Yang_Request_NotLogin){
				console.log("Not Login============>");
				//sdk.close();
				//alert("Not Login!");
			}
			if(msg.requestType==Yang_Request_isConnected){
			
				sdk.close();
				alert("connect fail! please close other peer connect!");		
			}
			return;
		}
		if(msg.requestType==Yang_Request_LoginResponse) {
			yang_uid=msg.uid;		
			console.log("uid============>"+yang_uid);
			yang_start_interval();
			if(yang_mediaServerType==Yang_ConnectType_P2p){
				yang_startPlayP2p();	
				return;
			}
			sendMqttMsg(Yang_Request_Connect,yang_url,null);	
					
			return;
		}
	
		if(msg.connectType==Yang_ConnectType_P2p){
		 switch (msg.requestType) {
            case Yang_Request_Connect:  break;
            case Yang_Request_ConnectSdp: {
				yang_answerPlay(msg.data);
				break;
			} 
            case Yang_Request_Answer:  {	
				if(yang_playType==1)	
					console.log("url===="+msg.url);			
				console.log(",sdp=="+msg.data);
				if(sdk) sdk.pc.setRemoteDescription(new RTCSessionDescription({type: 'answer', sdp: msg.data}));
				break;
			}
			case Yang_Request_Candidate:{			
				if(sdk) {
					var candidateOjb=JSON.parse(msg.data);
					console.log("remote candidate=="+candidateOjb.candidate);
					sdk.pc.addIceCandidate(new RTCIceCandidate(candidateOjb)).then(_=>{
					  console.log("add candidate success!");
					}).catch(e=>{
					  console.log("Error: Failure during addIceCandidate()");
					});
				}
				break;
			}
          }
          return;
		}
		
		if(msg.connectType==Yang_ConnectType_Srs||msg.connectType==Yang_ConnectType_Zlm){
			if(msg.requestType==Yang_Request_SfuSuccess){
				yang_uid=msg.uid;
				if(msg.connectType==Yang_ConnectType_Srs)
					yang_startPlaySrs();
				else
					yang_startPlayZlm();
					
			}else if(msg.requestType==Yang_Request_SfuFail){
				
			}
		
		}
		
	}
	
	function yang_onMessageClose() {
		console.log("mqtt close");
	}
	
	function yang_mqtt_connect(hostname, port,  clientId){
		if(yang_connected) return;
		if(yang_client==null) yang_client = new Paho.Client(hostname, port, clientId);

		yang_client.onConnectionLost = yang_onConnectionLost;
		yang_client.onMessageArrived = yang_onMessageArrived;

		
		// connect the client
		yang_client.connect({
			timeout: 40, // 连接超时时间/秒
			keepAliveInterval: 1800, // 1800秒内没有活动自动断开连接 默认值为60秒 心跳
			cleanSession: true, // 如果为true(默认值)，则在成功连接时删除客户端和服务器持久化状态
			useSSL: yang_mqtt_isWss, // 如果存在且为true，则使用SSL Websocket连接
			invocationContext: {}, // 传递给onSuccess回调或onFailure回调
			userName: "",
			password: "",
			onSuccess: yang_onConnect, // 注册连接成功
			onFailure: function(err) { console.warn("Mqtt connection fail", err) }, // 连接失败事件
			reconnect: true// 设置如果连接丢失，客户端是否自动尝试重新连接到服务器 如果设置为false，当连接丢失时，客户端将不会尝试自动重新连接到服务器。  如果设置为true，当连接丢失时，客户端将尝试重新连接服务器。 在尝试重新连接之前，它最初将等待1秒，对于每一次失败的重新连接尝试，延迟将加倍，直到达到2分钟，此时延迟将保持在2分钟。  
			
			});
	}

	function yang_mqtt_disconnect(){
		if(yang_client!=null&&yang_client.isConnected()) 	{
			//if(connected) client.unsubscribe(clientTopic);
			//client.disconnect();
		}
	}	

	function yang_publish(msg){
		//console.log("send mqtt msg:"+msg);
		var message = new Paho.Message(msg);
		message.destinationName = yang_pubTopic;
		if(yang_client.isConnected()) {
			yang_client.send(message);
		}else{
			console.log("error:mqtt client is disconnetcted");
		}
	}

	
	function yang_start_interval(){
		if(!yang_isSetinterval){
			yang_isSetinterval=true;	
			yangInterval=setInterval(sendMqttPing,yangIntervalTime);
		}		
	}
