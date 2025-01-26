//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

//------------------------ Libraries --------------------------
#include <FastLED.h>  // Library Adafruit NeoPixel by Adafruit
#include <Wire.h>                                        /////
#include "SPIFFS.h"                                      /////
#include <LiquidCrystal_I2C.h>                           /////
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
//////////////////////////////////////////////////////////////

#define TIME_BETWEEN_CHANGES 5000  // 5seconds
#define LIMIAR_ANALOG 1500

////////////////  PIN CONFIGURATIONS ///////////////
#define LED 2     /////
#define BUZZER 4  /////´
#define LIGHTS 5  /////´
#define BUTTON_TURN_STATE 18

//-------------------------- LED Tape -------------------------
#define NUM_LEDS 298
#define DATA_PIN 15
#define CLOCK_PIN 13


const char* ssid = "MHN";
const char* password = "575frut4mba";


bool ledTapeIsOn = true;
bool ledTapeAnimation = true;
unsigned int colorCounter = 0;
unsigned int startPoint = 0;
unsigned int endPoint = NUM_LEDS;
unsigned long int timeDelay = 0;

char projectState = 'A';
float temperature=0.0, humidity=0.0;

const char *PARAM_MESSAGE = "message";

CRGB leds[NUM_LEDS];
CRGB color = CRGB::Yellow;
CRGB colors[] = { CRGB::White, CRGB::Red, CRGB::Yellow, CRGB::Orange, CRGB::Pink, CRGB::Blue, CRGB::Green, CRGB::Purple, CRGB::Aqua, CRGB::LightCoral, CRGB::DarkOrange, CRGB::Aquamarine, CRGB::DarkBlue };

AsyncWebServer server(80);
LiquidCrystal_I2C lcd(0x27, 20, 4);  /////


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  initConfig();
  wifiConfig();
  Serial.println(initMyFS() ? " ## PARTIÇÃO SPIFFS MONTADA! ##" : " ## ERRO MONTANDO A PARTIÇÃO SPIFFS ##");
  serverHandlers();
  Serial.println(" ##-------MHA ON!--------##");
}


void loop() {
  static byte cont = 0;
  if (millis() - timeDelay > 1000) {
    timeDelay = millis();
    readSensors();
    if (++cont == 2) {
      cont = 0;
      printLCD();
    }

    // -----------LED Tape ---------------
    if(ledTapeIsOn && ledTapeAnimation)
      changeTapeColor();
    if(ledTapeIsOn)
      updateTapeState();

    digitalWrite(LED, !digitalRead(LED));
  }

  buttonsHandler();
  delay(1);
}

////////////////////////////////////////////////////
void initConfig() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  
  pinMode(LIGHTS, OUTPUT);
  digitalWrite(LIGHTS, LOW);


  pinMode(BUTTON_TURN_STATE, INPUT_PULLUP);

  Serial.begin(115200);
  delay(500);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("######## MHA #########");
  lcd.setCursor(0, 1);
  lcd.print("# RESIDENCIAL SYSTEM #");
  lcd.setCursor(0, 2);
  lcd.print("BY:ERIVELTO SILVA");
  lcd.setCursor(0, 3);
  lcd.print("#####################");

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  for(int i=startPoint; i < endPoint; i++)
    leds[i] = color;
  FastLED.show();
}

/////////////////////////////////////////////////////
void wifiConfig() {
  if (WiFi.status() == WL_CONNECTED)
    return;
  Serial.println();
  Serial.print("CONECTANDO A WIFI:");
  Serial.println(ssid);
  Serial.print("PROCURANDO");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CONECTANDO A REDE:");
  lcd.setCursor(5, 1);
  lcd.print(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  for (int i = 0; WiFi.status() != WL_CONNECTED; i++) {
    digitalWrite(LED, !digitalRead(LED));
    delay(150);
    Serial.print(".");
    lcd.print(".");
    if (i == 100)
      ESP.restart();
  }
  Serial.print("\nCONECTADO AO WIFI NO IP:");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("##  CONECTADO ##");
  lcd.setCursor(0, 1);
  lcd.print("REDE:");
  lcd.print(ssid);
  lcd.setCursor(0, 2);
  lcd.print("IP:");
  lcd.print(WiFi.localIP());
  delay(5000);
}

void turnOnTape(){
  for(int i = startPoint; i<endPoint; i++)
    leds[i] = CRGB::Red;
  FastLED.show();
}

void turnOffTape(){
  for(int i = startPoint; i<endPoint; i++)
    leds[i] = CRGB::Black;
  FastLED.show();
}


void blynkTapeState(){
  turnOffTape();
  delay(50);
  turnOnTape();
  delay(50);
}


void updateTapeState(){
  for(int i = startPoint; i<endPoint; i++)
    leds[i] = color;
  FastLED.show();
}

void changeTapeColor()
{
  if(++colorCounter>12) colorCounter=0;
    color = colors[colorCounter];
}


//////////////////////////////////////////////////////
void readSensors() {
}

////////////////////////////////////////////////////////////////////////
bool initMyFS() {
  return (SPIFFS.begin(true));
}

void buttonsHandler() {
  if (!digitalRead(BUTTON_TURN_STATE)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("####################");
    lcd.setCursor(0, 1);
    lcd.print(" FITA DE LED");
    lcd.setCursor(0, 2);
    lcd.print(" ESTADO PARA " + String((ledTapeIsOn) ? "OFF" : "ON"));
    lcd.setCursor(0, 3);
    lcd.print("####################");
    while(!digitalRead(BUTTON_TURN_STATE))blynkTapeState();
    ledTapeIsOn = !ledTapeIsOn;
    if(!ledTapeIsOn)
      turnOffTape();
  }
  // if(analogRead(BUTTON_TURN_STATE)<LIMIAR_ANALOG)
  // {
  //   while(analogRead(BUTTON_TURN_STATE)<LIMIAR_ANALOG)blynkTapeState();
  //   ledTapeIsOn = !ledTapeIsOn;
  //   if(!ledTapeIsOn)
  //     turnOffTape();
  // }
}

bool isBuzzerOn() {
  return (digitalRead(BUZZER));
}

void bipBuzzer() {
  bool buzzerPreviewStatus = isBuzzerOn();
  turnOffBuzzer();
  turnOnBuzzer();
  delay(250);
  turnOffBuzzer();
  delay(250);
  turnOnBuzzer();
  delay(250);
  turnOffBuzzer();

  if (buzzerPreviewStatus)
    turnOnBuzzer();
  else
    turnOffBuzzer();
}

void turnOnBuzzer() {
  digitalWrite(BUZZER, HIGH);
}

void turnOffBuzzer() {
  digitalWrite(BUZZER, LOW);
}

bool isLightsOn() {
  return digitalRead(LIGHTS);
}

void turnOffLights() {
  digitalWrite(LIGHTS, LOW);
}

void turnOnLights() {
  digitalWrite(LIGHTS, HIGH);
}

void printLCD() {
  lcd.init();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("TEMPERTURA:");
  lcd.print(temperature);
  lcd.print("*C");

  lcd.setCursor(0, 1);
  lcd.print("HUM. AR...:");
  lcd.print(humidity);
  lcd.print("%");

  lcd.setCursor(19, 3);
  lcd.print(projectState);
}

////////////////////////////////////////////////////////////////////////
void serverHandlers() {
  // Route to load bootstrap.min.css file
  server.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/bootstrap.min.css", "text/css");
  });

  // Route to load bootstrap.bundle.min.js file
  server.on("/bootstrap.bundle.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/bootstrap.bundle.min.js", "text/javascript");
  });

  // Route to load login.css file
  server.on("/login.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/login.css", "text/css");
  });

  // Route to load dashboard.css file
  server.on("/dashboard.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/dashboard.css", "text/css");
  });

  // Route to load dashboard.js file
  server.on("/dashboard.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/dashboard.js", "text/javascript");
  });




  server.on("/loginIcon.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/loginIcon.png", "image/jpeg");
  });
  server.on("/humidity.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/humidity.png", "image/jpeg");
  });

  server.on("/lampOff.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/lampOff.png", "image/jpeg");
  });

  server.on("/lampOn.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/lampOn.png", "image/jpeg");
  });

  server.on("/fire.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/fire.png", "image/jpeg");
  });

  server.on("/nofire.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/nofire.png", "image/jpeg");
  });

  server.on("/nosmoke.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/nosmoke.png", "image/jpeg");
  });

  server.on("/smoke.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/smoke.png", "image/jpeg");
  });

  server.on("/tel.jpg", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/tel.jpg", "image/jpeg");
  });

  server.on("/temperature.jpeg", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/temperature.jpeg", "image/jpeg");
  });

  server.on("/pumpOff.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/pumpOff.png", "image/jpeg");
  });

  server.on("/pumpOn.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/pumpOn.png", "image/jpeg");
  });

  server.on("/estufa-capa.jpg", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/estufa-capa.jpg", "image/jpeg");
  });

  server.on("/soil.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/soil.png", "image/jpeg");
  });




  /*--------------------------ENDPOINS---------------------------*/
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("-------->redirecionando para login.html");
    request->redirect("/login");
  });



  // POST request para login
  server.on("/getin", HTTP_POST, [](AsyncWebServerRequest *request) {
    Serial.println("------> Req de /getin do login.html");
    if (!(request->hasParam("username", true) && request->hasParam("password", true))) {
      Serial.println("---> Erro... Parametros de Login Incompletos!\nRedirecionando para ----> login.html");
      request->redirect("/login");
    } 
    else {
      String username = request->getParam("username", true)->value();
      String password = request->getParam("password", true)->value();
      Serial.println("Dados:"+String(username)+String(password));
      if (username.equals("admin") && password.equals("admin1234")) {
        Serial.println("--->Username e Senha de Admin Válidos!\nRedirecionando para ----> register.html");
        request->redirect("/dashboard");
      }
      else {
        Serial.println("--->Username e Senha Válidos!\nRedirecionando para ---> index.html");
        request->redirect("/dashboard");
      }
    }
  });


  // POST request para Cadastro de Usuario
  server.on("/make-register", HTTP_POST, [](AsyncWebServerRequest *request) {
    Serial.println("------> Req de /register do register.html");
    if (!(request->hasParam("username", true) && request->hasParam("passwordUser", true) && request->hasParam("passwordAdmin", true))) {
      Serial.println("---> Erro... Parametros de Login Incompletos!\nRedirecionando para ----> login.html");
      request->redirect("/login");
    } else {
      String username = request->getParam("username", true)->value();
      String passwordUser = request->getParam("passwordUser", true)->value();
      String passwordAdmin = request->getParam("passwordAdmin", true)->value();

      if (passwordAdmin.equals("otlevire")) {
        Serial.println("--->Admin Válido! Salvando dados do novo usuario no Firebase...");
        //Firebase.setString("/users/" + username, passwordUser);
        //saveUserFirebase(username, passwordUser);
        Serial.println("Dados do Usuario Salvos!");
        request->redirect("/dashboard");
      } else {
        Serial.println("--->Admin Inválido!Redirecionando para register.html");
        request->redirect("/register");
      }
    }
  });

  // Route for login / web page
  server.on("/login", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("--------> login.html");
    request->send(SPIFFS, "/login.html");
  });

  // Route for root / web page
  server.on("/register", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("--------> register.html");
    request->send(SPIFFS, "/register.html");
  });

  // Route for root / web page
  server.on("/dashboard", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("--------> dashboard.html");
    request->send(SPIFFS, "/dashboard.html");
  });



  server.on("/dados", HTTP_GET, [](AsyncWebServerRequest *request) {
    /*
      D ------- 0
      temp ---- 1
      humid --- 2
      projectState ---- 3
      lampState --- 4
      ledTapeState --- 5
    */
    String dataStored = "D*" + String(temperature) + "*" + String(humidity) + "*" + String(projectState) + "*" + String((isLightsOn())?"1":"0")+ "*" + String(ledTapeIsOn? "1" : "0") + "*" +  String(ledTapeAnimation ? "1" : "0") + "*";

    Serial.println("--------> Dados:" + dataStored);
    String resp = "{\"status\":\"success\", \"data\":\"" + dataStored + "\"}";
    request->send(200, "application/json", resp);
  });

  server.on("/mode", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("--------> mode");
    projectState = (projectState == 'A') ? 'M' : 'A';
    request->send(200, "application/json", "{\"status\":\"success\"}");
  });

  server.on("/lights", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("--------> luzes");
    if (projectState == 'M') {
      if (isLightsOn())
        turnOffLights();

      else
        turnOnLights();
      request->send(200, "application/json", "{\"status\":\"success\"}");
    } else {
      request->send(200, "application/json", "{\"status\":\"error\", \"message\":\"O Sistema está no modo Automático\"}");
    }
  });

  server.on("/ledTapeState", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("--------> ledTapeState");
    if (projectState == 'M') {
      ledTapeIsOn = !ledTapeIsOn;
      if(!ledTapeIsOn)
        turnOffTape();
      else
        turnOnTape();
      request->send(200, "application/json", "{\"status\":\"success\"}");
    } else
      request->send(200, "application/json", "{\"status\":\"error\", \"message\":\"O Sistema está no modo Automático\"}");
  });
  
  server.on("/ledTapeAnimation", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("--------> ledTapeAnimation");
    if (projectState == 'M') {
      ledTapeIsOn = true;
      ledTapeAnimation = !ledTapeAnimation;
      request->send(200, "application/json", "{\"status\":\"success\"}");
    } else
      request->send(200, "application/json", "{\"status\":\"error\", \"message\":\"O Sistema está no modo Automático\"}");
  });

  server.on("/ledTapeChange", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("--------> ledTapeChange");
    if (projectState == 'M') {
      changeTapeColor();
      request->send(200, "application/json", "{\"status\":\"success\"}");
    } 
    else
      request->send(200, "application/json", "{\"status\":\"error\", \"message\":\"O Sistema está no modo Automático\"}");
  });


  server.onNotFound(notFound);
  server.begin();
}

