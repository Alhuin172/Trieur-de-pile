#include "EEPROM.h"
#include <rgb_lcd.h>  //Bibliothèque de l'écran
rgb_lcd lcd;

float moyGlissante[4] = { 3, 3, 3, 3 };
float sommeMoyGlissante = 3 * 4;
//____________________________________________________________________________
int ledRouge = 4;
int ledVerte = 2;
int ledJaune = 3;

//____________________________________________________________________________
int compteurPilesSauvees = 0;
int compteurPilesTestees = 0;
int conditionCompteur = 0;
long alternanceAffichage = 0;

/*____________________________________________________________________________
_               fonction d'affichage qui prend pour parametre la tension de la pile
_               distinction des cas au regard de la tension de la pile
_               si tension appartient a [0,1.7] : affiche tension, allume led et conditionCompteur++
_               ainsi conditionCOmpteur est incrémentée plusieur fois pour le test d'une meme pile

*/
void affichage(float U) {
  lcd.clear();
  lcd.home();
  if (U > 1.7) {// uniquement s'il n'ya pas de piles
    if (alternanceAffichage % 2 == 1) {
      lcd.setRGB(0, 0, 0);
      lcd.print("Ce trieur piston ");
      lcd.setCursor(0, 1);
      lcd.print("a sauve ");
      lcd.print(compteurPilesSauvees);
      lcd.print(" piles");
      digitalWrite(ledVerte, LOW);
      digitalWrite(ledRouge, LOW);
      digitalWrite(ledJaune, LOW);
      conditionCompteur = true;

    } else {
      lcd.setRGB(0, 0, 0);
      lcd.print(int(compteurPilesSauvees * 100 / compteurPilesTestees));
      lcd.print("% des piles ");
      lcd.setCursor(0, 1);
      lcd.print("ont ete sauvees");
      digitalWrite(ledVerte, LOW);
      digitalWrite(ledRouge, LOW);
      digitalWrite(ledJaune, LOW);
      conditionCompteur = 0;
    }
  } else {
    lcd.print("Tension : ");
    if (U > 1.4) {
      lcd.print(U);
      lcd.print(" V");
      lcd.setCursor(0, 1);
      lcd.print("pile Neuve");
      lcd.setColor(2);
      digitalWrite(ledVerte, HIGH);
      digitalWrite(ledJaune, LOW);
      digitalWrite(ledRouge, LOW);
    }

    else if (U > 1.1) {
      lcd.print(U);
      lcd.print(" V");
      lcd.setCursor(0, 1);
      lcd.print("pile Bonne");
      lcd.setRGB(200, 200, 0);
      digitalWrite(ledJaune, HIGH);
      digitalWrite(ledRouge, LOW);
      digitalWrite(ledVerte, LOW);
    }

    else if (U < 1.1) {
      lcd.print(U);
      lcd.print(" V");
      lcd.setCursor(0, 1);
      lcd.print("pile à jeter");
      lcd.setColor(1);
      digitalWrite(ledRouge, HIGH);
      digitalWrite(ledVerte, LOW);
      digitalWrite(ledJaune, LOW);
    }

    conditionCompteur++;
    if (conditionCompteur == 171) {
      if (U > 1.1) {
        if (compteurPilesSauvees==3){creu();}
        compteurPilesSauvees++;

              EEPROM.put(172,compteurPilesSauvees);
      }
      compteurPilesTestees++;
      alternanceAffichage++;
            EEPROM.put(18,compteurPilesTestees);
    }
  }
}


//____________________________________________________________________________ mesure de tension
//variables de calibrage
float tensionReelleP1 = 1.49;   
float tensionReelleP2 = 0.8;    //tension de la pile2 (mesurée par multimètre)
int valP1 = 319;                //valeur affichee par Serial.println(analogRead(A3)); pour la pile1
int valP2 = 182;
float pente = (tensionReelleP1 - tensionReelleP2) / (valP1 - valP2);

float lectureTension() {
  moyGlissante[3] = moyGlissante[2];
  moyGlissante[2] = moyGlissante[1];
  moyGlissante[1] = moyGlissante[0];
  moyGlissante[0] = pente * (analogRead(A3) - valP1) + tensionReelleP1;
  sommeMoyGlissante = moyGlissante[0] + moyGlissante[1] + moyGlissante[2] + moyGlissante[3];

  return (sommeMoyGlissante / 4);
}
//____________________________________________________________________________

void creu(){
  lcd.clear();
  lcd.home();
  lcd.print("Alhuin 172 CL224");
  lcd.setCursor(0, 1);
  lcd.print("172 piles sauvees !!");
for (int i=0;i<100;i++){
    lcd.setRGB(200, 200, 0);
    digitalWrite(ledJaune, HIGH);
    digitalWrite(ledRouge, LOW);
    digitalWrite(ledVerte, LOW);
    delay(50);digitalWrite(ledJaune, LOW);delay(20);
    lcd.setRGB(255,0, 0);
    digitalWrite(ledJaune, LOW);
    digitalWrite(ledRouge, HIGH);
    digitalWrite(ledVerte, LOW);
    delay(50);digitalWrite(ledRouge, LOW);delay(20);
    lcd.setRGB(0, 255, 0);
    digitalWrite(ledJaune, LOW);
    digitalWrite(ledRouge, LOW);
    digitalWrite(ledVerte, HIGH);
    delay(50);digitalWrite(ledVerte, LOW);delay(20);
  }
}

void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);    //On initialise l'écran
  pinMode(2, OUTPUT);  //Les pins 2,3,4 sont les relais liés aux leds
  pinMode(3, OUTPUT);  //orange
  pinMode(4, OUTPUT);  //vert


  lcd.setRGB(0, 0, 0);  //Définit la couleur de l'écran en RGB
  lcd.setCursor(0, 0);  //Définit la position du curseur (16×2)

  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  pinMode(A3, INPUT_PULLUP);

  EEPROM.get(18, compteurPilesTestees);
  EEPROM.get(172, compteurPilesSauvees);

}

void loop() {
  affichage(lectureTension());
  delay(100);
}
