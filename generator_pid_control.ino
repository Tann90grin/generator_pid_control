#include <PID_v1.h>

// Pins
const int tempPin = A0;
const int valvePin = 9;

double temp_in, humid_in;
double temp_set, humid_set;
double temp_out, humid_out;

//Set PID coefficients
double Kp_t, Ki_t, Kd_t;
double Kp_h, Ki_h, Kd_h;

//For serial commands
String str = "";
bool str_complete = false;

double readTemp();
double readHumid();

PID tempPID(&temp_in, &temp_out, &temp_set, Kp_t, Ki_t, Kd_t, DIRECT);
PID humidPID(&humid_in, &humid_out, &humid_set, Kp_h, Ki_h, Kd_h, DIRECT);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  str.reserve(200);
  tempPID.SetMode(AUTOMATIC);
  humidPID.SetMode(AUTOMATIC);
}

void loop() {
  // put your main code here, to run repeatedly:
    if (str_complete) {
    str.trim();

    if (str.startsWith("SET_TEMP:")) {
      String val = str.substring(9);
      temp_set = val.toFloat();
      Serial.print("TEMP: ");
      Serial.println(temp_set);
    } else {
      Serial.println("Unknown command: " + str);
    }
    str = "";
    str_complete = false;
  }
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
