#include <EEPROM.h>



const byte int_array_size = 14;
int int_array[int_array_size] = {0};

//Mapping
//HOME
#define HOME_MAXSPEED     int_array[0]
//RUNNING
#define MAX_SPEED_X      int_array[1]
#define MAX_ACCELERATION_X       int_array[2]
#define MAX_SPEED_Y      int_array[3]
#define MAX_ACCELERATION_Y       int_array[4]
#define MAX_POSITION_LX      int_array[5]
#define MAX_POSITION_LY      int_array[6]
#define MAX_POSITION_RX      int_array[7]
#define MAX_POSITION_RY      int_array[8]
#define INVERT_DIR_LX      int_array[9]
#define INVERT_DIR_LY      int_array[10]
#define INVERT_DIR_RX      int_array[11]
#define INVERT_DIR_RY      int_array[12]

void Load_To_Variables() {
  for (int stepperNumber = 0; stepperNumber < numOfStepper; stepperNumber++) {
    
    //HOME -> USE DIRECTLY
  home_speed = int_array[0];
    
    if (stepperNumber == 0 || stepperNumber == 2) {
      maxSpeed[stepperNumber] = int_array[1];
      maxAccel[stepperNumber] = int_array[2];
    } else {
      maxSpeed[stepperNumber] = int_array[3];
      maxAccel[stepperNumber] = int_array[4];
    }

    maxPos[stepperNumber] = int_array[5 + stepperNumber];
    
    //inverseDir[stepperNumber]= int_array[10 + stepperNumber];
    if (int_array[10 + stepperNumber] == 0) {
      inverseDir[stepperNumber] = 1;
    } else {
      inverseDir[stepperNumber] = -1;
    }
  }

}

void Load_Flash()
{
  //HOME
  HOME_MAXSPEED =  EEPROMReadInt(0);
  //RUNNING
  MAX_SPEED_X =  EEPROMReadInt(1);
  MAX_ACCELERATION_X  =  EEPROMReadInt(2);
  MAX_SPEED_Y =  EEPROMReadInt(3);
  MAX_ACCELERATION_Y  =  EEPROMReadInt(4);
  MAX_POSITION_LX =  EEPROMReadInt(5);
  MAX_POSITION_LY =  EEPROMReadInt(6);
  MAX_POSITION_RX =  EEPROMReadInt(7);
  MAX_POSITION_RY =  EEPROMReadInt(8);
  INVERT_DIR_LX =  EEPROMReadInt(9);
  INVERT_DIR_LY =  EEPROMReadInt(10);
  INVERT_DIR_RX =  EEPROMReadInt(11);
  INVERT_DIR_RY =  EEPROMReadInt(12);

}
void Write_Flash()
{
  //HOME
  EEPROMWriteInt(0, HOME_MAXSPEED); //
  //RUNNING
  EEPROMWriteInt(1, MAX_SPEED_X); //
  EEPROMWriteInt(2, MAX_ACCELERATION_X ); //
  EEPROMWriteInt(3, MAX_SPEED_Y); //
  EEPROMWriteInt(4, MAX_ACCELERATION_Y ); //
  EEPROMWriteInt(5, MAX_POSITION_LX); //
  EEPROMWriteInt(6, MAX_POSITION_LY); //
  EEPROMWriteInt(7, MAX_POSITION_RX); //
  EEPROMWriteInt(8, MAX_POSITION_RY); //
  EEPROMWriteInt(9, INVERT_DIR_LX); //
  EEPROMWriteInt(10, INVERT_DIR_LY); //
  EEPROMWriteInt(11, INVERT_DIR_RX); //
  EEPROMWriteInt(12, INVERT_DIR_RY); //

}


// ============ WRITE/READ Method ===========
// ============ ============ ================
// ============ ============ ================

#define Offset_long 4

byte low1Byte = 0;
byte low2Byte = 0;
byte low3Byte = 0;
byte low4Byte = 0;

void EEPROMWriteInt(unsigned int p_address, int p_value)
{
  unsigned int pc_address = p_address * 4 + Offset_long;
  low1Byte = ((p_value >> 0) & 0xFF);
  low2Byte = ((p_value >> 8) & 0xFF);
  low3Byte = ((p_value >> 16) & 0xFF);
  low4Byte = ((p_value >> 24) & 0xFF);
  // Serial.print("Write Flash: ");
  //Serial.print(pc_address, DEC);
  //Serial.print(": ");
  //Serial.println(p_value, DEC);
  EEPROM.write(pc_address, low1Byte);
  EEPROM.write(pc_address + 1, low2Byte);
  EEPROM.write(pc_address + 2, low3Byte);
  EEPROM.write(pc_address + 3, low4Byte);
}
//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
int EEPROMReadInt(unsigned int p_address)
{
  unsigned int pc_address = p_address * 4 + Offset_long;
  low1Byte = EEPROM.read(pc_address);
  low2Byte = EEPROM.read(pc_address + 1);
  low3Byte = EEPROM.read(pc_address + 2);
  low4Byte = EEPROM.read(pc_address + 3);
  //Serial.println("Read Flash:");
  //Serial.println(low1Byte, HEX);
  //Serial.println(low2Byte, HEX);
  // Serial.println(low3Byte, HEX);
  //Serial.println(low4Byte, HEX);
  //  Serial.println(low3Byte, HEX);
  //  Serial.println(highByte, HEX);
  return ((low1Byte << 0) & 0x000000FF) + ((low2Byte << 8) & 0x0000FF00) + ((low3Byte << 16) & 0x00FF0000) + ((low4Byte << 24) & 0xFF000000);
}


