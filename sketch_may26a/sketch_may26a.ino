#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

String ssid = "";
String password = "";
String serverURL = "";
String storedData = ""; 
const int maxDataLength = 100; // Maximální délka přijatých dat

WiFiClient wifiClient;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);

  Serial.println("Ready to receive new WiFi credentials");

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
      if (line.startsWith("URL:")){
          serverURL = line.substring(4);
          serverURL.trim();
          Serial.println("serverURL is: " + serverURL);
          break;
      }

    
    }
  }

  // Uložení SSID do EEPROM
  for (int i = 0; i < ssid.length(); ++i) {
    EEPROM.write(i, ssid[i]);
  }
  EEPROM.write(ssid.length(), '\0'); // Ukončovací znak

  // Uložení hesla do EEPROM
  for (int i = 0; i < password.length(); ++i) {
    EEPROM.write(32 + i, password[i]);
  }
  EEPROM.write(32 + password.length(), '\0'); // Ukončovací znak

  EEPROM.commit();
  Serial.println("WiFi credentials saved to EEPROM");

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

  // Kontrola a aktualizace dat v paměti
  checkAndUpdateData();
}

void loop() {
  // Prázdná smyčka
}

void checkAndUpdateData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(wifiClient, serverURL);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);

      if (storedData != response) {
        // Uložit nová data do stavové proměnné a aktualizovat data v paměti
        storedData = response;
        updateEEPROMData(storedData);
        Serial.println("Data updated");
      } else {
        // Odeslat dotaz na server
        httpResponseCode = http.GET();
        if (httpResponseCode > 0) {
          String response = http.getString();
          Serial.println(httpResponseCode);
          Serial.println(response);

          // Odeslat odpověď zpět na micro:bit
          Serial.println("Response sent to micro:bit");
          Serial.println(response); // Odeslání odpovědi na sériovou linku zpět na micro:bit
        } else {
          Serial.print("Error on sending GET request: ");
          Serial.println(httpResponseCode);
        }
      }
    } else {
      Serial.print("Error on sending GET request: ");
      Serial.println(httpResponseCode);
    }
    http.end(); // Uzavření spojení
  } else {
    Serial.println("WiFi not connected");
  }
}

void updateEEPROMData(String newData) {
  // Odebere stará data
  for (int i = 0; i < maxDataLength; ++i) {
    EEPROM.write(64 + i, 0);
  }
  // Uloží nová data
  for (int i = 0; i < newData.length(); ++i) {
    EEPROM.write(64 + i, newData[i]);
  }
  EEPROM.commit();
}

String readEEPROMData() {
  String data = "";
  for (int i = 0; i < maxDataLength; ++i) {
    char c = EEPROM.read(64 + i);
    if (c == '\0') break;
    data += c;
  }
  return data;
}
