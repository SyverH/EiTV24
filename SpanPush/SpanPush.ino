#include <MKRNB.h>
#include "arduino_secrets.h"

#include <ArduinoJson.h>

const int drySensorValue = 498;
const int wetSensorValue = 886;

const int JsonSize = 128;

const int groundMoisturePin = A1;

const char PINNUMBER[] = SECRET_PINNUMBER;

unsigned int localPort = 1234;      // local port to listen for UDP packets

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

    Serial.println("Starting Arduino GPRS NTP client.");

    boolean Connected = false;

    while(!Connected){
        if((nbAccess.begin(PINNUMBER) == NB_READY) &&
            (gprs.attachGPRS() == GPRS_READY)){
                Connected = true;
            }
        else{
            Serial.println("NB not connected");
            delay(1000);
        }
    }

    Serial.println("\nStarting connection to server...");

    Udp.begin(localPort);
}

void loop(){


    delay(5000);
    Serial.println("Sending data to Lab5eSpanIP ");

    /*
    This code should fill a json document with the groundMoisture value and send it to the Lab5eSpanIP
    */

    int sensorValue = readAnalogSensor(groundMoisturePin);
    int moisturePercentage = map(sensorValue, drySensorValue, wetSensorValue, 100, 0);

    doc["Source"] = "Arduino MKR 1500 NB";
    // Adds the groundMoisture value to the json document
    doc["groundMoisture"] = moisturePercentage;

    char messagePayload[JsonSize] = "";

    serializeJson(doc, Serial);
    Serial.println("\n");

    /*
    serializeJson(doc, messagePayload) + "\n";

    Udp.beginPacket(Lab5eSpanIP, localPort);

    Udp.write(messagePayload);

    Udp.endPacket();
    */
}

int readAnalogSensor(int Pin){
    int sensorValue = analogRead(Pin);
    return sensorValue;
}