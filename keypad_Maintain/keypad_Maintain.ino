//#include <Key.h>
#include <Keypad.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal.h>

//keypad initialize
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {22, 23, 24, 25}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {26, 27, 28, 29}; //connect to the column pinouts of the keypad
//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//Lcd Display Initialize
const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
 
String s;//Password Variable
String default_pass = "123";

//servo
Servo myservo;
bool takepas_reset = true; //Password Take Flag

int pos;//Position of Servo
String first_line = "pass: ";//First line of LCD
String default_second_line = " (Press B) to image capture ||  (Press C)to Change  password || (Press *) to submit  ";
String second_line = "Change  pass: (press C)    ";//Second line of LCD
String submit_pass = "(Press *) to submit    ";
bool rotate = false;//Servo Rotation Flag
bool ch_pass = false;//Change Password Flag
String temp = "";//Previous Password Variable
String temp2 = "";//New Password Variable
bool new_pass = false;//New Password Flag


#define slave_address 0x04
int sendNumber = 0; // Send Variable in Pi(Master)

///// SETUP ////
void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);
  s = "";

  myservo.attach(6);//Servo pin initialize
  first_line = "pass: ";
  rotate = false;
  takepas_reset = true;
  ch_pass = false;
  temp = "";
  second_line = default_second_line;
  submit_pass = "presss (*) to submit    ";
  first_line = "pass: ";
  temp2 = "";
  new_pass = false;

  myservo.write(0);// Set servo to initial position
  
  //I2C Communication Initialize
  Wire.begin(slave_address);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(sendData);


}

//////LOOP ////////////
void loop() {

  char customKey = customKeypad.getKey();//Read Data From Keypad
  //Checking 
  if (!ch_pass) 
  {
    if (customKey >= '0' && customKey <= '9' || customKey == 'D')
    {

      Change(customKey);
      customKey = '.';//Reset the keypad read variable

    }
    else if (customKey == 'A')setup();//Setting up Variables of Main Menu
    else if (customKey == '*'  )welc();//Submiting the password to open the gate 
    else if (customKey == 'B')//Sending request to master(PI) to take image
    {
      sendNumber = 123;
      first_line = "IMAGE CAPTURING";
      second_line = " PLEASE COME IN FRONT OF CAMERA ";
    }
    else if ( customKey == 'C')//Change Password Option
    {
      setup();
      ch_pass = true;//Change password flag set to TRUE
      temp = "";//
      takepas_reset = false;
      new_pass = false;
    }

  }
  if (ch_pass && new_pass == false) //Will take previous password if true
  {
    if (customKey >= '0' && customKey <= '9' || customKey == 'D')
    {

      Change_temp(customKey);
      customKey = '.';
    }
    else if (customKey == 'A')setup();
    else if (customKey == '*'  ) {
      new_pass = true;
      temp2 = "";
      // Serial.println("printed first");
      customKey = '.';
    }
    else if ( customKey == 'C') {
      setup();
      ch_pass = true;
      temp = "";
      takepas_reset = false;
      new_pass = false;
    }

  }



  if (new_pass == true) //Will take new password to change default password
  {
    if (customKey >= '0' && customKey <= '9' || customKey == 'D')
    {

      Change_temp2(customKey);
      customKey = '.';
    }
    else if (customKey == 'A')setup();
    else if (customKey == '*'  ) {
      //Serial.println("not need to be printed");
      welc_reset();//Will check previous pass & set new pass 
    }
    else if ( customKey == 'C') {
      setup();
      ch_pass = true;
      temp = "";
      takepas_reset = false;
      new_pass = false;
    }

  }

  if (takepas_reset == true)takepas_reset_f();//Printing taked password & options
  if (ch_pass == true) second_line_f();//Printing taked prev password & options
  if (rotate == true) {//opening and closing the gate by rotating servo motor if true
    // Serial.println(pos);


    for (; pos <= 90; pos += 5)
    { // goes from 0 degrees to 90 degrees in steps of 5 degree
      myservo.write(pos);// tell servo to go to position in variable 'pos'
      s = default_pass;//Setting the default password to s for varification
      welc();//Setting the strings to print Successful
      takepas_reset_f();//Printing the strings

      //delay(15);// waits 15ms for the servo to reach the position
    }
    pos = 90;
    for (int i = 0; i < 25; i++)takepas_reset_f();//Delay gate before close

    for (pos = 90; pos >= 0; pos -= 5)
    {
      // goes from 90 degrees to 0 degrees in steps of 5 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      s = default_pass;// tell servo to go to position in variable 'pos'
      welc();

      takepas_reset_f();

      //delay(15);                       // waits 15ms for the servo to reach the position
    }
    pos = 0;

    rotate = false;
    setup();

  }
}

void Change( char key) //Changing taken password
{
  int len = s.length();
  if (key == 'D' && len != 0)   s.remove(len - 1, 1);
    //Backspacing password
  else if (key == 'D' && len == 0)return;
    //If lenth of given password is 0 then can not Backspace password
  else  s += key;
    //Adding new char to password / taking password 
}

void Change_temp( char key) {//Changing taken prev password

  int len = temp.length();
  if (key == 'D' && len != 0)   temp.remove(len - 1, 1);
  else if (key == 'D' && len == 0)return;
  else  temp += key;

}
void Change_temp2(char key) {//Changing taken new password

  int len = temp2.length();
  if (key == 'D' && len != 0)   temp2.remove(len - 1, 1);
  else if (key == 'D' && len == 0)return;
  else  temp2 += key;
}

void welc() {
  if (s == default_pass) {

    first_line = " <<SUCCESSSFUL>>  ";
    rotate = true;
    s = "";
    second_line = "                       " ;
    //delay(299);

  }
  else {
    first_line = " <<WRONG PASS>> ";
    s = "";
    second_line =  "(Press A) to main menu  ";
  }


}

void welc_reset() {
  //Changing Password
  new_pass = false;
  if (default_pass == temp) {
    default_pass = temp2;
    s = "";
    first_line = "  <<CHANGED>>  ";
    ch_pass = false;
    second_line = "(Press A) to main menu)  ";
    temp = "";
    temp2 = "";
    takepas_reset = true;

  }
  else {
    first_line = "<<NOT CHANGED>>";
    ch_pass = false;

    second_line = "(press A to main menu)  ";
    temp = "";
    temp2 = "";
    takepas_reset = true;

  }

}
///Rad.....
void scrollInFromLeft (int line, String str1) {

  // Written by R. Jordan Kreindler June 2016

  int i = 10 - str1.length();

  // line = line - 1;

  for (int j = i; j <= i + 16; j++) {

    for (int k = 0; k <= 15; k++) {

      lcd.print(" "); // Clear line

    }

    lcd.setCursor(j, line);

    lcd.print(str1);

    delay(300);

  }

}

///Rad//////


void takepas_reset_f() {
  // Serial.println(s);
  // Serial.println(second_line);

  lcd.print(first_line);
  lcd.print(s);

  lcd.setCursor(0, 1);
  second_line += second_line[0];
  second_line.remove(0, 1);
  String _T = "";
  for (int i = 0; i < 20; i++) {
    _T += second_line[i];
  }
  lcd.print(_T);

  lcd.display();
  delay(200);
  lcd.clear();


}

void second_line_f() {

  if (new_pass == false) {
    lcd.print("prev: ");
    lcd.print(temp);
  }
  if (new_pass == true) {
    lcd.print("new: ");
    lcd.print(temp2);
  }

  lcd.setCursor(0, 1);
  submit_pass += submit_pass[0];
  submit_pass.remove(0, 1);
  lcd.print(submit_pass);

  lcd.display();
  delay(200);
  lcd.clear();



}
/*
  void receiveEvent(int howMany) {
  lcd.clear();

  int numOfBytes = Wire.available();
  //display number of bytes and cmd received, as bytes

  byte b = Wire.read();  //cmd


  for (int i = 0; i < numOfBytes - 1; i++) {
    char data = Wire.read();
   // Serial.println(data);
    if (data == '1')rotate = true;
    //lcd.print(data);
  }

  }*/

/*void sendData() {

        Wire.write("1");
        Serial.println("1");
        delay(1000);


  }*/


// callback for received data

void receiveEvent(int byteCount) {
  int val = 0;
  while (Wire.available()) {
    val = Wire.read();
    // Serial.print("data received: ");
    // Serial.println(val);

    if (val == 1) {
      lcd.clear();
      rotate = true;

    }
    else {
      //   Serial.println("zero found" + val);
      rotate = false;
      first_line = " <<NOT RECOGNIZED>> ";
      second_line = default_second_line;
      s = "";

    }
  }
}

// callback for sending data
void sendData() {
  Wire.write(sendNumber);
  sendNumber = 0;
}
