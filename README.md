# metaScreenshare
windows remote control
# config file

[video]  
videoEncHwType=0  
   
[rtc]  
iceTransportPolicy=0  
iceCandidateType=0  
iceServerPort=3478  //coturn 端口  
iceServerIP=192.168.0.104 //coturn IP  
iceUserName=metartc //turn 用户名  
icePassword=metartc //turn 密码  
   
[sys]  
enableLogFile=1  
logLevel=5  
   
[mqtt]  
enableMqttTls=0  
mqttPort=1883 //mqtt端口  
mqttServerIP=192.168.0.104 //mqtt IP地址  
maxReconnectTimes=1000  
reconnectIntervalTime=1000  
