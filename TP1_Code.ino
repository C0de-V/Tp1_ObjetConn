#include <Arduino.h>

#undef D0
#undef D1
#undef D2
#undef D3
#undef D4
#undef D5
#undef D6
#undef D7
#undef D8
#undef RX
#undef TX

enum Pin : uint8_t {
  D0 = 16,
  D1 =  5,
  D2 =  4,
  D3 =  0,
  D4 =  2,
  D5 = 14,
  D6 = 12,
  D7 = 13,
  D8 = 15,
  RX =  3,
  TX =  1
};

int pinBouton = D1;
int pinBuzzer = D2;
int pinDS     = D5;
int pinSTCP   = D6;
int pinSHCP   = D7;

int score = 0;
int prises = 0;

byte etatRegistre1 = 0b00000000;
byte etatRegistre2 = 0b00000000;


void actualiserRegistres() {
  digitalWrite(pinSTCP, LOW);
  shiftOut(pinDS, pinSHCP, MSBFIRST, etatRegistre2);
  shiftOut(pinDS, pinSHCP, MSBFIRST, etatRegistre1);
  digitalWrite(pinSTCP, HIGH);
}

void eteindreDELs() {
  etatRegistre1 = 0b00000000;
  etatRegistre2 = 0b00000000;
  actualiserRegistres();
}

void afficherPuissance(int niveau) {
  if (niveau == 0) etatRegistre1 = 0b00000000;
  if (niveau == 1) etatRegistre1 = 0b10000000;
  if (niveau == 2) etatRegistre1 = 0b11000000;
  if (niveau == 3) etatRegistre1 = 0b11100000;
  if (niveau == 4) etatRegistre1 = 0b11110000;
  if (niveau == 5) etatRegistre1 = 0b11111000;
  actualiserRegistres();
}

void afficherCible(int cible) {
  if (cible == 0) etatRegistre1 = 0b00000000;
  if (cible == 1) etatRegistre1 = 0b00000100;
  if (cible == 2) etatRegistre1 = 0b00000010;
  if (cible == 3) etatRegistre1 = 0b00000001;
  actualiserRegistres();
}

void actualiserScoreEtPrises() {
  etatRegistre2 = 0b00000000;
  if (prises >= 1) etatRegistre2 = etatRegistre2 + 0b10000000;
  if (score >= 1)  etatRegistre2 = etatRegistre2 + 0b00010000;
  if (score >= 2)  etatRegistre2 = etatRegistre2 + 0b00001000;
  if (score >= 3)  etatRegistre2 = etatRegistre2 + 0b00000100;
  actualiserRegistres();
}


void jouerNote(int frequence, int duree) {
  tone(pinBuzzer, frequence);
  delay(duree);
  noTone(pinBuzzer);
  delay(25);
}

void sonChargement(int niveau) {
  jouerNote(300 + (niveau * 150), 60);
}

void sonVolTir() {
  for (int f = 800; f > 300; f -= 50) {
    jouerNote(f, 15);
  }
}

void sonSucces() {
  jouerNote(784, 100);
  jouerNote(1046, 250);
}

void sonEchec() {
  jouerNote(300, 180);
  jouerNote(220, 250);
}

void sonVictoire() {
  jouerNote(523, 150);
  jouerNote(659, 150);
  jouerNote(784, 150);
  jouerNote(1046, 350);
}

void sonDefaite() {
  jouerNote(400, 200);
  jouerNote(350, 200);
  jouerNote(300, 200);
  jouerNote(250, 400);
}

void animationDebut() {
  for (int i = 0; i < 2; i++) {
    etatRegistre1 = 0b11111111;
    etatRegistre2 = 0b00000000;
    actualiserRegistres();
    jouerNote(523, 180);

    etatRegistre1 = 0b00000000;
    etatRegistre2 = 0b10011100;
    actualiserRegistres();
    jouerNote(784, 180);
  }

  eteindreDELs();
  delay(1000);
}

void clignoterNouveauScore() {
  for (int i = 0; i < 3; i++) {
    score--;
    actualiserScoreEtPrises();
    delay(150);
    score++;
    actualiserScoreEtPrises();
    delay(150);
  }
}

void clignoterPrise() {
  for (int i = 0; i < 3; i++) {
    etatRegistre2 = etatRegistre2 - 0b10000000;
    actualiserRegistres();
    delay(150);
    etatRegistre2 = etatRegistre2 + 0b10000000;
    actualiserRegistres();
    delay(150);
  }
}

void partieGagnee() {
  eteindreDELs();

  for (int i = 0; i < 3; i++) {
    etatRegistre2 = 0b00010000;
    actualiserRegistres();
    delay(120);
    etatRegistre2 = 0b00001000;
    actualiserRegistres();
    delay(120);
    etatRegistre2 = 0b00000100;
    actualiserRegistres();
    delay(120);
  }

  sonVictoire();
  eteindreDELs();
  delay(2000);

  score = 0;
  prises = 0;
  animationDebut();
}

void partiePerdue() {
  eteindreDELs();

  for (int i = 0; i < 4; i++) {
    etatRegistre2 = 0b10000000;
    actualiserRegistres();
    delay(200);
    etatRegistre2 = 0b00000000;
    actualiserRegistres();
    delay(200);
  }

  sonDefaite();
  eteindreDELs();
  delay(2000);

  score = 0;
  prises = 0;
  animationDebut();
}

void setup() {
  pinMode(pinBouton, INPUT);
  pinMode(pinDS, OUTPUT);
  pinMode(pinSTCP, OUTPUT);
  pinMode(pinSHCP, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);

  randomSeed(analogRead(A0));

  eteindreDELs();
  animationDebut();
}

void loop() {
  int cible = random(1, 4);
  int forceRequise = cible + 1;

  afficherCible(cible);
  delay(1000);
  afficherCible(0);

  while (digitalRead(pinBouton) == LOW) {
    delay(10);
  }


  while (digitalRead(pinBouton) == HIGH) {
    delay(10);
  }

  int puissance = 0;
  while (digitalRead(pinBouton) == LOW && puissance < 5) {
    puissance++;
    afficherPuissance(puissance);
    sonChargement(puissance);
    delay(300);
  }

  sonVolTir();
  afficherPuissance(0);


  if (puissance >= 2 && puissance <= 4) {
    afficherCible(puissance - 1);
  }

  if (puissance == forceRequise) {
    sonSucces();
    score++;
    actualiserScoreEtPrises();
    clignoterNouveauScore();
  } else {
    sonEchec();
    prises++;
    actualiserScoreEtPrises();
    clignoterPrise();
  }

  afficherCible(0);

  while (digitalRead(pinBouton) == LOW) {
    delay(10);
  }


  if (score == 3) {
    partieGagnee();
  }

  if (prises == 2) {
    partiePerdue();
  }

  delay(1000);
}