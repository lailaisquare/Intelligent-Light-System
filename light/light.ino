#include <dummy.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define LED_ON  HIGH  
#define LED_OFF LOW
#define ONENET_FORMAT_TRUE "{\"id\":\"123\", \"params\": {\"button\":{\"value\":true}}}"  //button是根据自己的onenet平台的设备管理中的标识符
#define ONENET_FORMAT_FALSE "{\"id\":\"123\", \"params\": {\"button\":{\"value\":false}}}"//button是根据自己的onenet平台的设备管理中的标识符
#define ONENET_POST   "$sys/0kk25wrig9/test/thing/property/post"
#define ONENET_SUB    "$sys/0kk25wrig9/test/thing/property/set"
int flag=1,state;

//===========根据需要自己配置引脚=============//
int led_pin=4,light_control_pin=5,radiation_control_pin=12;
//===============需要自己配置================//
//配置需要连接的WiFi
const char* ssid = "money";                       //wifi名称
const char* password = "123123qwe";               //wifi密码

/*OneNet MQTT配置*/ 
const char* mqtt_server = "mqtts.heclouds.com";   // OneNet MQTT服务器地址
const int mqtt_port = 1883;                       // 非SSL端口

const char* client_id = "test";                   // 设备名称
const char* username = "0kk25wrig9";              // 产品ID
const char* mqtt_password = "version=2018-10-31&res=products%2F0kk25wrig9%2Fdevices%2Ftest&et=2052911776&method=md5&sign=iG1%2BIhj5kMtew0x9mMNCWw%3D%3D"; // 由token生成
//==========================================//
/*初始化对象*/
WiFiClient espClient;
PubSubClient client(espClient);

/*初始化wifi*/
void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("连接WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  delay(3000);

  if(WiFi.status() == WL_CONNECTED)
  {
     Serial.println("");
     Serial.println("WiFi已连接");
     Serial.println("IP地址: ");
     Serial.println(WiFi.localIP());
  }
}
/*连接onenet*/
void reconnect()
{
    Serial.print("尝试MQTT连接...");
    
    if (client.connect(client_id, username, mqtt_password)) {
      Serial.println("已连接到OneNet");
      client.subscribe(ONENET_SUB); 
    } 
    else {
      Serial.print("连接失败");
      Serial.print(client.state());
    }
}
/*发送状态给onenet*/
void mqttIntervalPost(int state)
{
    char jsonBuf[128];
    if(state)
    {//发送true打开
      sprintf(jsonBuf,ONENET_FORMAT_TRUE);
      Serial.println(jsonBuf);
      client.publish(ONENET_POST, jsonBuf);
    }
    else//发送false关闭
    { sprintf(jsonBuf,ONENET_FORMAT_FALSE);
      Serial.println(jsonBuf);
      client.publish(ONENET_POST, jsonBuf); 
    }
}
/*从app经onenet给指令*/
void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  // 解析JSON
  DynamicJsonDocument doc(256);
  deserializeJson(doc, payload, length);
  
  if(doc.containsKey("params")) {
    JsonObject params = doc["params"];
    //===============修改标识符==================//
    if(params.containsKey("button")) {
      int ledState = params["button"];
    //===============修改标识符==================//
      digitalWrite(led_pin, ledState ? LED_ON : LED_OFF);
      mqttIntervalPost(!ledState);
      if(!ledState)
        flag=1;
      else
        flag=0;
      Serial.println("LED set to: " + String(ledState));
    }
  }
}
/*程序初始化*/
void setup()
{
  // put your setup code here, to run once:
  //配置引脚模式
    pinMode(led_pin,OUTPUT);
    pinMode(light_control_pin,INPUT);
    pinMode(radiation_control_pin,INPUT);

    Serial.begin(115200);
    digitalWrite(led_pin,LED_OFF);
    digitalWrite(radiation_control_pin,LOW);
    mqttIntervalPost(0);

    setup_wifi();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
}
/*程序循环*/
void loop() 
{
  // put your main code here, to run repeatedly:
   static unsigned long lastMsgTime = 0;
   //每30s重连一次WiFi和onenet
   if (millis() - lastMsgTime > 30000)
  {
      lastMsgTime = millis();

    if(WiFi.status() != WL_CONNECTED)
      setup_wifi();
    if (!client.connected()) {
      reconnect();
    }
  }
  client.loop();
  //当状态改变时，发送当前灯的引脚状态
  static unsigned long laststate = 0;
  state=digitalRead(led_pin);
  if(laststate != state)
  {
    laststate=state;
    mqttIntervalPost(state);
    Serial.println("数据已发送:");
  }
    
  if(flag)
  {
    if(digitalRead(light_control_pin)==1 && digitalRead(radiation_control_pin)==1)
    {
      digitalWrite(led_pin,LED_ON);
    }
    else
    {
      digitalWrite(led_pin,LED_OFF);
    }
    delay(1000);
  }
}
