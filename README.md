# FoodRest
Code pour le Projet de poubelle connectée basée sur la solution Open Source [TOCIO](https://github.com/UBO-Open-factory/TOCIO-Back-office) permettant de collecter et afficher des données.

Ce projet présente le code Arduino pour un ESP ou un Arduino.

# Principe de fonctionnement du programme
- TODO -


## Librairies Arduino utilisées
Plusieures librairies tierces sont utilisées dans le projet, pour faciliter l'intégration elles sont toutes mises dans le répertoire **Librairies Arduino**. Il suffi donc de les prendre et de les glisser dans le répertoire Arduino/librairies (Sous windows : C:\Users\<user>\Documents\Arduino\libraries en remplacant <user> par votre nom d'utilisateur windows.

### WiFiClientSecure
La librairie **WiFiClientSecure** est la librairie disponible sur le [dépot Git](https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFiClientSecure)

### RTC
La librairie **RTC** utilisée est celle disponible sur le [dépot Git officiel Adafruit](https://github.com/adafruit/RTClib)
[Mode d'emploi](https://learn.adafruit.com/adafruit-pcf8523-real-time-clock?view=all)

### Balance (HX711)
La librairie **HX711** utilisée est celle disponible sur le [Arduino core for ESP32](https://github.com/espressif/arduino-esp32)
