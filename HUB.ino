#include <nRF24L01.h>
#include <RF24.h>
#include <Keypad_I2C.h>
#include <Keypad.h> // GDY120705
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define I2CADDR 0x20


const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] =
    {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {3, 2, 1, 0}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 6, 5, 4}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad_I2C keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS, I2CADDR);
LiquidCrystal_I2C lcd(0x27, 20, 4);
RF24 radio(9, 10); // CE, CSN

const byte addresses[][6] = {"00001", "00002"}; //Setting the two addresses. One for transmitting and one for receiving
char key;
bool state;
int nextState = 0;
int listeningState = 0;
float soilSensor;
float tempSensor;

void startMenu()
{
  lcd.setCursor(2, 1); lcd.print("Watering Project");
  lcd.setCursor(0, 3); lcd.print("Version 1.0");
  delay(1000);
  lcd.clear();
}

float getData (int state)
{
  radio.stopListening(); //This sets the module as transmitter
  switch (state)
  {
  case 1: // Soil Sensor 
    listeningState = state;
    radio.write(&listeningState, sizeof(listeningState)); //Sending the data
    radio.startListening();                               //This sets the module as receiver
    if (radio.available())
      radio.read(&soilSensor, sizeof(soilSensor)); //Reading the data
    return soilSensor;
    break;
  case 2: // Temperature Sensor
    listeningState = state;
    Serial.print(listeningState);
    radio.write(&listeningState, sizeof(listeningState)); //Sending the data
    radio.startListening();                               //This sets the module as receiver
    if (radio.available())
      radio.read(&tempSensor, sizeof(tempSensor)); //Reading the data
    return tempSensor;
    break;
  // default:
  //   break;
  }
}

void initialMenu()
{
  lcd.clear();
  lcd.setCursor(2, 0); lcd.print("Choose A Option");
  lcd.setCursor(0, 1); lcd.print("A)    Soil Level");
  lcd.setCursor(0, 2); lcd.print("B)   Temperature"); 
  lcd.setCursor(0, 3); lcd.print("C)     Settings");
}

void soilLevelMenu()
{
  lcd.clear();
  lcd.setCursor(6, 0); lcd.print("Soil Level");
  lcd.setCursor(0, 1); lcd.print("Current Soil Level :");
  lcd.setCursor(0, 3); lcd.print("*) Go Back");
  while (key != '*')
  {
    soilSensor = getData(1); // Get Soil 
    lcd.setCursor(14, 2); lcd.print(soilSensor);
    key = keypad.getKey();
    delay(100);
  }
  state = false;
  listeningState = 0; 
  radio.stopListening();
  radio.write(&listeningState, sizeof(listeningState));
}

void temperatureMenu()
{
  lcd.clear();
  lcd.setCursor(1, 0); lcd.print("Temperature Level");
  lcd.setCursor(0, 1); lcd.print("Current Temp Level :");
  lcd.setCursor(0, 3); lcd.print("*) Go Back");
  while (key != '*')
  {
    tempSensor = getData(2); // Get Temperature
    lcd.setCursor(14, 2); lcd.print(tempSensor);
    key = keypad.getKey();
    delay(100);
  }
  state = false;
  listeningState = 0;
  radio.stopListening();
  radio.write(&listeningState, sizeof(listeningState));
}

void settingMenu1()
{
  lcd.clear();
  lcd.setCursor(4, 0); lcd.print("Setting Page");
  lcd.setCursor(0, 1); lcd.print("1) Change Temp Units");
  lcd.setCursor(0, 2); lcd.print("2) Enable AutoScroll");
  lcd.setCursor(13, 3); lcd.print("#) Next");
}

void optionMenu(int x)
{
  nextState = 0;
  state = true; 
  switch (x)
  {
    case 1: 
      soilLevelMenu();
      break;
    case 2:
      temperatureMenu();
      break;
    case 3:
      settingMenu1();
      settingSetUp();
      break;
    default:
      state = false;
      break;
  }
  while (state)
  {
    key = keypad.getKey();
    switch (key)
    {
      case '*':
        state = false;
        break;
    }
  }
  menu();
}

void settingSetUp()
{
  state = true; 
  nextState = 0; 
  while(state)
  {
    key = keypad.getKey();
    switch (key)
    {
      case '1':
        lcd.clear(); lcd.print("Temp Units");
        delay(1000);
        nextState = 3;
        state = false;
        break;
      case '2':
        lcd.clear(); lcd.print("AutoScroll");
        delay(1000);
        nextState = 3;
        state = false;
        break;
      case '*':
        state = false;
        nextState = 4; 
        break;
    }
  }
  optionMenu(nextState);
}

void menu()
{
  state = true;
  nextState = 0;

  initialMenu();
  while (state)
  {
    key = keypad.getKey();
    switch (key) 
    {
      case 'A':
        state = false;
        nextState = 1;
        break;
      case 'B':
        nextState = 2;
        state = false;
        break;
      case 'C': 
        nextState = 3; 
        state = false;
        break;
    }
  }
  optionMenu(nextState);
}

void setup()
{
  lcd.init();
  lcd.backlight();
  Wire.begin();   // GDY200622
  keypad.begin(); // GDY120705
  Serial.begin(9600);
  radio.begin();                          //Starting the radio communication
  radio.openWritingPipe(addresses[1]);    //Setting the address at which we will send the data
  radio.openReadingPipe(1, addresses[0]); //Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_MIN);
}

void loop()
{
  menu();
}