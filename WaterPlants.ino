#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DFRobot_Aliyun.h"


#define MotorSW_PIN 13       //灌溉马达开关
#define WaterSW0_PIN 26      //电磁阀开关0
#define WaterSW1_PIN 27      //电磁阀开关1
#define HumUpdate0_PIN 34   //土壤湿度0采集
#define HumUpdate1_PIN 35   //土壤湿度1采集
#define TempUpdate_PIN 16   //环境温度采集
#define SolarVol_PIN 39     //电池电压采集
#define HaveWater_PIN 25    //灌溉桶液位采集
#define NoWaterLED_PIN 5    //缺水指示灯
#define NoWiFiLED_PIN 0    //断网指示灯

/*配置WIFI名和密码*/
const char* WIFI_SSID = "xxx";
const char* WIFI_PASSWORD = "xxx";

/*配置设备证书信息*/
String ProductKey = "xxx";
String DeviceName = "xxx";
String ClientId = String(ProductKey) + "." + String(DeviceName);
String DeviceSecret = "xxx";

/*配置域名和端口号*/
String ALIYUN_SERVER = "xxx.mqtt.iothub.aliyuncs.com";
uint16_t PORT = 1883;

/*云端下发控制*/
String MotorSW_ID = "MotorSW";        //灌溉马达开关
String WaterMode_ID = "WaterMode";    //灌溉模式
String WaterSW0_ID = "WaterSW0";      //电磁阀开关0
String WaterSW1_ID = "WaterSW1";      //电磁阀开关1
String HumLevel0_ID = "HumLevel0";    //湿度阈值0
String HumLevel1_ID = "HumLevel1";    //湿度阈值1
String WaterTime0_ID = "WaterTime0";  //灌溉时长0
String WaterTime1_ID = "WaterTime1";  //灌溉时长1
String UpdateTime_ID = "UpdateTime";  //数据上传间隔

/*定时上报值*/
String HumUpdate0_ID = "HumUpdate0";  //土壤湿度0
String HumUpdate1_ID = "HumUpdate1";  //土壤湿度1
String HaveWater_ID = "HaveWater";    //是否有水
String SolarVol_ID = "SolarVol";      //电池电压
String TempUpdate_ID = "TempUpdate";  //环境温度
String WiFi_RSSI_ID = "WiFi_RSSI";    //WiFi信号

/*端侧上电启动默认值，同时也上传云端*/
int MotorSW_aliyun = 0;               //默认关闭灌溉马达，0-关闭，1-开启
int WaterMode_aliyun = 1;             //默认手动灌溉模式，0-自动，1-手动
int WaterSW0_aliyun = 0;              //默认关闭电磁阀0，0-关闭，1-开启
int WaterSW1_aliyun = 0;              //默认关闭电磁阀1，0-关闭，1-开启
int HumLevel0_aliyun = 10;            //默认湿度阈值0，范围0~100，步长1
int HumLevel1_aliyun = 10;            //默认湿度阈值1，范围0~100，步长1
int WaterTime0_aliyun = 15;            //默认灌溉时长0，范围1~60s，步长1
int WaterTime1_aliyun = 10;            //默认灌溉时长1，范围1~60s，步长1
int UpdateTime_aliyun = 10;           //默认数据上传间隔，范围1~600s，步长1
float HumUpdate0_aliyun = 10.0;       //默认土壤湿度0，范围0~100，步长0.1
float HumUpdate1_aliyun = 10.0;       //默认土壤湿度1，范围0~100，步长0.1
int HaveWater_aliyun = 1;             //默认是否有水，0-无水，1-有水
float SolarVol_aliyun = 12.0;         //默认电池电压，范围10~15v，步长0.1v
float TempUpdate_aliyun = 25.0;       //默认环境温度，范围-30~60℃，步长0.1
int WiFi_RSSI_aliyun = -50;           //默认WiFi信号，范围-70~0，步长1


/*需要上报和订阅的两个TOPIC*/
const char* subTopic = "xxx/thing/service/property/set";    //云端下发，设备端订阅
const char* pubTopic = "xxx/thing/event/property/post";   //设备端上报，云端订阅

DFRobot_Aliyun myAliyun;
WiFiClient espClient;
PubSubClient client(espClient);

/*订阅Topic callback*/
void callback(char* topic, byte* payload, unsigned int len) {
  Serial.print("Receive [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < len; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  /*使用 ArduinoJson 6 版本的 JSON 解析方法*/
  DynamicJsonDocument doc(400); //因为不确定云端下发多少数据，所以采用DynamicJson
  DeserializationError error = deserializeJson(doc, payload, len);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  if (doc.containsKey("params") && doc["params"].containsKey(MotorSW_ID)) {
    MotorSW_aliyun = doc["params"][MotorSW_ID];
    Serial.print("MotorSW_aliyun = ");
    Serial.println(MotorSW_aliyun);
    /*将执行结果post给云端，以确保云端下发的数据确实在设备端同步更新成功了*/
    String tempMseg0;
    serializeJson(doc, tempMseg0);
    client.publish(pubTopic, tempMseg0.c_str());
  } else if (doc.containsKey("params") && doc["params"].containsKey(WaterSW0_ID)) {
    WaterSW0_aliyun = doc["params"][WaterSW0_ID];
    Serial.print("WaterSW0_aliyun = ");
    Serial.println(WaterSW0_aliyun);
    String tempMseg1;
    serializeJson(doc, tempMseg1);
    client.publish(pubTopic, tempMseg1.c_str());
  } else if (doc.containsKey("params") && doc["params"].containsKey(WaterSW1_ID)) {
    WaterSW1_aliyun = doc["params"][WaterSW1_ID];
    Serial.print("WaterSW1_aliyun = ");
    Serial.println(WaterSW1_aliyun);
    String tempMseg2;
    serializeJson(doc, tempMseg2);
    client.publish(pubTopic, tempMseg2.c_str());
  } else if (doc.containsKey("params") && doc["params"].containsKey(HumLevel0_ID)) {
    HumLevel0_aliyun = doc["params"][HumLevel0_ID];
    Serial.print("HumLevel0_aliyun = ");
    Serial.println(HumLevel0_aliyun);
    String tempMseg3;
    serializeJson(doc, tempMseg3);
    client.publish(pubTopic, tempMseg3.c_str());
  } else if (doc.containsKey("params") && doc["params"].containsKey(HumLevel1_ID)) {
    HumLevel1_aliyun = doc["params"][HumLevel1_ID];
    Serial.print("HumLevel1_aliyun = ");
    Serial.println(HumLevel1_aliyun);
    String tempMseg4;
    serializeJson(doc, tempMseg4);
    client.publish(pubTopic, tempMseg4.c_str());
  } else if (doc.containsKey("params") && doc["params"].containsKey(UpdateTime_ID)) {
    UpdateTime_aliyun = doc["params"][UpdateTime_ID];
    Serial.print("UpdateTime_aliyun = ");
    Serial.println(UpdateTime_aliyun);
    String tempMseg5;
    serializeJson(doc, tempMseg5);
    client.publish(pubTopic, tempMseg5.c_str());
  } else if (doc.containsKey("params") && doc["params"].containsKey(WaterTime0_ID)) {
    WaterTime0_aliyun = doc["params"][WaterTime0_ID];
    Serial.print("WaterTime0_aliyun = ");
    Serial.println(WaterTime0_aliyun);
    String tempMseg6;
    serializeJson(doc, tempMseg6);
    client.publish(pubTopic, tempMseg6.c_str());
  } else if (doc.containsKey("params") && doc["params"].containsKey(WaterTime1_ID)) {
    WaterTime1_aliyun = doc["params"][WaterTime1_ID];
    Serial.print("WaterTime1_aliyun = ");
    Serial.println(WaterTime1_aliyun);
    String tempMseg7;
    serializeJson(doc, tempMseg7);
    client.publish(pubTopic, tempMseg7.c_str());
  } else if (doc.containsKey("params") && doc["params"].containsKey(WaterMode_ID)) {
    WaterMode_aliyun = doc["params"][WaterMode_ID];
    Serial.print("WaterMode_aliyun = ");
    Serial.println(WaterMode_aliyun);
    String tempMseg8;
    serializeJson(doc, tempMseg8);
    client.publish(pubTopic, tempMseg8.c_str());
  } else {
    Serial.println("Message does not contain expected topics.");
  }
}

unsigned long previousWaterMillis0 = 0;
unsigned long previousWaterMillis1 = 0;
bool isWatering0 = 0;
bool isWatering1 = 0;

/*非阻塞定时器0，用于第0路灌溉计时*/
void delayforWaterTime0(unsigned long seconds) {
  unsigned long currentWaterMillis = millis();
  unsigned long intervalMillis = seconds * 1000;
  if (isWatering0 && (currentWaterMillis - previousWaterMillis0 >= intervalMillis)) {
    digitalWrite(MotorSW_PIN, LOW);
    digitalWrite(WaterSW0_PIN, LOW);
    isWatering0 = 0;
    Serial.println("Shutdown watering0");
  }
}

/*非阻塞定时器1，用于第1路灌溉计时*/
void delayforWaterTime1(unsigned long seconds) {
  unsigned long currentWaterMillis = millis();
  unsigned long intervalMillis = seconds * 1000;

  if (isWatering1 && (currentWaterMillis - previousWaterMillis1 >= intervalMillis)) {
    digitalWrite(MotorSW_PIN, LOW);
    digitalWrite(WaterSW1_PIN, LOW);
    isWatering1 = 0;
    Serial.println("Shutdown watering1");
  }
}

void startWatering0() {
  digitalWrite(MotorSW_PIN, HIGH);
  digitalWrite(WaterSW0_PIN, HIGH);
  previousWaterMillis0 = millis();
  isWatering0 = 1;
  Serial.println("startWatering0");
}

void startWatering1() {
  digitalWrite(MotorSW_PIN, HIGH);
  digitalWrite(WaterSW1_PIN, HIGH);
  previousWaterMillis1 = millis();
  isWatering1 = 1;
  Serial.println("startWatering1");
}

void connectWiFi() {
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  unsigned long wifiAttemptTime = millis(); 
  const unsigned long wifiTimeout = 60000; // 设定超时时间为1分钟（60,000毫秒）

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(NoWiFiLED_PIN, HIGH);  // 开启断网报警灯
    if (millis() - wifiAttemptTime >= wifiTimeout) {
      Serial.println();
      Serial.println("WiFi Connection Timeout. Restarting...");
      ESP.restart(); // 超时重启ESP32
    }
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(NoWiFiLED_PIN, LOW);  // 关闭断网报警灯
}


OneWire DS18B20(TempUpdate_PIN);
DallasTemperature sensors(&DS18B20);//全局调用

float getTemperature() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Error: Could not read temperature data");
    return NAN; // 返回 Not-A-Number 表示读取失败
  }
  return tempC;
}

const float referenceVoltage = 3.3; // ADC参考电压是3.3V
const float humidityVoltageScale = 1.47; // 上拉4.7K，下拉10K，适配ADC最高4.8的输入电压
float getHumidity(int sensorPin) {
    int analogValue = analogRead(sensorPin);//12bit ADC返回值范围0~4095
    float voltage = (analogValue * (referenceVoltage / 4095.0)) * humidityVoltageScale; // 将ADC值转换为原始电压

    float humidity = 0.0;

    if (voltage >= 4.8) {
        humidity = 0.0;
    } else if (voltage >= 3.8) {
        humidity = 0.0 + (20.0 - 0.0) * (4.8 - voltage) / (4.8 - 3.8);
    } else if (voltage >= 2.7) {
        humidity = 20.0 + (40.0 - 20.0) * (3.8 - voltage) / (3.8 - 2.7);
    } else if (voltage >= 1.8) {
        humidity = 40.0 + (60.0 - 40.0) * (2.7 - voltage) / (2.7 - 1.8);
    } else if (voltage >= 1.2) {
        humidity = 60.0 + (80.0 - 60.0) * (1.8 - voltage) / (1.8 - 1.2);
    } else if (voltage >= 0.8) {
        humidity = 80.0 + (100.0 - 80.0) * (1.2 - voltage) / (1.2 - 0.8);
    } else {
        humidity = 100.0;
    }

    return humidity;
}

//const float referenceVoltage = 3.3; // ADC参考电压是3.3V
const float solarVoltageScale = 4.0;     // 上拉30K，下拉10K，适配ADC最高10.8~13.6v的输入电压

float getsolarVol(int sensorPin) {
    int analogValue = analogRead(sensorPin);  // 读取ADC值
    float voltage = analogValue * (referenceVoltage / 4095.0); // 将ADC值转换为分压后的电压
    float actualVoltage = voltage * solarVoltageScale; // 将分压后的电压转换为实际电压
    return actualVoltage;
}

/*非阻塞定时器，用于数据定时上传计时*/
void delayforUpdate(unsigned long seconds) {
  static unsigned long previousHumMillis = 0;
  unsigned long currentHumMillis = millis();
  unsigned long intervalMillis = seconds * 1000;
  if (currentHumMillis - previousHumMillis >= intervalMillis) {
    previousHumMillis = currentHumMillis;
    publishUpdate();
  }
}

/*定时上传的数据*/
void publishUpdate() {
  float humValue0 = getHumidity(HumUpdate0_PIN); 
  float humValue1 = getHumidity(HumUpdate1_PIN);
  float solarVolValue = getsolarVol(SolarVol_PIN);
  int havewaterValue = digitalRead(HaveWater_PIN);
  float celsiusValue = getTemperature();
  int rssiValue = WiFi.RSSI();

  // 创建JSON文档，因为知道上传多少数据，所以采用StaticJson定义
  StaticJsonDocument<200> doc;
  doc["id"] = 2;
  doc["params"][HumUpdate0_ID] = humValue0;
  doc["params"][HumUpdate1_ID] = humValue1;
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
  unsigned long aliyunAttemptTime = millis(); 
  const unsigned long aliyunTimeout = 60000; // 设定超时时间为1分钟（60,000毫秒）
  while (!client.connected()) {
    digitalWrite(NoWiFiLED_PIN, HIGH);  // 开启断网报警灯
    if (millis() - aliyunAttemptTime >= aliyunTimeout) {
      Serial.println();
      Serial.println("Aliyun Connection Timeout. Restarting...");
      ESP.restart(); // 超时重启ESP32
    }

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
      digitalWrite(NoWiFiLED_PIN, LOW);  // 关闭断网报警灯
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
  /* digitalWrite和digitalRead调用之前需要设置pinMode，而analogRead不需要*/
  pinMode(MotorSW_PIN, OUTPUT);
  pinMode(WaterSW0_PIN, OUTPUT);
  pinMode(WaterSW1_PIN, OUTPUT);
  pinMode(NoWaterLED_PIN, OUTPUT);
  pinMode(NoWiFiLED_PIN, OUTPUT);
  // pinMode(TempUpdate_PIN, INPUT_PULLUP);电路板上增加了上拉
  pinMode(HaveWater_PIN, INPUT);
  
  /*开机默认*/
  digitalWrite(MotorSW_PIN, LOW);
  // digitalWrite(WaterSW0_PIN, LOW);//灌溉电源已经关闭，不需要再关闭电磁阀
  // digitalWrite(WaterSW1_PIN, LOW);
  digitalWrite(NoWaterLED_PIN, LOW);
  digitalWrite(NoWiFiLED_PIN, LOW);

  /*连接WIFI*/
  connectWiFi();

  /*初始化Alinyun的配置，可自动计算用户名和密码*/
  myAliyun.init(ALIYUN_SERVER, ProductKey, ClientId, DeviceName, DeviceSecret);

  client.setServer(myAliyun.mqtt_server, PORT);

  /*设置回调函数，当收到订阅信息时会执行回调函数*/
  client.setCallback(callback);

  /*连接到Aliyun*/
  ConnectAliyun();

  /*初始化设备端数据到云端，因为知道上传多少数据，所以采用StaticJson定义*/
  StaticJsonDocument<300> doc;
  doc["id"] = 1;
  doc["params"][MotorSW_ID] = MotorSW_aliyun;
  doc["params"][WaterSW0_ID] = WaterSW0_aliyun;
  doc["params"][WaterSW1_ID] = WaterSW1_aliyun;
  doc["params"][HumLevel0_ID] = HumLevel0_aliyun;
  doc["params"][HumLevel1_ID] = HumLevel1_aliyun;
  doc["params"][WaterTime0_ID] = WaterTime0_aliyun;
  doc["params"][WaterTime1_ID] = WaterTime1_aliyun;
  doc["params"][UpdateTime_ID] = UpdateTime_aliyun;
  doc["params"][WaterMode_ID] = WaterMode_aliyun;
  doc["params"][HumUpdate0_ID] = HumUpdate0_aliyun;
  doc["params"][HumUpdate1_ID] = HumUpdate1_aliyun;
  doc["params"][HaveWater_ID] = HaveWater_aliyun;
  doc["params"][SolarVol_ID] = SolarVol_aliyun;
  doc["params"][TempUpdate_ID] = TempUpdate_aliyun;
  doc["params"][WiFi_RSSI_ID] = WiFi_RSSI_aliyun;
  doc["method"] = "thing.event.property.post";

  String jsonBuffer;
  serializeJson(doc, jsonBuffer);
  client.publish(pubTopic, jsonBuffer.c_str());

}

void loop() {
  if (!client.connected()) {
    ConnectAliyun();
  }
  client.loop();

  // 定时上传数据
  delayforUpdate(UpdateTime_aliyun);

  // 检查并更新灌溉时间，确保灌溉按计划结束
  delayforWaterTime0(WaterTime0_aliyun);
  delayforWaterTime1(WaterTime1_aliyun);

  if (!digitalRead(HaveWater_PIN)) {
    digitalWrite(NoWaterLED_PIN, HIGH); //开启缺水报警灯
    digitalWrite(MotorSW_PIN, LOW);     //关闭灌溉泵
    Serial.println("Oops! No water at all");
  } else {
    digitalWrite(NoWaterLED_PIN, LOW); //关闭缺水报警灯
    if (WaterMode_aliyun == 0) {  // 自动灌溉模式
      Serial.println("Auto mode");
      /*实时湿度值大于阈值，说明土壤缺水*/
      if (getHumidity(HumUpdate0_PIN) < HumLevel0_aliyun) {
        startWatering0();  // 开始灌溉0
        Serial.println("Auto mode Watering0");
      } 
      if (getHumidity(HumUpdate1_PIN) < HumLevel1_aliyun) {
        startWatering1();  // 开始灌溉1
        Serial.println("Auto mode Watering1");
      }
    } else if (MotorSW_aliyun == 1) {  // 手动灌溉模式
      Serial.println("Manual mode");
      if (WaterSW0_aliyun == 1) {
        startWatering0();
        Serial.println("Manual mode watering0");
      } 
      if (WaterSW1_aliyun == 1) {
        startWatering1();
        Serial.println("Manual mode watering1");
      }
    }
  }   
}
