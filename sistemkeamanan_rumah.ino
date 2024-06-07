#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define TRIG_PIN 5
#define ECHO_PIN 4
#define BUZZER_PIN 0

const char* ssid = "Azharianto";
const char* password = "112233445566789";
const char* mqtt_server = "broker.emqx.io";
const char* mqtt_topic = "topik/notifikasi";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Menghubungkan ke ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi terhubung");
  Serial.println("Alamat IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    if (client.connect("NodeMCUClient")) {
      Serial.println("terhubung");
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 5 detik");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  long duration, distance;
  // digitalWrite(BUZZER_PIN, HIGH);
  
  digitalWrite(TRIG_PIN, LOW);  
  delayMicroseconds(2); 
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10); 
  digitalWrite(TRIG_PIN, LOW);
  
  duration = pulseIn(ECHO_PIN, HIGH);
  
  distance = (duration / 2) / 29.1;
  
  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  if (distance > 10) {
    Serial.println("Mengirim notifikasi ke MQTT...");
    client.publish(mqtt_topic, "rumah terdeteksi tidak aman");
    
    // Membunyikan buzzer
    digitalWrite(BUZZER_PIN, LOW);
    delay(2000); // Buzzer berbunyi selama 1 detik
    digitalWrite(BUZZER_PIN, HIGH);
  }
  
  delay(1000);
}
