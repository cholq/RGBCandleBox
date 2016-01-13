# RGBCandleBox

Device used to create "candles" for jack-o-lanterns using RGB LEDs.

The device has a number of potentiometers that are used to choose a color for the various RGB LEDs.  Then, the Adafruit TLC5947 board is used to control up to 8 LEDs.  The code currently allows for 3 sets of potentiometers to be used, and an Adruino Pro Mini was used because it has 9 Analog input pins.  The code also attempts to simulate the flickering of a real candle flame.
