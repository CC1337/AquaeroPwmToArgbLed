#include <FastLED.h>

// Converts PWM 3.3V RGB signal on LED_INPUT_R_PIN / LED_INPUT_G_PIN / LED_INPUT_B_PIN to ARGB output on LED_OUT_ARGB_PIN
// Different settings can be used to tune duty cycles and frequency. These settings were tested with Aqua Computer Aquaero 6.
// Tested on Arduion Pro Mini with ATmega 328P 8MHz 3.3V version, should run on ESPs as well

// ---------- CONFIG ----------

#define LED_OUT_ARGB_PIN  13
#define LED_INPUT_R_PIN   A0
#define LED_INPUT_G_PIN   A1
#define LED_INPUT_B_PIN   A2
#define NUM_LEDS          60        // 20 per SL-INF120
#define BRIGHTNESS        255
#define LED_TYPE          WS2811    // Fits for Lian Li SL-INF120
#define COLOR_ORDER       GRB       // Fits for Lian Li SL-INF120
#define SERIALDEBUG       false
#define ENABLE_CROSSFADING true     // Smooth out stuttering fading steps of Aquaero

#define PULSE_TIMEOUT     25000     // aquaero: 25ms / 25000us is enough
#define AQUAREO_MAX_BRIGHNTESS 228  // stretch to 255 later for reaching max brightness in reality

// ----------------------------

CRGB leds[NUM_LEDS];
byte color_r, color_g, color_b, color_r_new, color_g_new, color_b_new;
float aquaeroMaxValueMultiplier = 255.0 * (255.0 / (float)AQUAREO_MAX_BRIGHNTESS); // Stretch to real 255 as aquaero does not reach 100% duty cycle
byte loopCounter = 42;

void setup() {
  if (SERIALDEBUG)
    Serial.begin(115200);
  delay( 1337 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
}

void loop()
{
  if (loopCounter > 10) {
    loopCounter = 0;
    readNewColors();
  }
  renderLeds();
  if (SERIALDEBUG)
    dumpColors();
  delay(25);
  loopCounter++;
}

void renderLeds() {
  fillLEDs();
  FastLED.show();
}

void dumpColors() {
  Serial.print("TARGET: R ");
  Serial.print(color_r_new);
  Serial.print(" | G ");
  Serial.print(color_g_new);
  Serial.print(" | B ");
  Serial.print(color_b_new);
  Serial.print("   ---   CURRENT: R ");
  Serial.print(color_r);
  Serial.print(" | G ");
  Serial.print(color_g);
  Serial.print(" | B ");
  Serial.println(color_b);
}

void readNewColors() {
  color_r_new = (byte)min(getDutyCycle(LED_INPUT_R_PIN) * aquaeroMaxValueMultiplier, 255.0);
  color_g_new = (byte)min(getDutyCycle(LED_INPUT_G_PIN) * aquaeroMaxValueMultiplier, 255.0);
  color_b_new = (byte)min(getDutyCycle(LED_INPUT_B_PIN) * aquaeroMaxValueMultiplier, 255.0);
}

void fillLEDs()
{
  color_r = getBlendedColor(color_r_new, color_r);
  color_g = getBlendedColor(color_g_new, color_g);
  color_b = getBlendedColor(color_b_new, color_b);

  fill_solid(leds, NUM_LEDS, CRGB(color_r,color_g,color_b));
}

byte getBlendedColor(byte newColor, byte oldColor) {
  if (newColor == oldColor || !ENABLE_CROSSFADING)
    return newColor;

  byte modifier = (byte)max((float)abs(newColor - oldColor)/15.0, 1.0);
  if (newColor < oldColor)
    return oldColor - modifier;
  else
    return oldColor + modifier;
}

float getDutyCycle(int inputPin) {
  unsigned long highTime = pulseIn(inputPin, HIGH, PULSE_TIMEOUT);
  renderLeds(); // Render in between long-running operation for less stutter in crossfades
  
  unsigned long lowTime = pulseIn(inputPin, LOW, PULSE_TIMEOUT);
  renderLeds(); // Render in between long-running operation for less stutter in crossfades
  
  unsigned long cycleTime = max(highTime + lowTime, 1);
  float dutyCycle = (float)highTime / float(cycleTime);
  
  return dutyCycle;
}
