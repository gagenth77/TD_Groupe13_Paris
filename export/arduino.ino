#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <DHT.h>
#include <DHT_U.h>

const char* ssid     = "Invite-ESIEA";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "hQV86deaazEZQPu9a";     // The password of the Wi-Fi network
const char* mqttUser = "admin";
const char* mqttPassword = "admin";
const int DHTPin = 2;

IPAddress server(10, 8, 128, 148);  //MQTT server IP
WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPin, DHT22);

void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  
  delay(10);
  Serial.println('\n');
  
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());    

  Serial.println('\n');
  if (!MDNS.begin("ESP")) { 
    Serial.println("Error setting up mDNS");
    return;
  } else {
    Serial.println("mDNS setup finished");

    Serial.println("Sending mDNS Query");
    int n = MDNS.queryService("mqtt", "tcp");

    if (n == 0) {
      Serial.println("No service found");
      //return;
    } else {
      // at least one MQTT service is found
      // ip no and port of the first one is MDNS.IP(0) and MDNS.port(0)
      Serial.print("IP : ");
      Serial.println(MDNS.IP(0));
      Serial.print("PORT : ");
      Serial.println(MDNS.port(0));
    }
  }
  Serial.println('\n');

  Serial.println("Setting MQTT connection...");
  //client.setServer(MDNS.IP(0), MDNS.port(0));
  client.setServer("10.8.128.250", 1883);
  client.setCallback(callback);

  while (!client.connected()) {
      Serial.println("Connecting to MQTT...");
  
      if (client.connect("ESIEA/grp666", mqttUser, mqttPassword)) {
        Serial.println("connected");
      } else {
        Serial.print("failed with state ");
        Serial.println(client.state());
        delay(2000);
      }
  }
  
  Serial.println('\n');
  Serial.println("Finished configuration"); 
}

void loop() {
  delay(5000);

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from sensor");
  }
  
  String json;
  json += F("{\"temperature\": ");
  json += String(temperature, 2);
  json += F(", \"humidity\": ");
  json += String(humidity, 2);
  json += F("}");
  Serial.println("   -----   ");
  Serial.println(json);
  	
  client.publish("ESIEA/grp13", json.c_str());
}
