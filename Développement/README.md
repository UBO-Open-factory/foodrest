

# FoodRest
Code pour le Projet de poubelle connectée basée sur la solution Open Source [TOCIO](https://github.com/UBO-Open-factory/TOCIO-Back-office) permettant de collecter et afficher des données.

Ce projet présente le code Arduino pour un ESP32.

# Principe de fonctionnement du programme
Principe d’enregistrement de mesures lors de l’utilisation de la poubelle connectée.

En jaune : Les interactions avec l’utilisateur.


![Algorythme](./Doc/Algorythme.png?raw=true "Algorythme")

# Installations
## Librairies Arduino utilisées
Plusieures librairies tierces sont utilisées dans le projet, pour faciliter l'intégration elles sont toutes mises 
dans le répertoire **Librairies**. 
Il suffi donc de les prendre et de les glisser dans le répertoire Arduino/librairies 
(Sous windows : C:\Users\<user>\Documents\Arduino\libraries en remplacant <user> par votre nom d'utilisateur windows.

### SDConfigFile
La librairie **SDConfigFile** est la librairie disponible sur le [dépot Git](https://github.com/bneedhamia/sdconfigfile)

Cette librairie est également disponible dans le répertoire du projet (répertoire /Librairies/SDConfigFile ).


### WiFiClientSecure
La librairie **WiFiClientSecure** est la librairie disponible sur le [dépot Git](https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFiClientSecure)

Cette librairie est également disponible dans le répertoire du projet (répertoire /Librairies/WiFiClientSecure ).

### RTC
La librairie **RTC** utilisée est celle disponible sur le [dépot Git officiel Adafruit](https://github.com/adafruit/RTClib)
[Mode d'emploi](https://learn.adafruit.com/adafruit-pcf8523-real-time-clock?view=all)

Cette librairie est également disponible dans le répertoire du projet (répertoire /Librairies/RTClib ).

### Balance (HX711)
La librairie **HX711** utilisée est celle disponible sur le [dépot GIT](https://github.com/bogde/HX711)

Cette librairie est également disponible dans le répertoire du projet (répertoire /Librairies/HX711 ).

## Dépendance des librairies utilisées
Les librairies doivent être incluses les unes par rapport au autres dans l'ordre suivant :
* globalVariable.h
  * SPI.h
* GestionErreur.h
* Config.h
  * SDCard.h
	* SDConfigFile.h
		* SD.h
	* FS.h
	* SPI.h
* Tocio.h
  * WiFiClientSecure.h
    * WiFi.h
* Balance.h
  * HX711.h
* RTC.h
  * Wire.h
  * RTClib.h
  * NTPClient.h
  * WiFiUDP.h
* IDpoubelle.h
* Functions.h
  * Wire.h
  

# Configuration de Arduino
## Installation des cartes ESP32 dans l'IDe Arduino
Lancez l’IDE Arduino et ouvrez les préférences depuis le menu Fichier. Sur macOS, allez dans le menu Arduino puis Préférences… Cliquez sur l’icône indiquée par la flèche rouge sur la photo ci-dessous.
![Ajout de Carte Node MCU dans l'IDE](./Doc/Arduino_1.png?raw=true "IDE")
Dans le champ de saisie, collez l’adresse internet suivante puis validez
```
https://dl.espressif.com/dl/package_esp32_index.json
```
> Si vous avez déjà des définitions de cartes, vous pouvez les cumuler en les les séparant par des virgules.
> Par exemple, dans mon cas j'ai : http://arduino.esp8266.com/stable/package_esp8266com_index.json,https://dl.espressif.com/dl/package_esp32_index.json

Maintenant, allez dans le menu Outils > Type de Cartes > Gestionnaire de carte.
Dans le champ de recherche, saisissez ESP32 pour trouver les nouvelles cartes à installer. Si les cartes n’apparaissent pas, vérifiez dans les préférences que l’URL est correcte. vérifiez également la connexion Internet. Cliquer sur Installer
![Import de la carte Node MCU dans l'IDE](./Doc/Arduino_2.png?raw=true "Import")

L’installation terminée, fermez le gestionnaire de carte et ouvrez de nouveau le menu Outils > Type de Carte. 
Vous avez maintenant une nouvelle section appelée ESP32 Boards (2.7.4)

## Sélection de la carte 
![Utilisation de la carte Firebeetle dans Arduino](./Doc/Arduino_Conf_NodeMCU2.png?raw=true "Utilisation de la carte Firebeetle dans Arduino")
