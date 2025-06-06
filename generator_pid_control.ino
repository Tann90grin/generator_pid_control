#include <PID_v1.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_AM2320.h>
#include <Adafruit_MAX31865.h>
#include <Adafruit_MCP4728.h>

Adafruit_MAX31865 thermo = Adafruit_MAX31865(10);
Adafruit_MCP4728 mcp;
Adafruit_AM2320 humid = Adafruit_AM2320();

#define RREF      4300.0
#define RNOMINAL  1000.0

double temp_in, temp_set, temp_out, temp_set_buf;
double humid_in, humid_set, humid_out;

double Kp_t_slow = 50, Ki_t_slow = 3.5, Kd_t_slow = 0;
double Kp_t_fast = 500, Ki_t_fast = 5, Kd_t_fast = 0;
double Kp_h = 10, Ki_h = 1, Kd_h = 0;

String str = "";
bool str_complete = false;
bool fast = false;
bool slow = false;
uint8_t err_counter = 0;

PID tempPID(&temp_in, &temp_out, &temp_set, Kp_t_fast, Ki_t_fast, Kd_t_fast, REVERSE);
PID humidPID(&humid_in, &humid_out, &humid_set, Kp_h, Ki_h, Kd_h, REVERSE);

void setup() {
  Serial.begin(115200);
  str.reserve(200);

  tempPID.SetMode(AUTOMATIC);
  tempPID.SetOutputLimits(0, 4095);
  temp_set = 20.0; // Initialize with a reasonable value
  temp_in = 25.0;  // Initialize with a reasonable value
  Serial.println("PID initialized");
  
  humidPID.SetMode(AUTOMATIC);
  
  thermo.begin(MAX31865_2WIRE);
  humid.begin();

  if (!mcp.begin()){
    Serial.println("Error: MCP4728 not found. Check wiring.");
    while (1);
  }
}

void loop() {

    if (str_complete) {
    str.trim();
    Serial.print("Arduino: Received command: '");
    Serial.print(str);
    Serial.println("'");

    if (str.startsWith("SET_TEMP:")) {
      String val = str.substring(9);
      Serial.print("Arduino: Extracted value: '");
      Serial.print(val);
      Serial.println("'");

      temp_set = (double)val.toFloat();
      temp_set_buf = temp_set;
      Serial.print("SETPOINT_RECEIVED:");
      Serial.println(temp_set);
      err_counter = 0;
    } else {
      Serial.println("Arduino: Command not recognized");
    }
    str = "";
    str_complete = false;
  }

  temp_in = thermo.temperature(RNOMINAL, RREF);
  humid_in = humid.readHumidity();
  Serial.print("TEMP:");
  Serial.println(temp_in);
  Serial.print("DEBUG: temp_in=");
  Serial.print(temp_in);
  Serial.print(", temp_set=");
  Serial.print(temp_set);
  Serial.print(", error=");
  Serial.print(temp_set - temp_in);
  Serial.print("HUMIDITY:");
  Serial.println(humid_in);
  if(err_counter < 10)
  {
    if(temp_set - temp_in < 5 && temp_set - temp_in > -5)
    {
      err_counter++;
    }else
    {
      err_counter = 0;
    }
    if(!fast)
    {
      tempPID.SetTunings(Kp_t_fast, Ki_t_fast, Kd_t_fast);
      fast = true;
      slow = false;
    }
  }else{
    if(!slow)
    {
      tempPID.SetTunings(Kp_t_slow, Ki_t_slow, Kd_t_slow);
      fast = false;
      slow = true;
    }
  }
  tempPID.Compute();
  mcp.setChannelValue(MCP4728_CHANNEL_A, (uint16_t)temp_out);
  Serial.print(", temp_out=");
  Serial.println(temp_out);

  delay(500);
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