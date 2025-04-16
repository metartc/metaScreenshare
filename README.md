# metaScreenshare

windows remote control

# Signaling Server

mqtt:[mosquitto download]([Download | Eclipse Mosquitto](https://mosquitto.org/download/))

coturn:[download](https://github.com/coturn/coturn)

# docs

[开源远控软件metaScreenshare8编程指南-CSDN博客](https://blog.csdn.net/m0_56595685/article/details/147259162)

# config file

`desktop_config.ini   
[video]  
videoEncHwType=0  `

`[rtc]  
iceTransportPolicy=0  
iceCandidateType=0  
iceServerPort=3478  //coturn port  
iceServerIP=192.168.0.104 //coturn IP  
iceUserName=metartc //turn username  
icePassword=metartc //turn password  `

`[sys]  
enableLogFile=1  
logLevel=5  `

`[mqtt]  
enableMqttTls=0  
mqttPort=1883 //mqtt port  
mqttServerIP=192.168.0.104 //mqtt IP  
maxReconnectTimes=1000  
reconnectIntervalTime=1000  `

# html demo

screenshare8_html/index_mqtt.html

screenshare8_html/index_mqtt_full.html
