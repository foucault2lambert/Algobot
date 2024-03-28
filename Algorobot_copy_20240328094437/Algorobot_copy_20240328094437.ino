#include <Wire.h>
#include <Ultrasonic.h>
#include <ESP32Servo.h>

#define CapteurDeVitesse1Pin 4
#define CapteurDeVitesse2Pin 5
#define TRIGGER_PIN_FRONT 12  // Capteur frontal
#define ECHO_PIN_FRONT 14
#define TRIGGER_PIN_LEFT 13  // Capteur à gauche
#define ECHO_PIN_LEFT 15
#define TRIGGER_PIN_RIGHT 16  // Capteur à droite
#define ECHO_PIN_RIGHT 17
#define DISTANCE_THRESHOLD 100

// coo en tps reel
int x = 0;
// coo pt de depart
int xinit = 0;
int yinit = 0;
volatile int speed1 = 0;
volatile int speed2 = 0;
double angle1 = 0;
double angle2 = 0;
double radius1 = 0.3;
double radius2 = 0.5;
int vitesse = 0;
bool wall = false;

Servo moteurGauche;
Servo moteurDroite;

Ultrasonic ultrasonicFront(TRIGGER_PIN_FRONT, ECHO_PIN_FRONT);
Ultrasonic ultrasonicLeft(TRIGGER_PIN_LEFT, ECHO_PIN_LEFT);
Ultrasonic ultrasonicRight(TRIGGER_PIN_RIGHT, ECHO_PIN_RIGHT);

bool readUltrasonicDistance(Ultrasonic ultrasonic) {
  long distance;
  distance = ultrasonic.read();
  if (distance < DISTANCE_THRESHOLD) {
    return true;
  } else {
    return false;
  }
}

void allerToutDroit() {
  moteurGauche.write(180);  // Fait tourner le moteur gauche à pleine vitesse
  moteurDroite.write(0);    // Fait tourner le moteur droit à pleine vitesse
}

void allerAGauche() {
  moteurGauche.write(90);  // Arrête le moteur gauche
  moteurDroite.write(0);   // Fait tourner le moteur droit à pleine vitesse
}

void allerADroite() {
  moteurGauche.write(180);  // Fait tourner le moteur gauche à pleine vitesse
  moteurDroite.write(90);   // Arrête le moteur droit
}

void demiTour() {
  moteurGauche.write(180);   // Fait tourner le moteur gauche à pleine vitesse
  moteurDroite.write(-180);  // Arrête le moteur droit
}


void setup() {
  Serial.begin(9600);

  moteurGauche.attach(4);  // Attache le moteur gauche au pin 4
  moteurDroite.attach(5);  // Attache le moteur droit au pin 5

  pinMode(TRIGGER_PIN_FRONT, OUTPUT);
  pinMode(ECHO_PIN_FRONT, INPUT);

  pinMode(TRIGGER_PIN_LEFT, OUTPUT);
  pinMode(ECHO_PIN_LEFT, INPUT);

  pinMode(TRIGGER_PIN_RIGHT, OUTPUT);
  pinMode(ECHO_PIN_RIGHT, INPUT);
}


void loop() {
  angle1 += speed1 * 0.1;
  angle2 += speed2 * 0.1;
  double x1 = radius1 * cos(angle1);
  double y1 = radius1 * sin(angle1);
  double x2 = radius2 * cos(angle2);
  double y2 = radius2 * sin(angle2);
  double speedReel = (speed1 + speed2) / 2;
  double x = round(x1);
  double y = round(y2);
  int vitesse = round(speedReel);
  speed1 = 0;
  speed2 = 0;
  delay(100);
  bool capteurUltrasonAvant = readUltrasonicDistance(ultrasonicFront);
  bool capteurUltrasonGauche = readUltrasonicDistance(ultrasonicLeft);
  bool capteurUltrasonDroit = readUltrasonicDistance(ultrasonicRight);

  while (capteurUltrasonAvant == false) {  // initialisation
    allerToutDroit();
  }

  while (y != yinit && x != xinit) {                                                                        // tour de piece
    if (capteurUltrasonGauche == true && capteurUltrasonDroit == false && capteurUltrasonAvant == false) {  // si juste un mur a gauche aller tt droit
      allerToutDroit();
    }
    if (capteurUltrasonGauche == true && capteurUltrasonDroit == false && capteurUltrasonAvant == true) {  // mur devant et a gauche = tournedroite
      allerADroite();
    }

    if (capteurUltrasonGauche == false && capteurUltrasonAvant == false) {  // ni mur devant ni gauche = agauche
      allerAGauche();
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  while (y != yinit && x != xinit) {  // fct balayage de piece
    bool capteurUltrasonAvant = readUltrasonicDistance(ultrasonicFront);
    bool capteurUltrasonGauche = readUltrasonicDistance(ultrasonicLeft);
    bool capteurUltrasonDroit = readUltrasonicDistance(ultrasonicRight);

    if (capteurUltrasonGauche == false && capteurUltrasonDroit == false && capteurUltrasonAvant == false) {
      allerToutDroit();
    } else {
      if (capteurUltrasonGauche == true) {
        allerADroite();
      } else if (capteurUltrasonDroit == true) {
        allerAGauche();
      } else {
        demiTour();
      }
    }
  }
}