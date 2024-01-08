#include <afstandssensor.h>
#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define NEOPIN 0  // On Trinket or Gemma, suggest changing this to 1
#define TRIGGER 6
#define ECHO 5
#define BUZZER_PIN 1
#define TRIP_DISTANCE 91 // in cm. 6ft = 183cm | 4ft = 122 | 3ft = 91

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 12                  // Popular NeoPixel ring size
#define DELAYVAL 500                  // Time (in milliseconds) to pause between pixels \
                                      // UltraSonicSensor(triggerPin, echoPin);
AfstandsSensor sonar(TRIGGER, ECHO);  // initialize ultra sonic on triggerPin and echoPin
Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIN, NEO_GRB + NEO_KHZ800);

int buttonState = 0;
const int buttonPin= 3;
enum class Application : uint8_t {
  Distance,
  Rainbow
};

Application app = { Application::Distance };

void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 4; j++) {  //do 6 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, c);  //turn every third pixel on
      }
      pixels.show();
      //digitalWrite(BUZZER_PIN, HIGH);
      delay(wait);
      //digitalWrite(BUZZER_PIN, LOW);
      for (uint16_t i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, 0);  //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}
void setup() {
  Serial.begin(9600);  //open serial communication for debug and sonar reading
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pixels.begin();
}

void loop() {
  pixels.clear();
  buttonState = digitalRead(buttonPin);
  switch (app) {
    case Application::Distance:
      socialDistance();
      if (buttonState == LOW) {
        app = Application::Rainbow;
      }
      break;

    case Application::Rainbow:
      rainbow(20);
      if (buttonState == LOW) {
        app = Application::Distance;
      }
    break;
  }
  pixels.show();
}

void socialDistance() {
  int reading = sonar.afstandCM();
  Serial.println(reading);
  /*if (reading < 255 ) { /// old code, sets the brightness of neopixels to the same value as the distance measured
    for (int i = 0; i < 12; i++) {
      pixels.setPixelColor(i, pixels.Color(250, 150, 0));
      pixels.setBrightness(reading);
    }
  } */
  if (reading < TRIP_DISTANCE && reading != -1) {
    theaterChase(pixels.Color(50, 0, 0), 65);  //red for 45ms
  } else {
    for (int i = 0; i < 12; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 5, 0));
    }
  }
  delay(500);
}