/*  Serial Decode

     Read Format

     20-20-400-200

     Write

    'C' - Serial Communication Checking - print online if true
    'H' - Home trigger
    'S' - Save EEPROM
    'L' - load EEPROM
    'E' - EMERGENCY Start 'R' to stop
    'R' - EMERGENCY Stop
    'Q' - Reset Arduino
    //EMERGENCY Start r
*/

String inString = "";    // string to hold input char
String inString_buffer = "";    // string to hold input Serial msg
String inString_display_buffer = "";    // string to hold Display info

bool exclude_print_val = true;

int current_index = 0;//index of current decoding number
//long input_value[Input_size];  //inputs value(s) buffer -> moved to main tab
boolean update_flag = false; //Flag for end of success input string follow /n


void serial_decode()   // Read serial input: //TODO add serial1
{
  byte Read_Flag = 0;
  while (Serial.available() > 0)
  {
    int inChar = Serial.read();
    char_decode(inChar) ;
  }
  while (Serial1.available() > 0)
  {
    int inChar = Serial1.read();
    char_decode(inChar) ;
  }
  /*
    while (Serial1.available() > 0)
    {
      int inChar = Serial1.read();
      char_decode(inChar) ;
    }
  */
}


void char_decode(int inChar)
{
  // Read_Flag=0;
  inString_buffer += (char)inChar;
  if (isDigit(inChar))       //if get a number
  {
    // convert the incoming byte to a char
    // and add it to the string:
    inString += (char)inChar;
    //Serial.println(inString);
  }
  else if (inChar == '-')       // if you get a comma, convert to a number,
  {
    if (current_index < Input_size) // within size
    {
      input_value[current_index] = inString.toInt(); //

      //  pdInput_value[current_index] = inString.toInt(); //
      current_index++;  //increase index
    }




    //            else// giveup the value to avoide over access array
    //            {
    //              Serial.println("overflow");
    //            }
    inString = "";   // clear the string buffer for new input:


  }
  else if (inChar == '\n' || inChar == 'N')       //if you get a newline, convert to a number and print all value
  {
    if (current_index < Input_size) // within size
    {
      input_value[current_index] = inString.toInt(); //final value

    }
    //            else// giveup the value to avoide over access array
    //            {
    //              Serial.println("overflow(end)");
    //            }


    inString = "";   // clear the string buffer for new input:
    current_index = 0;// reset the index
    //Set update_Flag
    update_flag = true;

  }
  else if (inChar == 'C') //Check if online
  {
    //DO STH HERE
    inString = "";   // clear the string buffer for new input:
    Serial.print("on");
    Serial1.print("on");

    digitalWrite(BrakeLy, HIGH);
    digitalWrite(BrakeRy, HIGH);

    digitalWrite(DI1_SERVO_ON_ly, HIGH);
    digitalWrite(DI1_SERVO_ON_ry, HIGH);

    update_flag = true;
    exclude_print_val = true;
  }
  else if (inChar == 'V') //go offline, brake on
  {
    //DO STH HERE
    inString = "";   // clear the string buffer for new input:
    Serial.print("off");
    Serial1.print("off");

    digitalWrite(BrakeLy, LOW);
    digitalWrite(BrakeRy, LOW);

    /*
        digitalWrite(DI1_SERVO_ON_ly, LOW);
        digitalWrite(DI1_SERVO_ON_ry, LOW);
    */

    digitalWrite(DI1_SERVO_ON_ly, HIGH);
    digitalWrite(DI1_SERVO_ON_ry, HIGH);

    update_flag = true;
    exclude_print_val = true;
  }
  else if (inChar == 'H') //HOME
  {
    GO_HOME = true;

    Serial.print("hm");
    Serial1.print("hm");
    inString = "";   // clear the string buffer for new input:
    for (int i = 0; i < numOfStepper; i++)
    {
      homeDone[i]  = false;
    }
    exclude_print_val = true;
  }
  else if (inChar == 'G') //HOME
  {
    GO_HOME = true;

    Serial.print("mhm");
    Serial1.print("mhm");
    inString = "";   // clear the string buffer for new input:
    for (int i = 0; i < numOfStepper; i++)
    {
      //homeDone[numOfStepper]  = false;
      homeDone[i] = input_value[i];
      Serial.print(homeDone[i]);
      Serial.print("-");
    }
    exclude_print_val = true;
  }
  else if (inChar == 'S') //SAVE EEPROM
  {

    inString = "";   // clear the string buffer for new input:
        #ifdef ISMEGA
    inString_display_buffer = F("Write Config");
    
    for (int i = 0; i < int_array_size; i++)
    {
      int_array[i] = input_value[i];

      Serial.print("\t");
      Serial.print(i);
      Serial.print(":");
      Serial.print(int_array[i]);

      Serial1.print("\t");
      Serial1.print(i);
      Serial1.print(":");
      Serial1.print(int_array[i]);
    }

    

    
    Write_Flash();
    Load_To_Variables();
    Serial.print("sa-");
    Serial1.print("sa-");
    for (int i = 0; i < int_array_size; i++) {

      Serial.print(int_array[i]);
      Serial1.print(int_array[i]);
      if (i != int_array_size - 1) {
        Serial.print("-");
        Serial1.print("-");
      }
    }
    Serial.print("\t");
    Serial1.print("\t");
        #else
    #endif
    exclude_print_val = true;

  }
  else if (inChar == 'L') //LOAD EEPROM
  {
    //DO STH HERE
    inString = "";   // clear the string buffer for new input:
#ifdef ISMEGA
    Load_Flash();

    Serial.print("lo");
    Serial1.print("lo");
    for (int i = 0; i < int_array_size; i++) {

      Serial.print(int_array[i]);
      Serial1.print(int_array[i]);
      if (i != int_array_size - 1) {
        Serial.print("-");
        Serial1.print("-");
      }
    }
    Serial.print("\t");
    Serial1.print("\t");
#else
#endif
    exclude_print_val = true;

  }
  else if (inChar == 'E') //EMERGENCY Start r
  {
    //DO STH HERE
    inString = "";   // clear the string buffer for new input:

    Serial.print("EMGY STP - R");
    Serial1.print("EMGY STP - R");
    isEmergencyStop = true;
    Serial.print("\t");
    Serial1.print("\t");

    exclude_print_val = true;

  }

  else if (inChar == 'R') //EMERGENCY STOP
  {
    //DO STH HERE
    inString = "";   // clear the string buffer for new input:

    Serial.print("EMGY OFF");
    Serial1.print("EMGY OFF");
    isEmergencyStop = false;
    Serial.print("\t");
    Serial1.print("\t");

    exclude_print_val = true;

  }

  else if (inChar == 'Q') //Soft Reset
  {
    //DO STH HERE
    inString = "";   // clear the string buffer for new input:

    Serial.print("re");
    Serial1.print("re");

    // soft_restart();

    exclude_print_val = true;

  }

}


void check_update()   //Check update flag and write value when string finish
{
  if ( update_flag == true) //update value
  {

    if (exclude_print_val == false) {
      //print all value
      for (int index = 0; index < Input_size; index++)
      {
        //Serial.print(index);
        //Serial.print(":");
        Serial.print(input_value[index]);
        Serial.print("-");

        Serial1.print(input_value[index]);
        Serial1.print("-");
      }

    }
    //Finish Serial.print, Next Line
    Serial.println("|");
    Serial1.println("|");

    exclude_print_val = false;
    update_flag = false; //clear flag
    inString_display_buffer = inString_buffer;    // string to hold input Serial msg
    inString_buffer = "";   // clear the string buffer for new input:
  }


}

