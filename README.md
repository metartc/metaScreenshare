# metaScreenshare
windows remote control
# config file

[video]  
videoEncHwType=0  
   
[rtc]  
iceTransportPolicy=0  
iceCandidateType=0  
iceServerPort=3478  //coturn port  
iceServerIP=192.168.0.104 //coturn IP  
iceUserName=metartc //turn username  
icePassword=metartc //turn password  
   
[sys]  
enableLogFile=1  
logLevel=5  
   
[mqtt]  
enableMqttTls=0  
mqttPort=1883 //mqtt port  
mqttServerIP=192.168.0.104 //mqtt IP  
maxReconnectTimes=1000  
reconnectIntervalTime=1000  
