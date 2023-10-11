#include <Servo.h>
Servo horizontalServo;
Servo verticalServo;

//DataPins
int ldrPins[] = {A0,A1,A2,A3};       //Lage en liste med datapins for LDR 
int readLDR(int pin){                //Funksjon som leser av datapins for LDR
    return analogRead(pin);
}
void attachServo(){                  //Funksjon som for datapins for ServoMotor
    horizontalServo.attach(9);
    verticalServo.attach(10);
}

//InitialVariabler for ServoMotor 
int horizontalPos = 90;             //InitialPosisjon Horisontal
int verticalPos = 90;               //InitialPosisjon Vertikal
int movementSpeed = 1;              //Bevegelseshastighet

//Error configurering 
const float emaAlpha = 0.2;         //Føre til en smooth bevegelse, kan endres for å øke/minke overgangen i bevegelsen
int deadZone = 20;                  //Døsone for filtrere ut unødvendige bevegeler

//Bevegelses variabler for ServoMotor og LDR 
const int numReadings = 5;
int ldrReadings[4][numReadings];    //Lagrer avlesningen fra hver LDR
int ldrIndex[4] = {0};              //Indeksen lagrer avlesningen til LDRene 
int ldrTotal[4] = {0};              //Lagrer lest totalVerdien til hvert LDR
int ldrAverage[4] = {0};            //Lagrer gjennomsnittet av hvert LDR

//Funksjonsliste 
void initializeReadingsArrays() {   //Funksjonene sett lesingen ldrReading for hvert LDR til null. Vi gjør dette for å resete programmet når koden blir kalt på
  for (int i = 0; i < 4; i++) {     
    for (int j = 0; j < numReadings; j++) {
      ldrReadings[i][j] = 0;
    }
  }
}

void updateEMA(int index, int ldrValue) {   //Funksjonen gir en jevnere gjennomsnittsverdi fra LDRs sensorene for å redusere utnødvendig støy 
  ldrAverage[index] = (1 - emaAlpha) * ldrAverage[index] + emaAlpha * ldrValue; 
}

bool isOutsideDeadZone(int index, int ldrValue) {   //Funksjonen sjekker om nåværende leseverdi og gjennomsnittsverdien for en bestemt LDRsensor er større en dødsonen. 
  return abs(ldrValue - ldrAverage[index]) > deadZone;
}

void adjustServoPositions(int index, int ldrValue) {    //Funksjonen justere i henhol til gjennomsnittet av LDR-verdien fra sensoren til horisontale og vertikale servomotoren.
  if (index == 0 || index == 2) {  // Adjust horizontal servo
    horizontalPos = map(ldrAverage[index], 0, 1023, 0, 180);
    horizontalPos = constrain(horizontalPos, 0, 180);
    horizontalServo.write(horizontalPos);
  }

  if (index == 1 || index == 3) {  // Adjust vertical servo
    verticalPos = map(ldrAverage[index], 0, 1023, 0, 180);
    verticalPos = constrain(verticalPos, 0, 180);
    verticalServo.write(verticalPos);
  }
}

void setup() {
  attachServo();
  initializeReadingsArrays();
}

void loop() {       //Leser av kontinuerlig LDR-sensoren, oppdaterer EMA og justerer posisjonene til servomotoren basert på disse verdiene 
  int ldrValues[4];
  for (int i = 0; i < 4; i++) {
    ldrValues[i] = readLDR(ldrPins[i]);
    updateEMA(i, ldrValues[i]);

    if (isOutsideDeadZone(i, ldrValues[i])) {
      adjustServoPositions(i, ldrValues[i]);
    }
  }

  // Add a small delay
  delay(20);
}