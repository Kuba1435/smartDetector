#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

String ssid = "";
String password = "";
String serverURL = "";
String storedData = "";
const int maxDataLength = 100; // Maximální délka přijatých dat
int lastDetectionState = LOW;  // Uchovávání posledního stavu detekce

WiFiClient wifiClient;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);

  pinMode(15, INPUT); // GPIO15 odpovídá D8 na ESP8266

  Serial.println("Ready to receive new WiFi credentials");

  // Načíst uložené WiFi údaje z EEPROM
  loadCredentials();

  // Čekání na přijetí přihlašovacích údajů
  while (true) {
    if (Serial.available()) {
      String line = Serial.readStringUntil('\n');
      if (line.startsWith("SSID:")) {
        ssid = line.substring(5);
        ssid.trim();
        Serial.println("Received SSID: " + ssid);
      }
      if (line.startsWith("PASS:")) {
        password = line.substring(5);
        password.trim();
        Serial.println("Received password: " + password);
      }
      if (line.startsWith("URL:")) {
        serverURL = line.substring(4);
        serverURL.trim();
        Serial.println("serverURL is: " + serverURL);
        break;
      }
    }
  }

  // Porovnat nové údaje s uloženými
  if (ssid != readStringFromEEPROM(0) || password != readStringFromEEPROM(50) || serverURL != readStringFromEEPROM(100)) {
    // Pokud se údaje liší, uložit nové údaje do EEPROM
    saveCredentials();
  }

  // Připojení k WiFi
  WiFi.begin(ssid.c_str(), password.c_str());

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Po připojení k WiFi zavolej funkci pro odeslání IP adresy
  notifyServerWithIP();
}

void loop() {
  int currentDetectionState = digitalRead(15); 

  if (currentDetectionState == HIGH && lastDetectionState == LOW) {
    notifyServerWithDetection();
    sendDeliveredMessage(); 
  }
  lastDetectionState = currentDetectionState;

  delay(100); 
}

void notifyServerWithIP() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = serverURL + "/api/device/online";
    String payload = "ip=" + WiFi.localIP().toString();
    http.begin(wifiClient, url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST request: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // Uzavření spojení
  } else {
    Serial.println("WiFi not connected");
  }
}

void notifyServerWithDetection() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = serverURL + "/api/device/detection";
    String payload = "{\"ip\":\"" + WiFi.localIP().toString() + "\"}";
    http.begin(wifiClient, url);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST request: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // Uzavření spojení
  } else {
    Serial.println("WiFi not connected");
  }
}

void sendDeliveredMessage() {
  Serial.println("delivered"); // Odeslání zprávy "delivered" na micro:bit přes sériový port
}

void saveCredentials() {
  writeStringToEEPROM(0, ssid);
  writeStringToEEPROM(50, password);
  writeStringToEEPROM(100, serverURL);
  EEPROM.commit();
  Serial.println("Credentials saved");
}

void loadCredentials() {
  ssid = readStringFromEEPROM(0);
  password = readStringFromEEPROM(50);
  serverURL = readStringFromEEPROM(100);
  Serial.println("Loaded credentials from EEPROM");
  Serial.println("SSID: " + ssid);
  Serial.println("Password: " + password);
  Serial.println("Server URL: " + serverURL);
}

void writeStringToEEPROM(int addr, const String &data) {
  int len = data.length();
  EEPROM.write(addr, len);
  for (int i = 0; i < len; i++) {
    EEPROM.write(addr + 1 + i, data[i]);
  }
}

String readStringFromEEPROM(int addr) {
  int len = EEPROM.read(addr);
  char data[len + 1];
  for (int i = 0; i < len; i++) {
    data[i] = EEPROM.read(addr + 1 + i);
  }
  data[len] = '\0';
  return String(data);
}
