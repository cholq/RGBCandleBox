#include "Adafruit_TLC5947.h"

// constants used in class
const uint8_t PROGMEM gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };
    
enum CandleState {StateUnknown, StateShowStd, StateShowDiff};
enum LEDColor {ColorUnknown, ColorRed, ColorGreen, ColorBlue};

// How many boards do you have chained?
#define NUM_TLC5974 1

#define data   4
#define clock   5
#define latch   6
#define oe  -1  // set to -1 to not use the enable pin (its optional)

Adafruit_TLC5947 tlc = Adafruit_TLC5947(NUM_TLC5974, clock, data, latch);

class RGB_Candle 
{
  
  // pins to read RGB values set by user
  int mRedPin;
  int mGreenPin;
  int mBluePin;
  
  // vars to store RGB values set by user
  int mRedValue;
  int mGreenValue;
  int mBlueValue;
  
  // vars to manage state of candle
  CandleState mState;
  unsigned long mPrevMillis;
  int mDelay;
  int mTLCIndex;
  
  public:
  
  RGB_Candle() 
  {

    mRedPin = 0;
    mGreenPin = 0;
    mBluePin = 0;
    mState = StateUnknown;
    mPrevMillis = 0;
    mDelay = 0;
    mTLCIndex = 0;
  }
  
  void begin(int pRedPin, int pGreenPin, int pBluePin, int pTLCIndex)
  {
    mRedPin = pRedPin;
    mGreenPin = pGreenPin;
    mBluePin = pBluePin;
    mTLCIndex = pTLCIndex;
  }
  
  void Flicker(unsigned long pCurrMillis) 
  {
    
    if (mState == StateShowStd) 
    {
      if (pCurrMillis - mPrevMillis >= mDelay) 
      {
        readAllValues();
        //print_debug();
        writeAllValues(mRedValue, mGreenValue, mBlueValue);
        mDelay = random(0,200);
        mState = StateShowDiff;
        mPrevMillis = pCurrMillis;
      }
    }
    else if (mState == StateShowDiff) 
    {
      if (pCurrMillis - mPrevMillis >= mDelay) 
      {
        // only vary the color on highest value to create flicker effect
        readAllValues();
        //print_debug();
        switch (determineHighestValue(mRedValue, mGreenValue, mBlueValue)) {
          case ColorRed:
            writeAllValues(calculateFlickerDiff(mRedValue), mGreenValue, mBlueValue);
            break;
          case ColorGreen:
            writeAllValues(mRedValue, calculateFlickerDiff(mGreenValue), mBlueValue);
            break;
          case ColorBlue:
            writeAllValues(mRedValue, mGreenValue, calculateFlickerDiff(mBlueValue));
            break;
        }
        mDelay = random(0,100);
        mState = StateShowStd;
        mPrevMillis = pCurrMillis;
      }
    }
    else if (mState == StateUnknown) 
    {
      readAllValues();
      mDelay = 0;
      mState = StateShowStd;
      mPrevMillis = pCurrMillis;
    }
  }
  
    void print_debug() 
    {
      Serial.println("---------------------");
      
      Serial.print("  TLC Index=");
      Serial.println(mTLCIndex);
      Serial.print("Red pin=");
      Serial.print(mRedPin);
      Serial.print("  value=");
      Serial.println(mRedValue);
      Serial.print("Green=");
      Serial.print(mGreenPin);
      Serial.print("  value=");
      Serial.println(mGreenValue);
      Serial.print("Blue=");
      Serial.print(mBluePin);
      Serial.print("  value=");
      Serial.println(mBlueValue);
    }
  
  private:
    void readAllValues() {
      mRedValue = calcRGBDisplayValue(analogRead(mRedPin)); 
      mGreenValue = calcRGBDisplayValue(analogRead(mGreenPin)); 
      mBlueValue = calcRGBDisplayValue(analogRead(mBluePin)); 
           
      //print_debug();
      
    }

    int calcRGBDisplayValue(int pInValue) {
      return pInValue / 4;
    }
    
    void writeAllValues(int pRedValue, int pGreenValue, int pBlueValue) {
      
      uint16_t r = pgm_read_byte(&gamma[pRedValue]);  
      uint16_t g = pgm_read_byte(&gamma[pGreenValue]);  
      uint16_t b = pgm_read_byte(&gamma[pBlueValue]);
      
      tlc.setLED(mTLCIndex, (r*16), (g*16), (b*16));
      tlc.write();
    }
  
    LEDColor determineHighestValue (int pRed, int pGreen, int pBlue) {
      
      LEDColor lReturn = ColorUnknown;
      
      if (pRed > pBlue) {
        if (pRed > pGreen) {
          if (pRed > 28) {
            lReturn = ColorRed;
          }
        }
        else {
          if (pGreen > 28) {
            lReturn = ColorGreen;
          }
        }
      }
      else {
        if (pBlue > pGreen) {
          if (pBlue > 28) {
            lReturn = ColorBlue;
          }
        }
        else{
          if (pGreen > 28) {
            lReturn = ColorGreen;
          }
        }
      }
      
      return lReturn;
    }
    
    int calculateFlickerDiff (int pStartVal) {
      
      int intReturn = 0;
      int intRange = 0;
      int intLow = pStartVal;
      int intHigh = pStartVal;
      
      // calc range used for random function
      if (pStartVal <= 83) {
        intRange = 40;
      }
      else if (pStartVal <= 166) {
        intRange = 50;
      }
      else {
        intRange = 60;
      }
      
      // calculate return value
      if (pStartVal >= (255 - intRange)) {
        intLow = pStartVal - intRange;
        intHigh = 255;
      }
      else if (pStartVal <= intRange) {
        intLow = 0;
        intHigh = pStartVal + intRange;
      }
      else {
        intLow = pStartVal - intRange;
        intHigh = pStartVal + intRange;
      }
      
      intReturn = random(intLow, intHigh);
      
      return intReturn;
    }
  
};

RGB_Candle candle5 = RGB_Candle();
RGB_Candle candle6 = RGB_Candle();
RGB_Candle candle7 = RGB_Candle();

void setup() {
  
  Serial.begin(9600);

  //delay(5000);

  candle5.begin(A3,A4,A5,5);
  candle6.begin(A9,A11,A10,6);
  candle7.begin(A1,A2,A0,7);

  tlc.begin();
  if (oe >= 0) {
    pinMode(oe, OUTPUT);
    digitalWrite(oe, LOW);
  }
  
  // timer interup set on timer0
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
  
}

// method called by interup on timer0
SIGNAL(TIMER0_COMPA_vect) 
{
  unsigned long lCurrMillis = millis();
  candle5.Flicker(lCurrMillis);
  //candle6.Flicker(lCurrMillis);
  //candle7.Flicker(lCurrMillis);
}

void loop() 
{ 
}
