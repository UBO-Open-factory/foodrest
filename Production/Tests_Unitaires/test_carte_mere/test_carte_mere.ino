/**
   Gestion des mesures de la poubelle connectée.
*/

// Variables globales
#include "globalVariable.h"


// Librairie de gestion des erreurs
#include "GestionErreur.h"

// Librairies pour la lecture de la configuration
#include "Config.h"

// Librairies pour le fonctionnement avec le Back office de TOCIO
#include "Tocio.h"

// Librairie de gestion de la balance
#include "balance.h"

// Librairie de gestion de la balance
#include "RTC.h"

// Librairie de gestion de la saisie de l'ID de la balance
#include "IDpoubelle.h"

// Librairies contenant les fonctions pour lire les mesures physiques.
#include "Functions.h"




// ************************************************************************************
void setup() {
  // initialisation de la liaison série (pour le moniteur)
  Serial.begin(115200);
  delay(200);  // attente pour l'init de la liaison serie

  // Orientation des E/S
  pinMode(LED_PESEE_PIN, OUTPUT);
  pinMode(GND_C_EN, OUTPUT);
  pinMode(MASQUE_RESET, OUTPUT);

  // Prépositionement des E/S
  digitalWrite(LED_PESEE_PIN, LOW);    // éteint led "pesée en cours"
  digitalWrite(GND_C_EN, LOW);         // coupe l'alimentation des périphériques (RTC, balance carte SD)
  digitalWrite(MASQUE_RESET, HIGH);      // autorisation du RESET lorsque le capot est ouvert

  //Initialisation du code erreur en mode normal
  code_erreur_normal = ERREUR_NO_ERREUR;

  // Initialisaiton de la balance ---------------------------------------------------------------------
  balance.begin(BALANCE_DAT, BALANCE_CLK);
  balance.set_scale();





Serial.println ("**************************************************************");
Serial.println ("*****               TEST DE LA CARTE MERE                *****");
Serial.println ("**************************************************************");
Serial.println ("");
Serial.println ("Vérifiez que la poubelle est vide.");
Serial.println ("Vérifiez que le capot est fermé.");
Serial.println ("     Branchez");
Serial.println ("         les leds");
Serial.println ("         le Firebeetle");
Serial.println ("         le lecteur de carte SD");
Serial.println ("         La RTC");
Serial.println ("         La balance");
Serial.println ("         le contacteur de capot");
Serial.println ("         la batterie à travers l'interface à bornes");
Serial.println ("         le multimètre (en ampèremètre => mA) sur l'interface à bornes");
Serial.println ("      sur la carte mère.");
Serial.println ("     Mettez l'interrupteur sur OFF");
Serial.println ("");
Serial.println ("Puis pressez une touche");
Serial.println ("");
while (!Serial.available());
while (Serial.available()) Serial.read();
Serial.println (" ----------------------------------------------------------------");
Serial.println ("                         TEST DE LA LED PESEE (led jaune)");
Serial.println ("");
digitalWrite(LED_PESEE_PIN, HIGH);    // allumage de la led pesée
Serial.println ("La led est-elle allumée (O/N)");
while (!Serial.available());
while (Serial.available()){
  String reponse = Serial.readString().substring(0,1);
  if (reponse !="O" and reponse !="o") {
    erreur_test("Verifiez le cablage de la led Pesée");
  }
}

// alimentaion des périphériques
  digitalWrite(GND_C_EN, HIGH);      // alimentation des périphériques


digitalWrite(LED_PESEE_PIN, LOW);    // extinction de la led pesée
Serial.println ("");
Serial.println (" ----------------------------------------------------------------");
Serial.println ("                         Test de la RTC");
Serial.println ("");
// écrit une heure et une date imaginaire dans la RTC 07/05/1965-07h45,12
// lit l'heure de la rtc
// compare
// si les deux heures sont différentes ou si erreur RTC => Erreur vérifier alimentaion des périphériques et cablage RTC
Serial.print ("Date indiquée par la RTC (elle doit être à peu près à lheure) :");
Serial.println (rtc_getDate());


Serial.println ("La date et l'heure sont-elles correctes (O/N) ? ");

while (!Serial.available());
while (Serial.available()){
  String reponse = Serial.readString().substring(0,1);
  if (reponse !="O" and reponse !="o") {
    erreur_test("Il y a probablement un soucis avec le cablage de la RTC");
  }
}

Serial.println ("");
Serial.println (" ----------------------------------------------------------------");
Serial.println ("                         Test du lecteur de carte SD");
Serial.println ("");
Serial.print ("Vérification présence lecteur et carte SD ...");
  if (!SD.begin(0))  erreur_test("Verifiez la présence du lecteur de carte SD");
  if (SD.cardType() == CARD_NONE)  erreur_test("Verifiez la présence d'une carte SD.");
  Serial.println ("OK");

// teste création/écriture/fermeture/lecture/destruction dans fichier
Serial.print ("Test écriture / lecture / destruction fichier sur carte SD ....");

String fileName = "/essai.txt";
File myFile = SD.open(fileName, FILE_WRITE);
  if (myFile) {
    // Ecrit les données dans le fichier.
    myFile.println("Hello les poteaux ! ");
    myFile.println();
    myFile.close();
  } else {
    erreur_test("Impossible de créer le fichier sur la carte SD.");
  }

// test de lecture sur la carte SD
 //Read file line by line
  myFile = SD.open(fileName);
  if (myFile) {
    String buffer;
//    while (myFile.available()) {
      buffer = myFile.readStringUntil('\n');
//    }
    myFile.close();
    if (buffer.substring(0,20) != "Hello les poteaux ! ") { // substring pour enlever le retour chariot à la fin de la ligne
      erreur_test("Erreur lors de la lecture du fichier sur carte SD.");
    }
  
  } else {
    erreur_test("Lecture du fichier sur carte SD impossible.");
  }

// détruit le fichier 
 SD.remove(fileName);
 Serial.println ("OK");

Serial.println ("");
Serial.println (" ----------------------------------------------------------------");
Serial.println ("                         Test de la balance");
Serial.println ("");

Serial.println ("La balance va peser continuellement et afficher le poids mesuré.");
Serial.println ("   ATTENTION : la balancen'est pas tarée et affichera un poids");
Serial.println ("   certainement fantaisiste.");
Serial.println (" Appuyez légèrement sur la balance pour vérifier si celle-ci affiche bien ");
Serial.println (" les variations de poids.");
Serial.println (" ");
Serial.println (" Pour commencer puis quitter le test appuyez sur une touche");
while (!Serial.available());
while (Serial.available()) Serial.read();

while (!Serial.available()){

  // pesée de la poubelle
  balance.set_scale(configLocale.calibrationFactor);
  float poidNew = BALANCE_pesee_balance() + (configLocale.valeurDeTarage / configLocale.calibrationFactor);
  Serial.print ("Poids mesuré : ");
  Serial.println (poidNew);
 
}
while (Serial.available()) Serial.read();


Serial.println ("");
Serial.println (" ----------------------------------------------------------------");
Serial.println ("                         Test du la lecture de la tension batterie");
Serial.println ("");


Serial.println ("La balance va afficher la tension de la batterie.");
Serial.print ("Tension mesurée (sortie CAN) : ");
int tension = analogRead(A0);
Serial.print(tension);
Serial.print (" ... ");
if (tension>500) {
  Serial.println ("OK");
} else {
  erreur_test("ERREUR lors de la lecture de la tension batterie.");
}

Serial.println ("");
Serial.println (" ----------------------------------------------------------------");
Serial.println ("                         Test du contacteur de capot");
Serial.println ("");
// mise en lace du masque de reset
 digitalWrite(MASQUE_RESET, LOW);      // interdiction du RESET lorsque le capot est ouvert
Serial.println ("Si la carte mère se réinitialise lorsque vous ouvrez le couvercle,");
Serial.println ("il faudra vérifier le cablage du masque de RESET.");
Serial.println ("");
Serial.println ("Si rien ne se passe lorsque vous ouvrez le capot ... c'est normal. Pressez une touche pour continuer.");
Serial.println ("");
Serial.println ("Ouvrez et fermez le couvercle.");

while (!Serial.available());
while (Serial.available()) Serial.read();
  
  // Sortie "propre"
  digitalWrite(MASQUE_RESET, HIGH); // autorise le RESET lorsque le capot est ouvert
  digitalWrite(GND_C_EN, LOW);      // coupe l'alimentation des périphériques
  digitalWrite(LED_PESEE_PIN, LOW); // extinction de la led "pesée en cours"

Serial.println ("");
Serial.println (" ----------------------------------------------------------------");
Serial.println ("                         Test de l'ouverture du capot");
Serial.println ("");
Serial.println ("La poubelle va passer en deep_Sleep.");
Serial.println ("D&brancher le cordon USB et mesurer la consommation en passant sur le calibre 'µA'");
Serial.println ("Rebrancher le cordon USB");
Serial.println ("Ouvrez et fermez le couvercle.");
Serial.println ("Si la carte mère reboot :  la carte mère est consdérée comme OK. ");
Serial.println ("Dans le cas contraire, il faudra vérifier le cablage du RESET et du masque");
Serial.println ("de reset.");
Serial.println ("");
affichage_erreurs_mode_normale(); // pour passer en deep sleep
}

void loop() {
}


void erreur_test(String chaine) {
  Serial.println ("");
  Serial.println ("************************************************************");
  Serial.println ("*****");
  Serial.println ("*****");
  Serial.println ("*****                 "+chaine);
  Serial.println ("*****");
  Serial.println ("*****");
  Serial.println ("*****");
  Serial.println ("*****");
  Serial.println ("************************************************************");
  Serial.println ("");
  Serial.println ("");
  while (true);

  
}
