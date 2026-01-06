// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI


//------------------------ Libraries --------------------------
#include <FastLED.h> // Library Adafruit NeoPixel by Adafruit


#define TIME_BETWEEN_CHANGES 5000 // 5seconds
#define LIMIAR_ANALOG 1500

//-------------------------- LED Tape -------------------------
#define NUM_LEDS 298
#define DATA_PIN 15
#define CLOCK_PIN 13
#define BUTTON_TURN_STATE 34



bool ledTapeIsOn = true;
bool ledTapeAutoChange = true;
unsigned int colorCounter = 0;
unsigned int startPoint = 0;
unsigned int endPoint = NUM_LEDS;
unsigned long int timeDelay=0;


// Define the array of leds
CRGB leds[NUM_LEDS];
CRGB color= CRGB::Yellow;
CRGB colors[]={CRGB::White, CRGB::Red, CRGB::Yellow, CRGB::Orange, CRGB::Pink, CRGB::Blue, CRGB::Green, CRGB::Purple, CRGB::Aqua, CRGB::LightCoral, CRGB::DarkOrange, CRGB::Aquamarine, CRGB::DarkBlue };


void setup() { 
  Serial.begin(115200);
  delay(1000);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
  for(int i=startPoint; i < endPoint; i++)
    leds[i] = color;
  FastLED.show();  
}

void loop() { 

  handleChangeTapeStateButton();

  if((millis()-timeDelay>TIME_BETWEEN_CHANGES))
  {
    timeDelay= millis();

    if(ledTapeIsOn && ledTapeAutoChange)
      changeTapeColor();

    if(ledTapeIsOn)
      updateTapeState();
  }

  delay(5);
}

void handleChangeTapeStateButton(){
  if(analogRead(BUTTON_TURN_STATE)<LIMIAR_ANALOG)
  {
    while(analogRead(BUTTON_TURN_STATE)<LIMIAR_ANALOG)blynkTapeState();
    ledTapeIsOn = !ledTapeIsOn;
    if(!ledTapeIsOn)
      turnOffTape();
  }
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
