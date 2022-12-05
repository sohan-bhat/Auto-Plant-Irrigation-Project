#include <SoftwareSerial.h>

SoftwareSerial espSerial(2, 11);

int moisture_sensor1 = A0;
int water_pump1 = 7;
int water_pump2 = 6;
int moisture_sensor2 = A1;

enum pump {
  pumpOff = 0,
  pumpOn = 1,
  manualPumpOn = 2
};

pump state_1 = pumpOff;
pump state_2 = pumpOff;

void setup() {
  Serial.begin(9600);
  espSerial.begin(115200);
  espSerial.setTimeout(1);
  pinMode(water_pump1, OUTPUT);
  pinMode(water_pump2, OUTPUT);
}

String enumToString(pump &state) {
//  if (state == 0)
//    return "0";
//  if (state == 1)
//    return "1";
//  if (state == 2)
//    return "2";

  switch (state) {
    case 0:
      return "0";
      break;
    case 1:
      return "1";
      break;
    case 2:
      return "2";
      break;
    defualt:
      break;
  }
}

void loop() {
  irrigationSystem(water_pump1, moisture_sensor1, state_1);
}

void irrigationSystem(int digital_water_pump_port, int analog_moisture_sensor_port, pump &state){
  float moist_level_percent = 0;
  waterPumpSystem(digital_water_pump_port, analog_moisture_sensor_port, state, moist_level_percent);
  handleManualButtonPress(state);
  reportInfo(state, moist_level_percent);
  pumpOnOrOff(digital_water_pump_port, state);
}

void reportInfo(pump &state, float &moist_level_percent){
  Serial.print("Soil Moisture Level: ");
  Serial.print(moist_level_percent);
  Serial.print("%          ");
  Serial.print("Water Pump State: ");
  Serial.println(enumToString(state));
  
  String moistureAndPumpState = String(moist_level_percent) + "%" + enumToString(state);
  espSerial.println(moistureAndPumpState);
}

void handleManualButtonPress(pump &state) {
   Serial.println(espSerial.available());
  if(espSerial.available() > 0) {
    String inputFromEsp = espSerial.readString();
    Serial.println(inputFromEsp);
    // ?????????????????0?????????1?????
    char espButtonInput;
    for(int i = 0; i < inputFromEsp.length(); i++){
      if(inputFromEsp[i] == '0' || inputFromEsp[i] == '1')
        espButtonInput = inputFromEsp[i];
    }
    
    if(espButtonInput == '1'){
      Serial.println("PRESSED");
      state = manualPumpOn;
    } else if(espButtonInput == '0') {
      state = pumpOff;
      Serial.println("RELEASED");
    }
  }
  delay(1);
}

void waterPumpSystem(int digital_water_pump_port, int analog_moisture_sensor_number, pump &state, float &moist_level_percent) {

  // Calculating Moisture Level
  float moist_level_res = analogRead(analog_moisture_sensor_number);
  moist_level_percent = map(moist_level_res, 180, 515, 100, 0);
  

  // Running Water Pump Logic
  if(state != manualPumpOn) {
    float least_amount_water = 40;
    float most_amount_water = 69.99;
    if (moist_level_percent < least_amount_water) {
      state = pumpOn;
    } else if (state == pumpOn && moist_level_percent > least_amount_water && moist_level_percent < most_amount_water) {
      state = pumpOn;
    } else {
      state = pumpOff; 
    };
  }
  
}

void pumpOnOrOff(int digital_water_pump_port, pump &state) {
  if (state == pumpOn || state == manualPumpOn) {
    digitalWrite(digital_water_pump_port, HIGH);
  } else {
    digitalWrite(digital_water_pump_port, LOW);
  };
}
