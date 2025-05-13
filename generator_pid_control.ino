#include <PID_v1.h>

// Pins
const int tempPin;
const int valvePin;

float temp_in, humid_in;
float temp_set, humid_set;
float temp_out, humid_out;

//Set PID coefficients
float Kp_t, Ki_t, Kd_t;
float Kp_h, Ki_h, Kd_h;

PID tempPID(&temp_in, &temp_out, &temp_set, Kp_t, Ki_t, Kd_t, DIRECT);
PID humidPID(&humid_in, &humid_out, &humid_set, Kp_h, Ki_h, Kd_h, DIRECT);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  tempPID.SetMode(AUTOMATIC);
  humidityPID.SetMode(AUTOMATIC);
}

void loop() {
  // put your main code here, to run repeatedly:

}
