#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <Tone.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include "my_EEPROM.h" 

byte skull[8] = {
  B01110,
  B01110,
  B01110,
  B01110,
  B01110,
  B01110,
  B11111,
  B11011
};

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7);
Tone speakerpin;
int starttune[] = { NOTE_E2, NOTE_E2, NOTE_C2, NOTE_E2, NOTE_G2, NOTE_G2 };
int duration2[] = { 100, 200, 100, 200, 100, 400 };
int note[] = { NOTE_A2, NOTE_A2, NOTE_A2, NOTE_A2, NOTE_B2, NOTE_A2 };
int duration[] = { 100, 100, 100, 300, 100, 300 };
int button[] = { 15, 16, 17, 18 };    // The four button input pins
int ledpin[] = { 8, 9, 10, 11 };  // LED pins
int turn = 0;                     // turn counter
int buttonstate = 0;              // button state checker
int randomArray[50];              // long to store up to 50 inputs
int inputArray[50];
int highscore = 0;
int highscoreEE;
int MyScore;
int newHighscore;
volatile bool isSleep = true;
int a0 = 14;

uint16_t scan_EEPROM(void)
{
 int i, data_location, number_data_found;
 uint8_t d;
 number_data_found = 0;
 Serial.println("Performing EEPROM scanning...");
 for(i=0;i<1024;i++)
 {
 d = EEPROM_read1byte(i);
 if (d != 0xFF)
 {
 data_location = i;
 number_data_found++;
 Serial.print(d);
 Serial.print(" is found in EEPROM location[");
 Serial.print(i);
 Serial.println("]");
 }
 }
 if (number_data_found > 1)
 Serial.println("More than one data are found in EEPROM");
 else if(number_data_found ==0)
 Serial.println("no data found in EEPROM");
} 

int find_first_location_of_data(void)
{
  uint8_t d;
  int i;
  uint16_t loc;
  for (i=0;i<1024;i++)
  {
    d = EEPROM_read1byte(i);
    if(d != 0xFF)
    {
      return i;
    }
  }
  return -1;
}

void edit_EEPROM(uint8_t mode){
  int addr; // สำหรับเก็บว่าตำแหน่งใดที่ไม่ใช่ 0xFF
  uint8_t d;
  addr = find_first_location_of_data(); // เช็คตำแหน่งที่ไม่ใช่ 0xFF
  EEPROM_Erase_only(addr); // ลบค่าจากตำแหน่งที่ตรวจเจอ
  if(addr == 0){ // ถ้า addr เป็น 0 จะอัพเดทตำแหน่งต่อไปที่ 1023
    EEPROM_Write_to_Empty_location(1023, mode); // เขียน mode ที่ตำแหน่งที่ 1023
  }else{
    EEPROM_Write_to_Empty_location(addr-1, mode); // เขียน mode ที่ตำแหน่งที่ <addr-1>
  }
}

void print2digit(int val){
  if(val < 10 && val >= 0)
     lcd.print(0);
  lcd.print(val);
}

void setup() {  ///////////////LCD/////////////////
  Serial.begin(9600);
  display_all_data_in_EEPROM();
  scan_EEPROM(); 
  
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(LOW);
  lcd.createChar(0, skull);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Your Score: 0");
  lcd.setCursor(0, 0);
  //edit_EEPROM(highscore);
  highscore = EEPROM_read1byte(find_first_location_of_data()); 
  highscoreEE = EEPROM_read1byte(find_first_location_of_data()); 
  
  lcd.print("High Score:");
  print2digit(highscore);
  
  speakerpin.begin(12);  // speaker is on pin 12
  pinMode(a0, INPUT);
  digitalWrite(a0, HIGH);
  for (int x = 0; x < 4; x++)  // LED pins are outputs
  {
    pinMode(ledpin[x], OUTPUT);
  }

  for (int x = 0; x < 4; x++) {
    pinMode(button[x], INPUT);      // button pins are inputs
    digitalWrite(button[x], HIGH);  // enable internal pullup; buttons start in high position; logic reversed
  }

  randomSeed(analogRead(0));  //Added to generate "more randomness" with the randomArray for the output function
  for (int thisNote = 0; thisNote < 6; thisNote++) {
    // play the next note:
    speakerpin.play(starttune[thisNote]);
    // hold the note:
    if (thisNote == 0 || thisNote == 2) {
      digitalWrite(ledpin[0], HIGH);
    }
    if (thisNote == 1 || thisNote == 3) {
      digitalWrite(ledpin[1], HIGH);
    }
    if (thisNote == 4 || thisNote == 5) {
      digitalWrite(ledpin[2], HIGH);
    }
    if (thisNote == 6) {
      digitalWrite(ledpin[3], HIGH);
    }
    delay(duration2[thisNote]);
    // stop for the next note:
    speakerpin.stop();
    digitalWrite(ledpin[0], LOW);
    digitalWrite(ledpin[1], LOW);
    digitalWrite(ledpin[2], LOW);
    digitalWrite(ledpin[3], LOW);
    delay(25);
  }
  delay(1000);
  
}



void loop()
{ 
  
  for (int y = 0; y <= 2; y++)
  {
    // function for generating the array to be matched by the player
    digitalWrite(ledpin[0], HIGH);
    digitalWrite(ledpin[1], HIGH);
    digitalWrite(ledpin[2], HIGH);
    digitalWrite(ledpin[3], HIGH);

    for (int thisNote = 0; thisNote < 6; thisNote++)
    {
      // play the next note:
      speakerpin.play(note[thisNote]);
      // hold the note:
      delay(duration[thisNote]);
      // stop for the next note:
      speakerpin.stop();
      delay(25);
    }

    digitalWrite(ledpin[0], LOW);
    digitalWrite(ledpin[1], LOW);
    digitalWrite(ledpin[2], LOW);
    digitalWrite(ledpin[3], LOW);
    delay(500);

    for (int y = turn; y <= turn; y++) // untuk sekali random
    {                                  // Limited by the turn variable
      Serial.println("");              // Some serial output to follow along
      Serial.print("Turn: ");
      Serial.print(y);
      Serial.println("");
      lcd.clear();
      MyScore = y;
      lcd.setCursor(0, 1); //(Column,Row)
      lcd.print("Your Score: ");
      lcd.setCursor(12, 1);
      lcd.print(MyScore);
      if (MyScore > highscore)
      {
        highscore = y;
        newHighscore = y;
      }
      lcd.setCursor(0, 0);
      lcd.print("High Score: " + (String)(highscore));
      Serial.print(y);
      Serial.println("");
      delay(1000);
      randomArray[y] = random(1, 5); // Assigning a random number (1-4) to the randomArray[y], y being the turn count
      for (int x = 0; x <= turn; x++)
      {
        Serial.print(randomArray[x]);

        for (int y = 0; y < 4; y++)
        {

          if (randomArray[x] == 1 && ledpin[y] == 8) 
          { // if statements to display the stored values in the array
            digitalWrite(ledpin[y], HIGH);
            speakerpin.play(NOTE_G3, 100);
            delay(400);
            digitalWrite(ledpin[y], LOW);
            delay(100);
          }

          if (randomArray[x] == 2 && ledpin[y] == 9)
          {
            digitalWrite(ledpin[y], HIGH);
            speakerpin.play(NOTE_A3, 100);
            delay(400);
            digitalWrite(ledpin[y], LOW);
            delay(100);
          }

          if (randomArray[x] == 3 && ledpin[y] == 10)
          {
            digitalWrite(ledpin[y], HIGH);
            speakerpin.play(NOTE_B3, 100);
            delay(400);
            digitalWrite(ledpin[y], LOW);
            delay(100);
          }

          if (randomArray[x] == 4 && ledpin[y] == 11)
          {
            digitalWrite(ledpin[y], HIGH);
            speakerpin.play(NOTE_C4, 100);
            delay(400);
            digitalWrite(ledpin[y], LOW);
            delay(100);
          }
        }
      }
    }
    input();
  }
}

void input()
{ // Function for allowing user input and checking input against the generated array

  for (int x = 0; x <= turn;)
  { // Statement controlled by turn count

    for (int y = 0; y < 5; y++)
    {

      buttonstate = digitalRead(button[y]);

      if (buttonstate == LOW && button[y] == 15)
      { // Checking for button push
        digitalWrite(ledpin[0], HIGH);
        speakerpin.play(NOTE_G3, 100);
        delay(200);
        digitalWrite(ledpin[0], LOW);
        inputArray[x] = 1;
        delay(250);
        Serial.print(" ");
        Serial.print(1);
        if (inputArray[x] != randomArray[x])
        {         // Checks value input by user and checks it against
          fail(); // the value in the same spot on the generated array
        }         // The fail function is called if it does not match
        x++;
      }
      if (buttonstate == LOW && button[y] == 16)
      {
        digitalWrite(ledpin[1], HIGH);
        speakerpin.play(NOTE_A3, 100);
        delay(200);
        digitalWrite(ledpin[1], LOW);
        inputArray[x] = 2;
        delay(250);
        Serial.print(" ");
        Serial.print(2);
        if (inputArray[x] != randomArray[x])
        {
          fail();
        }
        x++;
      }

      if (buttonstate == LOW && button[y] == 17)
      {
        digitalWrite(ledpin[2], HIGH);
        speakerpin.play(NOTE_B3, 100);
        delay(200);
        digitalWrite(ledpin[2], LOW);
        inputArray[x] = 3;
        delay(250);
        Serial.print(" ");
        Serial.print(3);
        if (inputArray[x] != randomArray[x])
        {
          fail();
        }
        x++;
      }

      if (buttonstate == LOW && button[y] == 18)
      {
        digitalWrite(ledpin[3], HIGH);
        speakerpin.play(NOTE_C4, 100);
        delay(200);
        digitalWrite(ledpin[3], LOW);
        inputArray[x] = 4;
        delay(250);
        Serial.print(" ");
        Serial.print(4);
        if (inputArray[x] != randomArray[x])
        {
          fail();
        }
        x++;
      }
      
    }
  }
  delay(500);
  turn++; // Increments the turn count, also the last action before starting the output function over again
}

void fail()
{ // Function used if the player fails to match the sequence

  for (int y = 0; y <= 3; y++)
  { // Flashes lights for failure

    digitalWrite(ledpin[0], HIGH);
    digitalWrite(ledpin[1], HIGH);
    digitalWrite(ledpin[2], HIGH);
    digitalWrite(ledpin[3], HIGH);
    speakerpin.play(NOTE_G2, 300);
    delay(200);
    digitalWrite(ledpin[0], LOW);
    digitalWrite(ledpin[1], LOW);
    digitalWrite(ledpin[2], LOW);
    digitalWrite(ledpin[3], LOW);
    speakerpin.play(NOTE_C2, 300);
    delay(200);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Game Over ::>_<::");
    //lcd.setCursor(1, 1);
    lcd.write(byte(0));
  }
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  if(newHighscore > highscoreEE)
  {
    edit_EEPROM(newHighscore);
  }
  
  lcd.print("High: " + (String)(highscore));
  // Serial.print(y);
  // Serial.println("");
  lcd.setCursor(9, 0);
  lcd.print("You: ");
  lcd.setCursor(14, 0);
  lcd.print(MyScore);
  // if(y > highscore) {
  // highscore = y;
  // }
  lcd.setCursor(0, 1);
  lcd.print("<-Press to Restart");
  

  ///////////////////////////////////////////
  {
    // buttonstate = digitalRead(button[y]);
    while (digitalRead(a0) > 0)
    {
      digitalWrite(ledpin[0], HIGH);
      digitalWrite(ledpin[1], HIGH);
      digitalWrite(ledpin[2], HIGH);
      digitalWrite(ledpin[3], HIGH);
      delay(100);
      digitalWrite(ledpin[0], LOW);
      digitalWrite(ledpin[1], LOW);
      digitalWrite(ledpin[2], LOW);
      digitalWrite(ledpin[3], LOW);
      delay(100);
    }
    delay(1000);
    
    turn = -1; // Resets turn value so the game starts over without need for a reset button
  }
}