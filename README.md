# ESP8266-EvilTwin
- Custom Evil Twin captive portal (edited) attack with Fake Acess Point
- connect to : 172.0.0.1 (or it will be redirect you automatically)
## ! For Education Purposes ONLY !
- ! DO NOT ! USE THIS TOOL FOR ILLEGAL ACTIVITIES !
- ! USING THIS TOOL YOU ARE ACCEPTED ALL RISK THAT MAY BE CREATED AT YOUR OWN RISK OR DANGER ACTIVITIES !
# PIN CONNECTION
## Oled -> esp8266
- VCC -> 3.3V
- GND -> GND
- SCL -> D1
- SDA -> D2
## SD Card to esp32
- VCC -> 3.V
- GND -> GND
- MOSI -> D19
- MISO -> D23
- SCK -> D18
- CS -> D5
# INSTALLATION / GUIDE
## Using Linux
```
git clone https://github.com/Fattcat/ESP8266-ESP32-EvilTwin.git
```
- and you know what will be next when u have Linux ...
## Using Windows
- Just download this repo (Click on GREEN Button and then click on Download ZIP)
- One. Clone this GitHub Repository or download it.
- Two. Go to your Downloaded ZIP File and extract it to some file and save it.
- Three. No open ESP8266-EvilTwin.ino file.
- Four. Plug in your ESP8266 Board to your PC - <strong>Select Correct BOARD NAME and PORT</strong>
- Five. Upload code to your ESP8266.
- Six. Now search WiFi called "ZiFi" and connect with password "Eviltwin"
- Seven. Open your favorite WEB Browser and type "192.168.4.1" (it will connect you ro main page)
- Eight. There sellect Target WiFi and click on button EvilTwin (it will disconnect you from ZiFi because it will create new FAKE WIFI)
- Nine. add soon ...

# ! Need to know :D !
- REAL / ORIGINAL WiFi MUST BE Disturbed by Deauther or Jammer.
### YOU NEED TO HAVE INSTALLED ALL THIS LIBRARIES : ESP8266WiFi.h , DNSServer.h , ESP8266WebServer.h

- Tool can be uploaded to standard ESP8266 Board (NodeMCU ESP8266) or ESP8266 D1 Mini too.
- More will be added soon ...
