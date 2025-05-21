#include <Adafruit_MAX31865.h>
#include <pid-autotune.h>

//For autotuning pid coefficients with Ziegler–Nichols method

// Pins
#define CS 10
#define MOSI 11
#define MISO 12
#define SCK 13
#define VPIN A0

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      4300.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  1000.0

Adafruit_MAX31865 thermo = Adafruit_MAX31865(CS, MOSI, MISO, SCK);
PID pid = PID();
pid_tuner tuner = pid_tuner(pid, 10, 1000000, pid_tuner::CLASSIC_PID);

double inputFunc(bool holder) {
  return thermo.temperature(RNOMINAL, RREF);
}

void outputFunc(double x) {
  analogWrite(A0, x);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  thermo.begin(MAX31865_3WIRE);
  tuner.setConstrains(0, 255);
  tuner.setTargetValue(-100.0);
  tuner.tune(inputFunc, 1, outputFunc);
}

void loop() {
  // put your main code here, to run repeatedly:

}
