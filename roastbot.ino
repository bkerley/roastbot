/***************************************************
  This is an example for the Adafruit Thermocouple Sensor w/MAX31855K

  Designed specifically to work with the Adafruit Thermocouple Sensor
  ----> https://www.adafruit.com/products/269

  These displays use SPI to communicate, 3 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <SPI.h>
#include "Adafruit_MAX31855.h"

// Default connection is using software SPI, but comment and uncomment one of
// the two examples below to switch between software SPI and hardware SPI:

// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define MAXDO   3
#define MAXCS   4
#define MAXCLK  5

#define TARGET_F 85.0
#define TARGET_F_RANGE 5.0
#define HEATER_PIN 13

int cycle_counter = 0;
int heater_running = 0;

// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

// Example creating a thermocouple instance with hardware SPI (Uno/Mega only)
// on a given CS pin.
//#define MAXCS   10
//Adafruit_MAX31855 thermocouple(MAXCS);

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

void setup() {
  #ifndef ESP8266
    while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
  #endif

  Serial.begin(9600);
  Serial.println("roastbot initializing");

  cycle_counter = 0;
  heater_running = 0;

  Serial.println("configuring heater pin");
  pinMode(HEATER_PIN, OUTPUT);
  analogWrite(HEATER_PIN, HIGH);
  delay(50);
  analogWrite(HEATER_PIN, LOW);

  Serial.println("waiting on MAX31855");
  // wait for MAX chip to stabilize
  delay(500);
}

void check_to_stop(double f);
void check_to_start(double f);
void status_print();

double c_to_f(double c);

void loop() {
  double f = thermocouple.readFarenheit();

  if (heater_running) check_to_stop(f);
  else check_to_start(f);

  if (cycle_counter == 0) status_print();
  cycle_counter = (cycle_counter + 1) % 63;
}


void check_to_stop(double f) {
  if (f < (TARGET_F + TARGET_F_RANGE)) return;

  heater_running = 0;
  digitalWrite(HEATER_PIN, LOW);
}

void check_to_start(double f) {
  if (f > (TARGET_F - TARGET_F_RANGE)) return;

  heater_running = 1;
  digitalWrite(HEATER_PIN, HIGH);
}

void status_print() {
  Serial.print("internal temp F ");
  Serial.println(c_to_f(thermocouple.readInternal()));

  Serial.print("thermocouple temp F ");
  Serial.println(thermocouple.readFarenheit());

  Serial.print("heater on? ");
  Serial.println(heater_running);
}

double c_to_f(double c) {
  return (c * 9.0/5.0) + 32.0;
}
