#include <Arduino.h>

boolean Zustand = false; // geschlossen
int T1 = 10;             //für geschlossen
int T1b = 9;             // für beschleinigung auf
int T2 = 11;             //für offen
int T2b = 8;             // für Beschleunigung zu
int Laufweg = 400;       // schrittlänge

int LEDA = 4; // Statusanzeige auf
int LEDZ = 3; // Statusanzeige zu

const int stepPin = 6; //PUL -Pulse
const int dirPin = 7;  //DIR -Direction
const int enPin = 8;   //ENA -Enable

void setup()
{
  pinMode(T1, INPUT);
  pinMode(T2, INPUT);
  pinMode(T1b, INPUT);
  pinMode(T2b, OUTPUT);
  pinMode(LEDA, OUTPUT);
  pinMode(LEDZ, OUTPUT);
  Serial.begin(9600);
  Serial.println("Programm wurde gestartet");
}

void loop()
{
  if (Zustand == false)
  {
    delay(300);
    if (digitalRead(T1b) == HIGH)
    {

      digitalWrite(dirPin, LOW); //Changes the rotations direction
      for (int x = 0; x < Laufweg; x++)
      { // Makes 200 pulses for making one full cycle rotation
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(500);
      }

      if (digitalRead(T2) == HIGH)
      {
        delay(100);
        Zustand = !Zustand;
      }
    }
  }
  if (Zustand == true)
  {
    delay(300);
    if (digitalRead(T2b) == HIGH)
    {

      digitalWrite(dirPin, HIGH); //Changes the rotations direction
      for (int x = 0; x < Laufweg; x++)
      { // Makes 200 pulses for making one full cycle rotation
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(500);
      }

      if (digitalRead(T1) == HIGH)
        ;
      {
        delay(100);
        Zustand = !Zustand;
      }
    }
  }
}