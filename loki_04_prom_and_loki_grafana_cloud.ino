
#include "config.h"
#include "certificates.h"
#include <PromLokiTransport.h>
#include <GrafanaLoki.h>
#include <PrometheusArduino.h>
#include "DHTesp.h"

#include "ESP32Servo.h"

#include <Wire.h>

// Create a client object for sending our data.
PromLokiTransport transport;
LokiClient lokiClient(transport);
PromClient promClient(transport);

// Create Loki Streams
LokiStreams streams(1);
LokiStream logger(3, 100, "{job=\"esp32\",type=\"log\"}");


// Create Prometheus Series
WriteRequest req(10,2048);

// Sensors
DHTesp dht;

int loopCounter = 0;
unsigned long previousPromTime = 0; // variable to store the previous time
unsigned long previousSensorTime = 0; // variable to store the previous time
const unsigned long interval = 5000; // interval time in milliseconds

Servo myservo;


struct sSensor
{
  uint16_t pin;
  String name;
  TimeSeries* tsobj;
  String type;
  float value;
};

sSensor sensors[] = {
  {4,"temperature",nullptr,"dht.temperature",0},
  {4,"humidity", nullptr, "dht.humidity",0},
  {36,"ambiance_light",nullptr,"analog",0},
  {39,"soil_moisture",nullptr,"analog",0},
  {10,"uptime_milliseconds_total",nullptr,"system.millis",0},
  {10,"heap_free_bytes",nullptr,"system.memfree",0}
};

struct sRule
{
  String name;
  String sensorName;
  float treshold;  
  bool triggered;
  String actuator;
  int seconds;
  unsigned long stopActuator;
};

sRule rules[] = {
  {"Humidity > 80","humidity",80, false,"pump",2,0},
  {"Temperature > 19","temperature",19, false,"",0,0},
};

void setup() {
    Serial.begin(115200);
    //Serial.begin(9600);
    
    // Wait 5s for serial connection or continue without it
    // some boards like the esp32 will run whether or not the 
    // serial port is connected, others like the MKR boards will wait
    // for ever if you don't break the loop.
    uint8_t serialTimeout;
    while (!Serial && serialTimeout < 50) {
        delay(100);
        serialTimeout++;
    }

    Serial.println("Running Setup");
    Serial.print("Free Mem Before Setup: ");
    Serial.println(freeMemory());

    initSensors();
    configureProm();
    
    // Setup Sensors

   

    Serial.print("Free Mem After Setup: ");
    Serial.println(freeMemory());

    // Setup pump ouput

    pinMode(19, OUTPUT); // pin1
    pinMode(18, OUTPUT); // pin2

    digitalWrite(18, LOW);
    digitalWrite(19,LOW);


   
}

void loop() {
  unsigned long currentTime = millis(); // get the current time
  if (currentTime - previousPromTime >= interval) { // check if the interval has elapsed
    previousPromTime = currentTime; // save the current time as the previous time
    postProm();
  }
  if (currentTime - previousSensorTime >= 1000) {
    previousSensorTime = currentTime;
    fillSensorData();
    parseRules();
  }

  checkActuatorsStop();
}
