/**
   Ce fichier contient toutes les fonctions pour la gestion des erreurs
*/



/**
   Affiche un message d'erreur
*/
void AfficheErreur(String chaine) {
  Serial.println(chaine);
}

/**
   Affiche une ligne pour le debug
   @see : http://www.pinon-hebert.fr/Knowledge/index.php/ARDUINO_predefined_constants

  void TraceDebug(String chaine) {
  if ( configLocale.AfficheTraceDebug ) {
    Serial.print(__FILE__); Serial.print(" ");
    Serial.print(__DATE__); Serial.print(" ");
    Serial.print(__TIME__); Serial.print(" ");
    Serial.print(__func__); Serial.print(" ");
    Serial.print(__LINE__); Serial.print(":");
    Serial.println( chaine );
  }
  }
*/
#define TraceDebug(message) \
  if( configLocale.AfficheTraceDebug ) {Serial.print("DEBUG("); Serial.print(__FILE__); Serial.print(" "); Serial.print(__func__); Serial.print(","); Serial.print(__LINE__); Serial.print(")>"); Serial.println(message);}


void affichage_erreurs_mode_normale() {
  // Erreur en mode normal
  //  Priotité 1 WIFI : clignotement rapide pendant une durée déterminée
  // priorité 2 Batterie faible : clignottement lent pendant une durée déterminée
  //
  //
  if ( code_erreur_normal == ERREUR_CARTE_SD) {
    Serial.println ("ERREUR CARTE SD");
    // Cette erreur ne mets pas l'ESP en deep Sleep. La led clignote jusqu'à la mort de la batterie (ou arrêt de la balance)
    int k=periode_led_erreur_sd/2;
    while(true) {
      digitalWrite(LED_PESEE_PIN, HIGH);
      delay(k);
      digitalWrite(LED_PESEE_PIN, LOW);
      delay(k);
    }
  }

  // affichage de l'erreur WIFI
  if ( code_erreur_normal == ERREUR_WIFI) {
    Serial.println ("ERREUR WIFI");
    int n = (duree_clignotement_erreur / periode_led_erreur_wifi)+1;
    int k=periode_led_erreur_wifi/2;
    for (int i=0; i<n; i++) {
      digitalWrite(LED_PESEE_PIN, HIGH);
      delay(k);
      digitalWrite(LED_PESEE_PIN, LOW);
      delay(k);
    }
  }

  // affichage de l'erreur LOW_BAT
  if ( code_erreur_normal == ERREUR_LOW_BAT) {
     Serial.println ("BATTERIE FAIBLE");
   int n = (duree_clignotement_erreur / periode_led_erreur_low_bat)+1;
    int k=periode_led_erreur_low_bat/2;
    for (int i=0; i<n; i++) {
      digitalWrite(LED_PESEE_PIN, HIGH);
      delay(k);
      digitalWrite(LED_PESEE_PIN, LOW);
      delay(k);
    }
  }

  // passage en deep sleep si ce n'est pas une erreur inhérenta à la carte SD
  esp_deep_sleep_start();




}


  
