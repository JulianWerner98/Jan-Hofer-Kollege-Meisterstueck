#include <Arduino.h>
#define STD_TIMER 640      //Schnellste Geschwindigkeit
#define REDUCED_TIMER 4000 //Langsamste Geschwindigkeit
#define VERLAMSARMUNG 1    //Wie schnell bremsen. Hohe Zahl = schnell bremsen
#define STEP_PIN 4
#define DIR_PIN 7    //DIR -Direction
#define ENABLE_PIN 8 //ENA-Enable
#define LED_NANO 13  //LED des Nano's
#define TASTER_ANSCHLAG_ZU 9
#define TASTER_ANSCHLAG_AUF 10
#define TASTER_BESCHLEUNIGUNG_AUF 2 //MUSS 2 ODER 3 SEIN
#define TASTER_BESCHLEUNIGUNG_ZU 3  //MUSS 2 ODER 3 SEIN
#define TASTER_NOTAUS 12
#define DIR_SCHLIESSEN 0  //Ändern wenn der Motor in die falsche Richtung fährt
#define MOTOR_DREHT false //Wenn Enable den Zustand hat ist er aktiv am fahren

void beschAufInterrupt();
void beschZuInterrupt();

boolean faehrtZu = false;
boolean faehrtAuf = false;
boolean slowMode = false;
long count = 0;

void setup()
{
  pinMode(TASTER_ANSCHLAG_AUF, INPUT_PULLUP);
  pinMode(TASTER_ANSCHLAG_ZU, INPUT_PULLUP);
  pinMode(TASTER_BESCHLEUNIGUNG_AUF, INPUT_PULLUP);
  pinMode(TASTER_BESCHLEUNIGUNG_ZU, INPUT_PULLUP);
  pinMode(TASTER_NOTAUS, INPUT_PULLUP);

  pinMode(LED_NANO, OUTPUT);

  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);

  Serial.begin(9600);
  Serial.println("Programm wurde gestartet");

  //Setzte Timer 1 Einstellungen
  cli(); //Lösche globales Interrupt-Enable-Bit
  //CTC-Mode aktivieren
  TCCR1A = 0;                               //Löschen des TCCR1A-Registers
  TCCR1B = 0;                               //Löschen des TCCR1B-Registers
  TCCR1B |= (1 << WGM12);                   //Setze CTC-Mode (Waveform Generation Mode)
  TCCR1B |= /*(1 << CS11) | */ (1 << CS10); //Setze CS10 (Clock Select), Vorteiler 1
  TCNT1 = 0;                                //Timer Counter Register löschen
  OCR1A = STD_TIMER;                        //Vergleichswert 1249->200Hz, 12499-> 20Hz
  TIMSK1 |= (1 << OCIE1A);                  //Bit Output Compare A Match Interrupt Enable setzen
  sei();                                    //Setze globales Interrupt-Enable-Bit

  //Setze Interrupt Einstellungen für Pin 2 & 3
  attachInterrupt(digitalPinToInterrupt(TASTER_BESCHLEUNIGUNG_AUF), beschAufInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(TASTER_BESCHLEUNIGUNG_ZU), beschZuInterrupt, FALLING);

  digitalWrite(ENABLE_PIN, 0);
  digitalWrite(DIR_PIN, 0);
  digitalWrite(STEP_PIN, 0);
}

void loop()
{
}

void beschAufInterrupt()
{
  if (faehrtAuf)
  {
    slowMode = true;
    Serial.println("Reduziere Geschwindigkeit");
  }
  else
  {
    faehrtZu = true;
    faehrtAuf = false;
    Serial.println("Starte schliessen");
  }
}
void beschZuInterrupt()
{
  if (faehrtZu)
  {
    slowMode = true;
    Serial.println("Reduziere Geschwindigkeit");
  }
  else
  {
    faehrtZu = false;
    faehrtAuf = true;
    Serial.println("Starte oeffnen");
  }
}
ISR(TIMER1_COMPA_vect)
{
  //Status LED, Kann einfach auskommentiert werden
  if (!(count++ % (1249900 / OCR1A)))
  {
    digitalWrite(LED_NANO, !digitalRead(LED_NANO));
  }

  if (digitalRead(TASTER_NOTAUS))
  {
    //Anschlag berührt
    if (!digitalRead(TASTER_ANSCHLAG_AUF) || !digitalRead(TASTER_ANSCHLAG_ZU))
    {
      if (digitalRead(ENABLE_PIN) == MOTOR_DREHT)
        Serial.println("Stoppe Motor");
      digitalWrite(ENABLE_PIN, !MOTOR_DREHT);
      faehrtAuf = false;
      faehrtZu = false;
      slowMode = false;
    }
    else
    {
      if (faehrtAuf)
      {
        digitalWrite(DIR_PIN, !DIR_SCHLIESSEN);
        digitalWrite(ENABLE_PIN, MOTOR_DREHT);
        digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
      }
      else if (faehrtZu)
      {
        digitalWrite(ENABLE_PIN, MOTOR_DREHT);
        digitalWrite(STEP_PIN, !digitalRead(STEP_PIN));
        digitalWrite(DIR_PIN, DIR_SCHLIESSEN);
      }
      if (slowMode)
      {
        if (OCR1A + VERLAMSARMUNG < REDUCED_TIMER)
        {
          OCR1A += VERLAMSARMUNG;
        }
        else
        {
          if (OCR1A != REDUCED_TIMER)
          {
            Serial.println("Minial Geschwindigkeit");
            OCR1A = REDUCED_TIMER;
          }
        }
      }
      else
      {
        OCR1A = STD_TIMER;
      }
    }
  }
}