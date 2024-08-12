#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DFRobot_Aliyun.h"


#define WaterSW_PIN 2    //灌溉马达开关D9
#define HumUpdate_PIN 34  //土壤湿度采集A2
#define TempUpdate_PIN 17    //环境温度采集
#define SolarVol_PIN 39  //光伏板电压采集A1
#define HaveWater_PIN 25  //液位采集D2

/*配置WIFI名和密码*/
const char* WIFI_SSID = "xxx";
const char* WIFI_PASSWORD = "xxx";

/*配置设备证书信息*/
String ProductKey = "xxx";
String DeviceName = "xxx";
String ClientId = String(ProductKey) + "." + String(DeviceName);
String DeviceSecret = "xxx";

/*配置域名和端口号*/
String ALIYUN_SERVER = "xxx";
uint16_t PORT = 1883;

/*需要操作的产品标识符*/
String WaterSW_ID = "WaterSW";      //灌溉马达开关
String HumUpdate_ID = "HumUpdate";  //土壤湿度
String HumLevel_ID = "HumLevel";  //土壤湿度阈值
String UpdateTime_ID = "UpdateTime";  //数据上传频率
String WaterTime_ID = "WaterTime";  //灌溉时长
String WaterMode_ID = "WaterMode";  //灌溉模式
String HaveWater_ID = "HaveWater";  //是否有水
String SolarVol_ID = "SolarVol";  //光伏电压
String TempUpdate_ID = "TempUpdate";  //环境温度
String WiFi_RSSI_ID = "WiFi_RSSI";  //环境温度

int HumLevel_aliyun = 50;//默认湿度阈值
unsigned long UpdateTime_aliyun = 10;//数据上传间隔5秒
unsigned long WaterTime_aliyun = 5;//每次灌溉时间5秒
int WaterMode_aliyun = 1;//默认手动
int WaterSW_aliyun = 0;//默认关闭

/*需要上报和订阅的两个TOPIC*/
const char* subTopic = "xxx";  //云端下发，设备端订阅
const char* pubTopic = "xxx";   //设备端上报，云端订阅

DFRobot_Aliyun myAliyun;
WiFiClient espClient;
PubSubClient client(espClient);

void delayforHumUpdate(unsigned long seconds) {
  static unsigned long previousHumMillis = 0;
  unsigned long currentHumMillis = millis();
  unsigned long intervalMillis = seconds * 1000;
  if (currentHumMillis - previousHumMillis >= intervalMillis) {
    previousHumMillis = currentHumMillis;
    publishHumUpdate();
  }
}

unsigned long previousWaterMillis = 0;
bool isWatering = 0;

void delayforWaterTime(unsigned long seconds) {
  unsigned long currentWaterMillis = millis();
  unsigned long intervalMillis = seconds * 1000;

  if (isWatering && (currentWaterMillis - previousWaterMillis >= intervalMillis)) {
    digitalWrite(WaterSW_PIN, LOW);
    isWatering = 0;
    //Serial.println("Shutdown watering");
  }
}

void startWatering() {
  digitalWrite(WaterSW_PIN, HIGH);
  previousWaterMillis = millis();
  isWatering = 1;
  //Serial.println("Watering");
}

void connectWiFi() {
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

//订阅Topic callback
void callback(char* topic, byte* payload, unsigned int len) {
  Serial.print("Receive [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < len; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // 使用 ArduinoJson 6 版本的 JSON 解析方法
  DynamicJsonDocument doc(200);
  DeserializationError error = deserializeJson(doc, payload, len);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  if (doc.containsKey("params") && doc["params"].containsKey(WaterSW_ID)) {
    WaterSW_aliyun = doc["params"][WaterSW_ID];
    Serial.print("WaterSW_aliyun = ");
    Serial.println(WaterSW_aliyun);
    //将执行结果post给云端
    String tempMseg;
    serializeJson(doc, tempMseg);
    client.publish(pubTopic, tempMseg.c_str());
  } else if (doc.containsKey("params") && doc["params"].containsKey(HumLevel_ID)) {
    HumLevel_aliyun = doc["params"][HumLevel_ID];
    Serial.print("HumLevel_aliyun = ");
    Serial.println(HumLevel_aliyun);
    String tempMseg1;
    serializeJson(doc, tempMseg1);
    client.publish(pubTopic, tempMseg1.c_str());
  } else if (doc.containsKey("params") && doc["params"].containsKey(UpdateTime_ID)) {
    UpdateTime_aliyun = doc["params"][UpdateTime_ID];
    Serial.print("UpdateTime_aliyun = ");
    Serial.println(UpdateTime_aliyun);
    String tempMseg2;
    serializeJson(doc, tempMseg2);
    client.publish(pubTopic, tempMseg2.c_str());
  } else if (doc.containsKey("params") && doc["params"].containsKey(WaterTime_ID)) {
    WaterTime_aliyun = doc["params"][WaterTime_ID];
    Serial.print("WaterTime_aliyun = ");
    Serial.println(WaterTime_aliyun);
    String tempMseg3;
    serializeJson(doc, tempMseg3);
    client.publish(pubTopic, tempMseg3.c_str());
  } else if (doc.containsKey("params") && doc["params"].containsKey(WaterMode_ID)) {
    WaterMode_aliyun = doc["params"][WaterMode_ID];
    Serial.print("WaterMode_aliyun = ");
    Serial.println(WaterMode_aliyun);
    String tempMseg4;
    serializeJson(doc, tempMseg4);
    client.publish(pubTopic, tempMseg4.c_str());
  } else {
    Serial.println("Message does not contain expected topics.");
  }
}


OneWire DS18B20(TempUpdate_PIN);
DallasTemperature sensors(&DS18B20);

float getTemperature() {
  // 请求温度
  sensors.requestTemperatures();
  // 获取温度值
  float tempC = sensors.getTempCByIndex(0);
  // 如果读取失败，返回一个错误值
  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Error: Could not read temperature data");
    return NAN; // 返回 Not-A-Number 表示读取失败
  }
  return tempC;
}

//上报Topic
void publishHumUpdate() {
  int humAnalogValue = analogRead(HumUpdate_PIN);
  int humValue = map(humAnalogValue, 0, 4095, 0, 100);  // 12bitADC，量化为0-100

  int solarVolAnalogValue = analogRead(SolarVol_PIN);
  int solarVolValue = map(solarVolAnalogValue, 0, 4095, 0, 100);  // 12bitADC，量化为0-100

  int celsiusValue = getTemperature();

  int havewaterValue = digitalRead(HaveWater_PIN);

  int rssiValue = WiFi.RSSI();

  // 创建JSON文档
  StaticJsonDocument<200> doc;
  doc["id"] = 2;
  doc["params"][HumUpdate_ID] = humValue;
  doc["params"][TempUpdate_ID] = celsiusValue;
  doc["params"][SolarVol_ID] = solarVolValue;
  doc["params"][HaveWater_ID] = havewaterValue;
  doc["params"][WiFi_RSSI_ID] = rssiValue;
  doc["method"] = "thing.event.property.post";

  String jsonBuffer;
  serializeJson(doc, jsonBuffer);
  client.publish(pubTopic, jsonBuffer.c_str());
}



void ConnectAliyun() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    /*根据自动计算的用户名和密码连接到Alinyun的设备，不需要更改*/
    Serial.print("MQTT Server: ");
    Serial.println(myAliyun.mqtt_server);
    Serial.print("Client ID: ");
    Serial.println(myAliyun.client_id);
    Serial.print("Username: ");
    Serial.println(myAliyun.username);
    Serial.print("Password: ");
    Serial.println(myAliyun.password);

    if (client.connect(myAliyun.client_id, myAliyun.username, myAliyun.password)) {
      Serial.println("connected");
      client.subscribe(subTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(WaterSW_PIN, OUTPUT);

  /*连接WIFI*/
  connectWiFi();

  /*初始化Alinyun的配置，可自动计算用户名和密码*/
  myAliyun.init(ALIYUN_SERVER, ProductKey, ClientId, DeviceName, DeviceSecret);

  //client.connect(clientId.c_str(), username.c_str(), password.c_str());
  client.setServer(myAliyun.mqtt_server, PORT);

  /*设置回调函数，当收到订阅信息时会执行回调函数*/
  client.setCallback(callback);

  /*连接到Aliyun*/
  ConnectAliyun();

  /*开机默认关闭灌溉泵*/
  digitalWrite(WaterSW_PIN, LOW);

  /*初始化设备端数据到云端*/
  StaticJsonDocument<200> doc;
  doc["id"] = 1;
  doc["params"][WaterSW_ID] = WaterSW_aliyun;
  doc["params"][HumLevel_ID] = HumLevel_aliyun;
  doc["params"][UpdateTime_ID] = UpdateTime_aliyun;
  doc["params"][WaterTime_ID] = WaterTime_aliyun;
  doc["params"][WaterMode_ID] = WaterMode_aliyun;
  doc["params"][HumUpdate_ID] = 0;
  doc["params"][HaveWater_ID] = 0;
  doc["params"][SolarVol_ID] = 11;
  doc["params"][TempUpdate_ID] = 30;
  doc["params"][WiFi_RSSI_ID] = 0;
  doc["method"] = "thing.event.property.post";

  String jsonBuffer;
  serializeJson(doc, jsonBuffer);
  client.publish(pubTopic, jsonBuffer.c_str());

}

void loop() {
  if (!client.connected()) {
    digitalWrite(WaterSW_PIN, LOW);/*如果发生断网，关闭灌溉泵*/
    //Serial.println("Oops! Reconnecting, shutdown watering");
    ConnectAliyun();
  }
  client.loop();

  delayforHumUpdate(UpdateTime_aliyun);

  if (WaterMode_aliyun == 0) {  // 自动灌溉模式
    //Serial.println("Auto mode");
    if (analogRead(HumUpdate_PIN) < HumLevel_aliyun) {
      startWatering();  // 开始灌溉
      //Serial.println("Auto mode Watering");
    }
  } else if (WaterSW_aliyun == 1) {  // 手动灌溉模式
    //Serial.println("Manual mode");
    startWatering();
    //Serial.println("Manual mode watering");
  }

  delayforWaterTime(WaterTime_aliyun);

}