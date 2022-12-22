#include <Servo.h>
#include <Wire.h>



#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels



int servoPin2 = 6;
int servoPin1 = 7;
int ledika = 8;
int gumbek = 10;
bool s0 = false; //stanje za izpis na zaslon

int sensorValueBright = 0; //senzor svetlobe
int sensorValueMoist = 0; //merilnik vlage
int sensorValueRain = 0; //merilnik dezja

int servo_state1 = 0;
int servo_state2 = 0;
int led_state = 0;
int stanje = 0;

Servo Servo1; //servo motor za dotok vode
Servo Servo2; //servo motor za streho

//spremenljivke za izpis in pomnenje časa
unsigned long zacetni_cas;
unsigned long trenutni_cas;
unsigned long konec_dezja;
unsigned long po_dezju;
unsigned long cas_zalivanja;

void setup()
{
  pinMode(ledika, OUTPUT);
  pinMode(gumbek, INPUT_PULLUP);

  pinMode(A0, INPUT); //senzor vlage
  pinMode(A1, INPUT); //senzor svetlobe
  pinMode(A2, INPUT); //senzor dezja

  Servo1.attach(servoPin1);
  Servo2.attach(servoPin2);

  zacetni_cas = millis();
  konec_dezja = 0;

  Servo1.write(0); //zacetni polozaj servo1
  Servo2.write(0); //zacetni polozaj servo2

  Serial.begin(115200);
}

void loop()
{
  zacetni_cas = millis();
  trenutni_cas = millis();
  sensorValueMoist = analogRead(A0); //branje vlage
  //Serial.print(sensorValueMoist);
  //Serial.print(" ");
  sensorValueBright = analogRead(A1); //branje svetlobe
  //Serial.print(sensorValueBright);
  //Serial.print(" ");
  sensorValueRain = analogRead(A2); //branje dezja
  //Serial.print(sensorValueRain);

  s0 = digitalRead(gumbek);

  switch (stanje) {
    //Stanje 0: Izpisuje vlago v procentih
    case 0: {
        
        if (s0 == false) { //če je tipka aktivna, spremeni stanje
          stanje++;

        }
      } break;

    case 1: {

        if (s0 == true) {  //če je tipka neaktivna, spremeni stanje
          stanje++;

        }
      } break;
    // Stanje 2: Izpisuje čas, ki je pretekel od zadnjega zalivanja
    case 2:
      {
       

        if (s0 == false)
        {
          //če je tipka aktivna, spremeni stanje
          stanje++;
        }
      } break;

    case 3: {
        if (s0 == true) {  //če je tipka neaktivna, spremeni stanje
          stanje = 0;
        }
      } break;

  }

  if (sensorValueMoist > 900) //ko zazna suho zemljo
  {

    Servo1.write(35); //sproži se servo motor za zalivanje
    servo_state1 = 1;
  }
  else
  {
    Servo1.write(0);
  };

  if (sensorValueMoist <= 900 && servo_state1) //če je servo motor aktiven in zazna zadosti vlage
  {
    cas_zalivanja = trenutni_cas; //zapomni si trenutek
    Servo1.write(-35); //servo motor zapre dovod vode
    servo_state1 = 0;
  }
  //************************************************************


  if (sensorValueRain < 700) //zapiranje strehe, ko dezuje
  {
    Servo2.write(120); //servo motor zapre streho
    servo_state2 = 1;

  }

  if (sensorValueRain > 700 && servo_state2 == true) //če preneha deževati
  {
    konec_dezja = zacetni_cas; //zapomni si ta trenutek
    servo_state2 = 0;
  }

  if (sensorValueRain > 700 && (zacetni_cas - konec_dezja >= 5000)) //če je minilo 5s od zadnje kapljice in je streha zaprta
  {
    Servo2.write(-120); //servo motor odpre streho
    servo_state2 = 0;
  }


  //*************************************************************

  if (sensorValueBright < 500) //če zazna premajhno intenziteto vpadne svetlobe
  {
    digitalWrite(ledika, HIGH); //led zasveti
    led_state = 1;
  }

  if (sensorValueBright > 500 && led_state) //če led sveti in zazna veliko intenziteto svetlobe
  {
    digitalWrite(ledika, LOW); //led ugasne
    led_state = 0;
  }
}



//FUNKCIJE

