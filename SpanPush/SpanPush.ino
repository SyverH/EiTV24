#include <MKRNB.h>
#include "arduino_secrets.h"

#include <ArduinoJson.h>


#include <ArduinoLowPower.h>
#include <DHT.h>

//#define DEBUG

#define SPAN_APN "mda.lab5e"

const char PINNUMBER[] = SECRET_PINNUMBER;

const int sleepTime = 60 * 1000; // 10 seconds

/* Calibration-values for the soil sensor */
const int drySensorValue = 498;
const int wetSensorValue = 886;

/* The maximum size of the JSON-formatted package */
const int JsonSize = 128;


const int groundMoisturePin = A1;


#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

/* Constants for connection to SPAN */
unsigned int localPort = 4200;      // local port to listen for UDP packets
unsigned int spanPort = 1234;

IPAddress Lab5eSpanIP(172, 16, 15, 14);

// initialize the library instance

NBClient client;

GPRS gprs;

NB nbAccess;

// A UDP instance to let us send and receive packets over UDP

NBUDP Udp;

// Create a Static json document
StaticJsonDocument<JsonSize> doc;

void setup(){

    Serial.begin(9600);
    // Wait for Serial to be ready
    
    while(!Serial){
        ;
    }

    dht.begin();


    Serial.println("Connecting to SPAN...."); 
    boolean connected = false;

    while (!connected) {
    if ((nbAccess.begin(PINNUMBER, SPAN_APN) == NB_READY) &&
        (gprs.attachGPRS() == GPRS_READY)) {
        connected = true;
    } else {
    Serial.println("Not connected, retrying...");
    delay(1000);
    }
  }

    Udp.begin(localPort);
}

void loop(){

    int sensorValue = analogRead(groundMoisturePin);
    int moisturePercentage = map(sensorValue, drySensorValue, wetSensorValue, 100, 0);

    float airTemp = dht.readTemperature(false);
      float airHumidity = dht.readHumidity();

    // Adds the data source to the json document
    //doc["Source"] = "Arduino MKR 1500 NB";
    // Adds the groundMoisture value to the json document
    doc["Moisture"] = moisturePercentage;
    doc["Temperature"] = airTemp;
    doc["Humidity"] = airHumidity;

    /* Clear the messagePayload */
    char messagePayload[JsonSize] = "";

    /* Debugging mode  */
    #ifdef DEBUG

    Serial.println("Sending data to Lab5eSpanIP\n");
    
    serializeJson(doc, Serial);
    Serial.println("\n");

    serializeJson(doc, messagePayload) + "\n";

    Udp.beginPacket(Lab5eSpanIP, localPort);

    Udp.write(messagePayload, strlen(messagePayload));


    Udp.endPacket();

    delay(5000);

    #else
    /* If not debug mode */

    serializeJson(doc, messagePayload) + "\n";

    Udp.beginPacket(Lab5eSpanIP, localPort);

    Udp.write(messagePayload, strlen(messagePayload));

    Udp.endPacket();

    LowPower.sleep(sleepTime);
    #endif
}

void serializePacket(StaticJsonDocument<JsonSize> doc, char *messagePayload){
    serializeJson(doc, Serial);
    Serial.println("\n");
}