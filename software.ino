//logiciel pour lumiere velo trotinette et LED64
#include <FastLED.h>
#define NUM_LEDS 64
#define DATA_PIN 5
//bouton entrees :
const int bopgauche =9;
const int bopdroit = 10;
const int bopfrein= 11;
const int bopbip=12;
const int journuit=A1;
int sensorjn=0; //valeur detection journuit
int valeur ;
// Define the array of leds
CRGB leds[64];

int currentLedAnimationG = 0;
int currentLedAnimationD = 0;
int ledFrameGauche = 0;
int ledFrameDroite = 0;

int clignGaucheEtat[9][8] = {
  {1,0,0,0,0,0,0,0},
  {2,1,0,0,0,0,0,0},
  {0,2,1,0,0,0,0,0},
  {0,0,2,1,0,0,0,0},
  {0,0,0,2,1,0,0,0},
  {0,0,0,0,2,1,0,0},
  {0,0,0,0,0,2,1,0},
  {0,0,0,0,0,0,2,1},
  {0,0,0,0,0,0,0,2}
};

int currentLed = 0;

void setup() {
  pinMode(bopgauche,INPUT_PULLUP);
  pinMode(bopdroit,INPUT_PULLUP);
  pinMode(bopfrein,INPUT_PULLUP);
  pinMode(bopbip,INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println(" depart test");
  FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, 64);
}

//********************************Fin SETUP***********

//*********************sous prog *******************

// Activation des freins true == OUI / false == NON
void activateBrakes(bool bFrein) {
  for (int led = 18; led < 57; led++) {
    if (
      (led >= 18 && led < 22) 
      || (led >= 26 && led < 30) 
      || (led >= 34 && led < 38)
      || (led >= 42 && led < 46)
    ) {
      // dois je activer les freins, OUI == CRGB(0,200,0) // NON = valeur par défaut de la led
      // si les lumieres sont allumées, elles restent allumées quand le frein est éteint
      leds[led] = bFrein == true ? CRGB(0,200,0) : leds[led];
    }
  }
}

// Definition des couleurs que doivent prendre les leds pendant l'animation
CRGB getCligColor(int value, CRGB defaultValue) {
  switch(value) {
    case 0 : 
      return defaultValue; break;
    case 1 : 
      return CRGB(40,250,0); break;
    case 2 : 
      return CRGB(20,125,0); break;   
    default:
      return defaultValue; break;
  }
}

void activateclignotantG(bool clig) {
  // clignotant relaché, on remet à 0 le comptage de l'animation et des frames
  Serial.println(ledFrameGauche);
  if (clig == false) {
    currentLedAnimationG = 0;
    ledFrameGauche = 0;
  } else {
    ledFrameGauche += 1;
    // toutes les 2 frames, on change d'animation (et on remet à 0 le comptage des frames)
    if (ledFrameGauche == 2) {
      currentLedAnimationG = currentLedAnimationG < 8 ? currentLedAnimationG + 1 : 0;
      ledFrameGauche = 0;
    }
  }
  // Soit le clignotant est actif et dans ce cas on prend la valeur de la led de l'animation courante,
  // sinon, on prend la valeur par défaut de la led (allumée ou éteinte)
  for (int led = 0; led < 8; led++) {
    leds[led] = clig == true ? getCligColor(clignGaucheEtat[currentLedAnimationG][led], leds[led]) : leds[led];
  }
}

void activateclignotantD(bool clig) {
  // clignotant relaché, on remet à 0 le comptage de l'animation et des frames
  Serial.println(ledFrameDroite);
  if (clig == false) {
    currentLedAnimationD = 0;
    ledFrameDroite = 0;
  } else {
    ledFrameDroite += 1;
    // toutes les 100 frames, on change d'animation (et on remet à 0 le comptage des frames)
    if (ledFrameDroite == 2) {
      currentLedAnimationD = currentLedAnimationD < 8 ? currentLedAnimationD + 1 : 0;
      ledFrameDroite = 0;
    }
  }
  // Soit le clignotant est actif et dans ce cas on prend la valeur de la led de l'animation courante,
  // sinon, on prend la valeur par défaut de la led (allumée ou éteinte)
  for (int led = 56; led < 64; led++) {
    leds[led] = clig == true ? getCligColor(clignGaucheEtat[currentLedAnimationD][abs(56-led)], leds[led]) : leds[led];
  }
}

void updateLeds(bool bFrein, bool cligGauche, bool cligDroite, bool bip) {
  //Placer ici suivant l'ordre d'importance (le plus bas est prioritaire sur le plus haut)
  // remise à 0 des leds
  for (int led = 0; led < 64; led++) {
    leds[led] = bip == true ? CRGB(50,50,50) : CRGB(0,0,0);
  }
  activateclignotantG(cligGauche);
  activateclignotantD(cligDroite);
  activateBrakes(bFrein);
}
  //************MAIN******
void loop() 
{
  int valeurFrein = digitalRead(bopfrein);
  int valeurClignotantGauche = digitalRead(bopgauche);
  int valeurClignotantDroit = digitalRead(bopdroit);
  int valeurBip = digitalRead(bopbip);

  // remise à 0 des leds
  for (int led = 0; led < 64; led++) {
    leds[led] = CRGB(0,0,0);
  }

  // calcul des couleurs dans la mémoire
  updateLeds(
    valeurFrein == 0 ? true : false,
    valeurClignotantGauche == 0 ? true : false,
    valeurClignotantDroit == 0 ? true : false,
    valeurBip == 0 ? true : false
  );
  
  // Affichage de la mémoire des leds dans le composant
  FastLED.show();
}

  
