#include <PID_v1.h>
#include <Adafruit_MAX31865.h>

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

double temp_in, humid_in;
double temp_set, humid_set;
double temp_out, humid_out;

//Set PID coefficients
double Kp_t = 2.0, Ki_t = 0.0, Kd_t = 0.0;
double Kp_h, Ki_h, Kd_h;

//For serial commands
String str = "";
bool str_complete = false;

PID tempPID(&temp_in, &temp_out, &temp_set, Kp_t, Ki_t, Kd_t, DIRECT);
PID humidPID(&humid_in, &humid_out, &humid_set, Kp_h, Ki_h, Kd_h, DIRECT);
Adafruit_MAX31865 thermo = Adafruit_MAX31865(CS, MOSI, MISO, SCK);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  str.reserve(200);
  tempPID.SetMode(AUTOMATIC);
  humidPID.SetMode(AUTOMATIC);
  thermo.begin(MAX31865_3WIRE);
}

void loop() {
  // put your main code here, to run repeatedly:
    if (str_complete) {
    str.trim();
    if (str.startsWith("SET_TEMP:")) {
      String val = str.substring(9);
      temp_set = (double)val.toFloat();
    } else {
      //Serial.println("Unknown command: " + str);
    }
    str = "";
    str_complete = false;
  }
  temp_in = thermo.temperature(RNOMINAL, RREF); //Read temperature
  Serial.print("TEMP: ");
  Serial.println(temp_set);
  tempPID.Compute();
  analogWrite(VPIN, (int)temp_out);
}

void serialEvent() {
  while (Serial.available()) {
    char buf = (char)Serial.read();
    if (buf == '\n') {
      str_complete = true;
    } else {
      str += buf;
    }
  }
}
