/*
 *
 * Reaktor23
 * 
 * project: powerCMDer
 * author: Bouni
 * 
 * a web enabled power control with integrated environment logging 
 * powerd by MQTT
 *
 */
 
//#######################################################################################

#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include "credentials.h"

// Pin definitions
#define ONE_WIRE_BUS        A5
#define SERVER_STATUS       3
#define SERVER_ON           6
#define SERVER_OFF          5
#define OUTLET_STATUS       7
#define OUTLET_ON           9
#define OUTLET_OFF          8
#define S0_PIN              2
#define S0_INTERRUPT        0
#define GPIO1               A0
#define GPIO2               A1
#define GPIO3               A2
#define GPIO4               A3
#define GPIO5               A4
#define GPIO6               A5  
#define GPIO7               1
#define GPIO8               0

// Temperature measurement setup
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Ethernet setup
byte mac[] = {0x90, 0xA2, 0xDA, 0x00, 0x65, 0x52};
EthernetClient ethClient;

// MQTT setup 
long mqttlastconnect = 0;
PubSubClient mqttClient(ethClient);

// temperature
long lasttemp = 0;

//#######################################################################################
 
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  if(strcmp(topic, "powerCMDer/server")==0) {
    if(payload[0] == '1'){
      server_on();
    } else if(payload[0] == '0') {
      server_off();  
    } else if(payload[0] == '?') {
      mqttClient.publish("powerCMDer/server/status", digitalRead(SERVER_STATUS));
    }
  } else if(strcmp(topic, "powerCMDer/outlets")==0) {
    if(payload[0] == '1'){
      outlets_on();
    } else if(payload[0] == '0') {
      outlets_off();  
    } else if(payload[0] == '?') {
      mqttClient.publish("powerCMDer/outlets/status", digitalRead(OUTLET_STATUS));
    }   
  }
}

void mqtt_connect() {
  if (!mqttClient.connected()) {
    long now = millis();
    if (now - mqttlastconnect > 5000) {
      mqttlastconnect = now;
      // Attempt to reconnect
      if (mqttClient.connected()) {
        mqttlastconnect = 0;
      } else {
        mqttClient.connect("powerCMDer", username, password);
        mqttClient.subscribe("powerCMDer/server");
        mqttClient.subscribe("powerCMDer/outlets");
      }
    } else {
      Serial.println("MQTT connection failed!");
      delay(10);  
    }
  } else {
    mqttClient.loop();
  }
}

void server_on() {
  digitalWrite(SERVER_OFF, LOW);  
  digitalWrite(SERVER_ON, HIGH);
  delay(500);
  digitalWrite(SERVER_ON, LOW);  
  mqttClient.publish("powerCMDer/server/status", "1");
}

void server_off() {
  digitalWrite(SERVER_ON, LOW); 
  digitalWrite(SERVER_OFF, HIGH);
  delay(500);
  digitalWrite(SERVER_OFF, LOW);  
  mqttClient.publish("powerCMDer/server/status", "0");
}

void outlets_on() {
  digitalWrite(OUTLET_OFF, LOW);  
  digitalWrite(OUTLET_ON, HIGH);
  delay(500);
  digitalWrite(OUTLET_ON, LOW);  
  mqttClient.publish("powerCMDer/outlets/status", "1");
}

void outlets_off() {
  digitalWrite(OUTLET_ON, LOW); 
  digitalWrite(OUTLET_OFF, HIGH);
  delay(500);
  digitalWrite(OUTLET_OFF, LOW);  
  mqttClient.publish("powerCMDer/outlets/status", "0");
}


// called by interrupt from S0 power meter
void S0_tick() {
  /* does not work because of hardware reasons at the moment*/
  /*
    if(millis() - time > 30) {
      Serial.println("tick");
      counter++;
      time = millis();
  }
  */
  Serial.println("tick");
  mqttClient.publish("powerCMDer/powermeter/tick", "1");
}

void send_temperature() {
  long now = millis();
  if (now - lasttemp > 60000) {
    lasttemp = now;
    sensors.requestTemperatures();
    float temperature = sensors.getTempCByIndex(0);
    char tbuffer[4];
    dtostrf(temperature,2,2,tbuffer);
    mqttClient.publish("powerCMDer/temperature", tbuffer); 
  }
}

void setup() {

  // Set pin for OneWire bus  
  pinMode(ONE_WIRE_BUS, INPUT_PULLUP);

  // set pins for server circuit
  pinMode(SERVER_STATUS, INPUT_PULLUP);
  pinMode(SERVER_ON, OUTPUT);
  digitalWrite(SERVER_ON, LOW);
  pinMode(SERVER_OFF, OUTPUT);
  digitalWrite(SERVER_OFF, LOW);

  // set pins for outlets circuits
  pinMode(OUTLET_STATUS, INPUT_PULLUP);
  pinMode(OUTLET_ON, OUTPUT);
  digitalWrite(OUTLET_ON, LOW);
  pinMode(OUTLET_OFF, OUTPUT);
  digitalWrite(OUTLET_OFF, LOW);

  // set pin for S0 power meter
  pinMode(S0_PIN, INPUT_PULLUP);
  attachInterrupt(S0_INTERRUPT, S0_tick, CHANGE);
  
  // set serial for debugging
  Serial.begin(57600);
  Serial.println("initializing powerCMDr");  

  // init ethernet 
  Serial.println("Trying to get a DHCP address, this can take up to 60 Seconds");
  while(Ethernet.begin(mac) != 1) {
    Serial.println("Failed to get DHCP address, retry until successful!");
  }
  Serial.print("Ethernet up, IP address: ");
  Serial.println(Ethernet.localIP());
  Serial.println("Give eth 1500ms to get ready");
  delay(1500);
  
  // init OneWire  
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices on the 1-wire bus.");

  // init MQTT
  mqttClient.setServer(mqttserver, mqttport);
  mqttClient.setCallback(mqtt_callback);   
}
 
void loop() {

  mqtt_connect();
  send_temperature();

}

