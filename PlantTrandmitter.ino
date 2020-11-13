#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10);               // CE, CSN
const byte addresses[][6] = {"00001", "00002"}; //Setting the two addresses. One for transmitting and one for receiving
int listeningState = 0; 

int soilSensorStat(float x)
{
  if (x >= 0 && x < 30)
    return 0;
  if (x >= 30 && x < 70)
    return 1;
  if (x >= 70)
    return 2;
}

void sendData(int state)
{
  if(state == 0)
  {
    Serial.print(state);
    Serial.print("\n");
    radio.startListening(); //This sets the module as receiver
    delay(5);
  }
  if (state == 1)
  {
    Serial.print(state);
    Serial.print("\n");
    float soilSensor = analogRead(A0) / 10;
    radio.stopListening();                        //This sets the module as transmitter
    radio.write(&soilSensor, sizeof(soilSensor)); //Sending the data
    delay(5);
  }
  if (state == 2)
  {
    Serial.print(state);
    Serial.print("\n");
    float tempSensor = analogRead(A1);
    radio.stopListening();
    radio.write(&tempSensor, sizeof(tempSensor)); //Sending the data
    delay(5);
  }
}

void setup()
{
  Serial.begin(9600);        //This sets the module as transmitter
  radio.begin();                          //Starting the radio communication
  radio.openWritingPipe(addresses[0]);    //Setting the address at which we will send the data
  radio.openReadingPipe(1, addresses[1]); //Setting the address at which we will receive the data
  radio.setPALevel(RF24_PA_MIN);
}

void loop()
{
  delay(5);
  radio.startListening(); //This sets the module as receiver
  if (radio.available())  //Looking for incoming data
  {
    radio.read(&listeningState, sizeof(listeningState));
    delay(5);
    sendData(listeningState);
  }
  else 
  {
    delay(8000);
  }

  // int status = soilSensorStat(soilSensor);
  // switch (status)
  // {
  //   char *text; 
  //   case 0:
  //     text = "Dry";
  //     radio.write(&text, sizeof(text)); 
  //     break; 
  //   case 1:
  //     text = "Good";
  //     radio.write(&text, sizeof(text));
  //     break;
  //   case 2: 
  //     text = "Bad"; 
  //     radio.write(&text, sizeof(text));
  //     break;
  // }
}
