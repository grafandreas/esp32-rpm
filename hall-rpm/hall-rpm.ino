
#include "WiFi.h"
#include "AsyncUDP.h"
#include "credentials.h"
const char * ssid = SSID;
const char * password = PWD;

AsyncUDP udp;

#define HAL_PIN 2                             // Drehzahl-Eingang: Pin2 / Interrupt 0
#define BAUDRATE 115200
#define PERIOD_MS 200

volatile unsigned int counter = 0;

volatile unsigned int lastSend = 0;


void setup() {
  pinMode(HAL_PIN, INPUT);                    // Eingangspin auf Eingang stellen
  //digitalWrite(Eingang, HIGH);              // und Pullup-Widerstand einschalten
   
  Serial.begin(BAUDRATE);
  Serial.println("WiFi starting");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
        while(1) {
            delay(1000);
        }
    }

       if(udp.connect(IPAddress(192,168,0,180), 5678)) {
        Serial.println("UDP connected");
        udp.onPacket([](AsyncUDPPacket packet) {
         
            packet.printf("Got %u bytes of data", packet.length());
        });
        //Send unicast
        udp.print("Hello Server!");
    }
  // Interrupt 0 auf Routine readmicros setzen
  attachInterrupt(0, tick, FALLING);

  Serial.println("RPM active");
}


void loop() {

  unsigned int timeNow = millis();
  unsigned counter_c  = counter;

  udp.broadcastTo("Broadcast", 5678);
  unsigned int delay_s = timeNow-lastSend;
  lastSend = timeNow;
  delay(delay_s);
}


// Interrupt-Routine
// Diese Funktion wird immer dann aufgerufen, wenn ein Magnet vom Arduino erkannt wird
void tick() {
  Serial.println("Magnet erkannt");
  counter ++;
  
}
