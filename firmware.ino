//    Distributed under MIT License.
//    Author: Dr. Ing. Diego D. Santiago
//    Institution: INAUT - UNSJ - CONICET
//    Email: dsantiago@inaut.unsj.edu.ar


#include "serialframedecoder.h"
#include <EEPROM.h>

// Defines pins numbers
const int _enaPin = 8;
const int ms1Pin  = 9;
const int ms2Pin  = 10;
const int ms3Pin  = 11;
const int stepPin = 12;
const int dirPin  = 13;


//addres of variables in EEPROM
const int     addr_rot_dir        = 0;
const int     addr_step_res       = 1;
const int     addr_freq           = 2; //4 bytes
const int     addr_pulse_duration = 4; //4 bytes

uint8_t       rot_dir             = 0; //rotation direction (left/rigth)
uint8_t       step_res            = 0; //step resolution (0=1; 1=1/2; 2=1/4; 3=1/8; 4=1/16)
unsigned         freq             = 0;      //pulse signal frequency
unsigned long pulse_duration      = 0; //pulse signal duration

void setup() {
  Serial.begin(115200);
  // Sets the two pins as Outputs
  pinMode(_enaPin, OUTPUT);
  pinMode(ms1Pin, OUTPUT);
  pinMode(ms2Pin, OUTPUT);
  pinMode(ms3Pin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  digitalWrite(_enaPin, LOW);
  digitalWrite(ms1Pin, LOW);
  digitalWrite(ms2Pin, LOW);
  digitalWrite(ms3Pin, LOW);
  digitalWrite(dirPin, LOW); //Enables the motor to move in a particular direction


  EEPROM.put(addr_rot_dir, rot_dir);
  EEPROM.put(addr_step_res, step_res);
  EEPROM.put(addr_freq, freq);
  EEPROM.put(addr_pulse_duration, pulse_duration);
  EEPROM.get(addr_rot_dir, rot_dir);
  EEPROM.get(addr_step_res, step_res);
  EEPROM.get(addr_freq, freq);
  EEPROM.get(addr_pulse_duration, pulse_duration);
  digitalWrite(dirPin, bool(rot_dir) ); //setup direction
  if (freq > 0) {
    setMSpins(step_res);
    //tone(stepPin, calcFreq(step_res, vel) );
    if (pulse_duration > 0)
      tone(stepPin, freq );
    else
      tone(stepPin, freq, pulse_duration );
  } else {
    noTone(stepPin);
  }
}

SerialFrameDecoder readFrame;
void serialEvent() {
  while (Serial.available()) {
    readFrame.appendFrameByte(Serial.read());
  }
}

byte          id                  = 1;
Instruction   inst                = Write;
byte          param[BUFFERSIZE]   = {0};
uint16_t      param_length        = 0;
byte16        addr;//address were data is stored.
int           datasize = 0; //siz of data too be stored


void loop() {
  if (readFrame.newData()) {
    if (readFrame.getInstructionPacket(id, inst, param, param_length) == No_Error) {
      if (inst == Write) {
        readFrame.clean();
        addr.word[0]  = param[0];
        addr.word[1]  = param[1];
        datasize = param_length - 2; //remove address
        if (datasize < BUFFERSIZE) {
          for (int i = 0; i < datasize; i++) {
            EEPROM.write(addr.value + i, param[i+2]);
          }
        }
      }
    }
    //update new values
    EEPROM.get(addr_rot_dir, rot_dir);
    EEPROM.get(addr_step_res, step_res);
    EEPROM.get(addr_freq, freq);
    EEPROM.get(addr_pulse_duration, pulse_duration);
    
//    Serial.print(" rot_dir=");
//    Serial.print(rot_dir);
//    Serial.print(" step_res=");
//    Serial.print(step_res);
//    Serial.print(" freq=");
//    Serial.print(freq);
//    Serial.print("\n pulse_duration=");
//    Serial.print(pulse_duration);

    digitalWrite(dirPin, bool(rot_dir) ); //setup direction
    setMSpins(step_res);
    if (freq > 0) {
      //tone(stepPin, calcFreq(step_res, vel) );
      if (pulse_duration > 0)
        tone(stepPin, freq, pulse_duration );
      else
        tone(stepPin, freq );
    } else {
      noTone(stepPin);
    }
  }
}

void setMSpins(uint8_t step_res) {
  switch (step_res) {
    case 0:
      digitalWrite(ms1Pin, LOW);
      digitalWrite(ms2Pin, LOW);
      digitalWrite(ms3Pin, LOW);
      break;
    case 1:
      digitalWrite(ms1Pin, HIGH);
      digitalWrite(ms2Pin, LOW);
      digitalWrite(ms3Pin, LOW);
      break;
    case 2:
      digitalWrite(ms1Pin, LOW);
      digitalWrite(ms2Pin, HIGH);
      digitalWrite(ms3Pin, LOW);
      break;
    case 3:
      digitalWrite(ms1Pin, HIGH);
      digitalWrite(ms2Pin, HIGH);
      digitalWrite(ms3Pin, LOW);
      break;
    case 4:
      digitalWrite(ms1Pin, HIGH);
      digitalWrite(ms2Pin, HIGH);
      digitalWrite(ms3Pin, HIGH);
      break;
  }
}

unsigned calcFreq(uint8_t step_res, float vel) {
  switch (step_res) {
    case 0:
      return unsigned(vel * 200.0);
    case 1:
      return unsigned(vel * 400.0);
    case 2:
      return unsigned(vel * 800.0);
    case 3:
      return unsigned(vel * 1600.0);
    case 4:
      return unsigned(vel * 3200.0);
  }
}


int speedUp() {
  int customDelay = analogRead(A0); // Reads the potentiometer
  int newCustom = map(customDelay, 0, 1023, 1, 500); // Convrests the read values of the potentiometer from 0 to 1023 into desireded delay values (300 to 4000)
  return newCustom;
}
