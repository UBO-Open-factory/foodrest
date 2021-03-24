> **Attention**
> Ce projet est en cours de développement.

# FoodRest
Code pour le Projet de poubelle connectée basée sur la solution Open Source [TOCIO](https://github.com/UBO-Open-factory/TOCIO-Back-office) permettant de collecter et afficher des données.

Ce projet présente le code Arduino pour un ESP ou un Arduino.

# Principe de fonctionnement du programme
Principe d’enregistrement de mesures lors de l’utilisation de la poubelle connectée.

En jaune : Les interactions avec l’utilisateur.
En Rouge, ce qu'il faut encore affiner.

![Algorythme](./Doc/Algorythme.png?raw=true "Algorythme")

# Installations
## Driver de la FireBeetle.
La carte [FireBeetle ESP8266](https://wiki.dfrobot.com/FireBeetle_ESP8266_IOT_Microcontroller_SKU__DFR0489) utilisée nécessite l'installation d'un driver spécifique.
* [Télécharger](https://github.com/Arduinolibrary/DFRobot_FireBeetle_ESP8266_DFR0489/raw/master/CH340%20Driver.zip) du driver à partir du Dépot Git Arduinolibrary / DFRobot_FireBeetle_ESP8266_DFR0489
* Décompresser l'archive ZIP dans un répertoire.
* Double-cliquer sur le fichier **setup.exe**
* Même si vous ne l’avez jamais installé, il faut commencer par cliquer sur **désinstaller** !
* Cliquer sur **installer**
La carte FireBeetle peut maintenant être branchée sur un port USB et reconnu sur votre PC par Arduino.

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
La librairie **HX711** utilisée est celle disponible sur le [Arduino core for ESP32](https://github.com/espressif/arduino-esp32)

Cette librairie est également disponible dans le répertoire du projet (répertoire /Librairies/HX711 ).

## Dépendance des librairies utilisées
Les librairies doivent être incluses les unes par rapport au autres dans l'ordre suivant :
* globalVariable.h
  * SPI.h
* GestionErreur.h
* Config.h
  * SDCard.h
	* SD.h
    * SDConfigFile.h
* Tocio.h
  * ESP8266WiFi.h
  *  WiFiClientSecure.h
* Balance.h
  * HX711.h
* RTC.h
  * Wire.h
  * RTClib.h
  * NTPClient.h
  * WiFiUDP.h
* Functions.h

# Configuration de Arduino
## Installation des cartes Node MCU dans l'IDe Arduino
Lancez l’IDE Arduino et ouvrez les préférences depuis le menu Fichier. Sur macOS, allez dans le menu Arduino puis Préférences… Cliquez sur l’icône indiquée par la flèche rouge sur la photo ci-dessous.
![Ajout de Carte Node MCU dans l'IDE](./Doc/Arduino_1.png?raw=true "IDE")
Dans le champ de saisie, collez l’adresse internet suivante puis validez
```
https://arduino.esp8266.com/stable/package_esp8266com_index.json
```
Maintenant, allez dans le menu Outils > Type de Cartes > Gestionnaire de carte.
Dans le champ de recherche, saisissez esp8266 pour trouver les nouvelles cartes à installer. Si les cartes n’apparaissent pas, vérifiez dans les préférences que l’URL est correcte. vérifiez également la connexion Internet. Cliquer sur Installer
![Import de la carte Node MCU dans l'IDE](./Doc/Arduino_2.png?raw=true "Import")

L’installation terminée, fermez le gestionnaire de carte et ouvrez de nouveau le menu Outils > Type de Carte. 
Vous avez maintenant une nouvelle section appelée ESP8266 Boards (2.7.4)

## Sélection de la carte 
![Utilisation de la carte Firebeetle dans Arduino](./Doc/Arduino_Conf_NodeMCU2.png?raw=true "Utilisation de la carte Firebeetle dans Arduino")