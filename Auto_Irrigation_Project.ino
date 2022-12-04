#include <SoftwareSerial.h>

SoftwareSerial espSerial(5, 6);

int moisture_sensor1 = A0;
int water_pump1 = 7;
int water_pump2 = 6;
int moisture_sensor2 = A1;

enum pump {
  pumpOff = 0,
  pumpOn = 1
};

pump state_1 = pumpOff;
pump state_2 = pumpOff;

void setup() {
  Serial.begin(9600);
  espSerial.begin(115200);
  espSerial.setTimeout(1);
  pinMode(water_pump1, OUTPUT);
  pinMode(water_pump2, OUTPUT);
};

String enumToString(pump &state) {
  if (state == 0)
    return "0";
  if (state == 1)
    return "1";
};

void loop() {
  waterPumpSystem(water_pump1, moisture_sensor1, state_1);
//  waterPumpSystem(water_pump2, moisture_sensor2, state_2)
  handleManualButtonPress(state_1);
};

void handleManualButtonPress(pump &state) {
   Serial.println(espSerial.available());
  if(espSerial.available() > 0) {
    String inputFromEsp;
    inputFromEsp += (char) espSerial.read();
    Serial.println(inputFromEsp);
    bool button_state = inputFromEsp.toInt();
    
    if(button_state){
      Serial.println("PRESSED");
      state = 1;
    } else {
      Serial.println("RELEASED");
      state = 0;
    }
  }
  delay(1);
};

void waterPumpSystem(int digital_water_pump_number, int analog_moisture_sensor_number, pump &state) {

  // Calculating Moisture Level
  float moist_level_res = analogRead(analog_moisture_sensor_number);
  float moist_level_percent = 100 - ((moist_level_res / 1023) * 100);
  Serial.print("Soil Moisture Level: ");
  Serial.print(moist_level_percent);
  Serial.print("%          ");

  // Running Water Pump Logic
  float least_amount_water = 40;
  float most_amount_water = 69.99;
  if (moist_level_percent < least_amount_water) {
    state = pumpOn;
    digitalWrite(digital_water_pump_number, HIGH);
  } else if (state == pumpOn && moist_level_percent > least_amount_water && moist_level_percent < most_amount_water) {
    state = pumpOn;
    digitalWrite(digital_water_pump_number, HIGH);
  } else {
    state = pumpOff; 
    digitalWrite(digital_water_pump_number, LOW);
  };
  
  String moistureAndPumpState = String(moist_level_percent) + "%" + enumToString(state);
  espSerial.println(moistureAndPumpState);
  
  Serial.print("Water Pump State: ");
  Serial.println(enumToString(state));
};
