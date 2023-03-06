/*
 const char* ssid = "Linksys03578";
 const char* password = "phh6hhzi04"; 
 * 
  *  Json parametric GET REST response with ArduinoJSON library
  *  by Mischianti Renzo <https://www.mischianti.org>
 *
 *  https://www.mischianti.org/
 *
 *
 *  https://www.mischianti.org/2020/07/15/how-to-create-a-rest-server-on-esp8266-or-esp32-cors-request-option-and-get-part-4/
 */


/*
*   D5 - RX from VINDRIKTNING  
*   D6 - DS1820
*   D2 - SDA
*   D1 - SLC
*/
 
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

#include <SimplePgSQL.h>
#include <ArduinoOTA.h>

#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;





#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



#include <SPI.h>
#include <Wire.h>

#include <OneWire.h>


#include <SerialCom.h>
#include <Types.h>

particleSensorState_t state;              

const char* ssid = "Linksys03578";
//const char* ssid = "cam_net_rear";
const char* password = "phh6hhzi04"; 
char identifier[24];

 
IPAddress PGIP(192,168,1,100);        // your PostgreSQL server IP
const char user[] = "postgres";       // your database user
const char db_password[] = "free.dom55";   // your database password
const char dbname[] = "timeseries";         // your database name



WiFiClientSecure secureclient;
WiFiClient client;
HTTPClient httpsClient;

char buffer[1024];

PGconnection conn(&client, 0, 1024, buffer);
int pg_status = 0;
float temperature;
float pressure;

ADC_MODE(ADC_VCC);
int vccvalue = 0;  // value read from VCC
const int voffset = 342;   //342 mV ADC offset

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//Week Days
String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//Month names
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};


// Variable to save current epoch time
unsigned long epochTime; 
unsigned long startTime;

 
ESP8266WebServer server(80);

// GPIO where the DS18B20 is connected to
const int oneWireBus = 12;    //4? 

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);



const char *URL = "https://hooks.slack.com/services/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";
const char *FINGERPRINT = "82AEFD933630DA030A2F6353DE2EB0438BF441F6";


const char* test_root_ca= \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n" \
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n" \
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n" \
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n" \
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n" \
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n" \
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n" \
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n" \
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n" \
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n" \
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n" \
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n" \
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n" \
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n" \
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n" \
"-----END CERTIFICATE-----\n";



const long timerInterval = 60000;
const long blinkInterval  = 1000;
unsigned long previousBlinkTimer = 0;
unsigned long uptime=0;


int ledState = LOW; 
int displayIDX;

unsigned long previousTimer = 0;
unsigned long sensorTimer = 0;

 
void setCrossOrigin(){
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    server.sendHeader(F("Access-Control-Max-Age"), F("600"));
    server.sendHeader(F("Access-Control-Allow-Methods"), F("PUT,POST,GET,OPTIONS"));
    server.sendHeader(F("Access-Control-Allow-Headers"), F("*"));
};

unsigned long getTime() {
  // Function that gets current epoch time
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}

String getUptime() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;
  int hours = (elapsedTime / 1000) / 3600;
  int minutes = (elapsedTime / 1000) / 60 % 60;
  int seconds = (elapsedTime / 1000) % 60;

  String uptime = String(hours);
  if (hours < 10) {
    uptime = "0" + uptime;
  }
  uptime += ":";

  String minutesString = String(minutes);
  if (minutes < 10) {
    minutesString = "0" + minutesString;
  }
  uptime += minutesString;
  uptime += ":";

  String secondsString = String(seconds);
  if (seconds < 10) {
    secondsString = "0" + secondsString;
  }
  uptime += secondsString;

  return uptime;
}
void dbwait()
{
    int tries = 242;
    int rc;
    String msg;
    while(1==1) {
        rc=conn.getData();
        
        if (rc & PG_RSTAT_READY) {
            pg_status = 2;
            Serial.println("db Ready");
            break;
        }

        
        if (rc < 0) {
            Serial.println("db error");
            msg = conn.getMessage();
              if (msg) 
                Serial.println(msg);
            break;
          
        }
        if (!rc) {
          Serial.print("*");
          tries--;
          if(tries<0){
            Serial.println("Retries exceeded");
              msg = conn.getMessage();
              if (msg) 
                Serial.println(msg);

            break;
          }
          
        }

      }
} 



void connect2db()
{
  int rc = 0;

   Serial.println("Connecting to db");
    if (!pg_status) {
        conn.setDbLogin(PGIP,
            user,
            db_password,
            dbname,
            "utf8");
        pg_status = 1;
    }

    if ( pg_status == 1)
    while(true){
      rc = conn.status();

      if (rc == CONNECTION_BAD || rc == CONNECTION_NEEDED) {
            char *c=conn.getMessage();
            if (c) Serial.println(c);
            Serial.println("Error connecting to db");
            pg_status = -1;
            break;
        }

        if (rc == CONNECTION_OK) {
            pg_status = 2;
            Serial.println("Connected to db");
            break;
        }

      if (rc == CONNECTION_AWAITING_RESPONSE || rc == CONNECTION_AUTH_OK) {
            pg_status = 2;
            Serial.println("Waiting for Connection");
        }

      
   }
}

void post2slack(String msg)
{
  epochTime = getTime();
  String formattedTime = timeClient.getFormattedTime();


    
    
    String data = "{\"text\":\"" + formattedTime + ": "  + msg + "\"}";


    httpsClient.begin(secureclient, URL);
    httpsClient.addHeader("Content-Type", "application/json");
    httpsClient.POST(data);
    httpsClient.end();
}
void temperatureService()

{
  
   pg_status = 0;

 
  DynamicJsonDocument doc(1024);

 
    
 String sensor_id = "";
 char q_copy[150];
 String sql_query = "";
  
   setCrossOrigin();
  

  unsigned long startTime;


    
  
  
  
 


 
 

 startTime = millis();

 
digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level




  

 
  doc["ip"] = WiFi.localIP().toString();
  doc["macAddress"] = WiFi.macAddress();
  doc["SSID"] = WiFi.SSID();
  doc["RSSI"] = WiFi.RSSI();

  doc["upTime"] = getUptime();

  temperature = bmp.readTemperature(); // Gets the values of the temperature
  pressure = bmp.readPressure()/1000.0; // Gets the values of the humidity 

  doc["sensorTime"] = millis() - sensorTimer;
  doc["Temperature"] = temperature;
  doc["Pressure"] = pressure;
  doc["Vcc"]  = vccvalue;

  Serial.println("Writing to Database");

   
  connect2db();

  sql_query = "insert into samples (location,sensor,resulttype,result) values('" + WiFi.macAddress() + "','" + "RSSI" + "','" + "dBm" +  "'," + String(WiFi.RSSI()) + ") ";
  Serial.println(sql_query);
  sql_query.toCharArray(q_copy, 150);
  conn.execute(q_copy);
  dbwait();

  sql_query = "insert into samples (location,sensor,resulttype,result) values('" + WiFi.macAddress() +  "','" + "Vcc" +  "','" +  "mV" + "'," + String(vccvalue) + ") ";
  Serial.println(sql_query);
  sql_query.toCharArray(q_copy, 150);
  conn.execute(q_copy);
  dbwait();

  sql_query = "insert into samples (location,sensor,resulttype,result) values('" + WiFi.macAddress() +  "','" + "BMP180" + "','temperature'," + String(temperature) + ") ";
  sql_query.toCharArray(q_copy, 150);
  Serial.println(sql_query);
  conn.execute(q_copy) ;
  dbwait();

  sql_query = "insert into samples (location,sensor,resulttype,result) values('" + WiFi.macAddress() +  "','" + "BMP180" + "','pressure'," + String(pressure) + ") ";
  sql_query.toCharArray(q_copy, 150);
  Serial.println(sql_query);
  conn.execute(q_copy) ;
  dbwait();
   
 
  conn.close();
   
      
 
   



digitalWrite(LED_BUILTIN, HIGH);   // Turn the LED OFF (Note that LOW is the voltage level



doc["elapsedTime"] = millis() - startTime;


  Serial.println(F("Stream..."));
  String buf;
  serializeJson(doc, buf);
  server.send(200, "application/json", buf);
  
  Serial.println(F("done."));



 

 buf.replace('}',']');  
 buf.replace('{','['); 
 buf.replace('\"', '\'' );
 buf.replace(",", ", \n" );  
 Serial.println("```" + buf + "```");
post2slack(buf);



}






 

void sendCrossOriginHeader(){
    Serial.println(F("sendCORSHeader"));
 
    server.sendHeader(F("access-control-allow-credentials"), F("false"));
 
    setCrossOrigin();
 
    server.send(204);
}
 
// Define routing
void restServerRouting() {
    // server.on("/", HTTP_GET, []() {server.send(200, F("text/html"),F("Welcome to the REST Web Server"));});

    
 server.on(F("/temperatureService"), HTTP_OPTIONS, sendCrossOriginHeader);
 server.on(F("/temperatureService"), HTTP_GET, temperatureService);
    

}
 
// Manage not found URL
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


void setupOTA() {
    ArduinoOTA.onStart([]() { Serial.println("Start"); });
    ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
        }
    });

    ArduinoOTA.setHostname(identifier);

    // This is less of a security measure and more a accidential flash prevention
//    ArduinoOTA.setPassword(identifier);
    ArduinoOTA.begin();
}






void setup(void) {

 timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT -5 = -18000
  timeClient.setTimeOffset(-18000);


  pinMode(LED_BUILTIN, OUTPUT);
 

  ESP.wdtEnable(0);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }


  // Clear the buffer
  display.clearDisplay();
 // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();



   if (!bmp.begin()) {
      Serial.println("Could not find a valid BMP085 sensor, check wiring!");
      while (1) {}
   } 
 

  displayIDX = 0;
   
  Serial.begin(9600);
  SerialCom::setup();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  secureclient.setFingerprint(FINGERPRINT);

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

 
  display.clearDisplay();
    display.setTextSize(1); // Draw 2X-scale text
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println((WiFi.localIP()));
    display.println((WiFi.macAddress()));
    display.println((WiFi.SSID()));
    display.println((millis()));
    display.println("------------");
    display.setTextSize(2); // Draw 2X-scale text
    display.print((WiFi.RSSI() ));
    display.println(" dBm");
  display.display();



  
 
  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
 
  // Set server routing
  restServerRouting();

  
  // Set not found response
  server.onNotFound(handleNotFound);
  // Start server
  server.begin();
  Serial.println("HTTP server started");

 
 

  
  
  snprintf(identifier, sizeof(identifier), "VINDRIKTNING-%X", ESP.getChipId());
  setupOTA();

  
 

  post2slack( WiFi.macAddress() + String(":  bootup:") );
   
} 
 
void loop(void) {

  String msg;
  unsigned long currentTimer = millis();
  vccvalue = ESP.getVcc() + voffset;

  epochTime = getTime();
  String formattedTime = timeClient.getFormattedTime();


  ArduinoOTA.handle();
  SerialCom::handleUart(state);

  

    if (currentTimer - previousTimer >= timerInterval) {
      previousTimer = currentTimer;


        display.clearDisplay();
        display.setTextSize(2); // Draw 2X-scale text
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println(("vcc"));
        display.setTextSize(3); // Draw 2X-scale text
        display.setTextColor(WHITE);
        display.setCursor(0, 30);
        display.println(String(vccvalue) );
        display.display();
      
        temperatureService();

     


  
    }

  if (currentTimer - previousBlinkTimer >= blinkInterval) {
    // save the last time you blinked the LED
    previousBlinkTimer = currentTimer;

    ESP.wdtFeed();


  
  // if the LED is off turn it on and vice-versa:
  if (ledState == LOW) {
    ledState = HIGH;
    #
  } else {
    ledState = LOW;
        }





 if (displayIDX == 0){
        display.clearDisplay();
        display.setTextSize(2); // Draw 2X-scale text
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println(("Temp 1"));
        display.setTextSize(3); // Draw 2X-scale text
        display.setTextColor(WHITE);
        display.setCursor(0, 30);
        display.println(String(temperature,1));
        display.display();
    }



 

  if (displayIDX == 1){
    display.clearDisplay();
    display.setTextSize(2); // Draw 2X-scale text
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println(formattedTime);
    display.setTextSize(1); // Draw 2X-scale text
    display.println("------------");
    display.println((WiFi.localIP()));
    display.println((WiFi.macAddress()));
    display.println((WiFi.SSID() + " " + WiFi.RSSI() + " dbM"));
     display.println("------------");
    display.println(String(vccvalue) + "mV");
    display.display();
  }
 
if (displayIDX == 2){
        display.clearDisplay();
        display.setTextSize(2); // Draw 2X-scale text
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println(("Pressure"));
        display.setTextSize(3); // Draw 2X-scale text
        display.setTextColor(WHITE);
        display.setCursor(0, 30);
        display.println(String(pressure,1));
        display.display();
  }

if (displayIDX == 3){
        display.clearDisplay();
        display.setTextSize(2); // Draw 2X-scale text
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println(("Vcc"));
        display.setTextSize(3); // Draw 2X-scale text
        display.setTextColor(WHITE);
        display.setCursor(0, 30);
        display.println(String(vccvalue));
        display.display();
  }

if (displayIDX == 4){
        display.clearDisplay();
        display.setTextSize(2); // Draw 2X-scale text
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println(("UpTime"));
        display.setTextSize(2); // Draw 2X-scale text
        display.setTextColor(WHITE);
        display.setCursor(0, 30);
        display.println(String(getUptime()));
        display.display();
  }


  displayIDX +=1;
  if (displayIDX > 4){
    displayIDX = 0;
  }


  vccvalue = ESP.getVcc() + voffset;



  

    // set the LED with the ledState of the variable:
    digitalWrite(LED_BUILTIN, ledState);
  }
  
  
 server.handleClient();

  
}
