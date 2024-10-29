# AquaeroPwmToArgbLed
### Converts a PWM RGB signal to Addressable RGB (ARGB) ouput, initially for syncing LianLi SL-INF120 fans to Aqua Computer Aquaero 6 LED Output

## Description

* converts PWM 3.3V RGB signal on LED_INPUT_R_PIN / LED_INPUT_G_PIN / LED_INPUT_B_PIN to ARGB output on LED_OUT_ARGB_PIN
* can interpolate/smoothen stuttering crossfade inputs 
* different settings can be used to tune duty cycles and frequency. These settings were tested with Aqua Computer Aquaero 6.
* tested on Arduino Pro Mini with ATmega 328P 8MHz 3.3V version, should easily run on ESPs as well
* used 10k PullDown resistor to GND for inputs