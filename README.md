# Capteur-EnergyLab
création d'un circuit imprimé doté de capteurs permettant de prendre des mesures de température, humidité, intensité de courant, etc. Voici le code Arduino associé à ce circuit, renvoyant les données sur une carte SD et un serveur cloud Adafruit IO.

Pour initialiser le code et l'utiliser vous même: 

à la ligne 5 du fichier config.h, modifier : 
#define IO_USERNAME "votre username du feed adafruit IO"
#define IO_KEY "votre clé"
à la ligne 23 du fichier config.h, modifier les données du partage de connexion: 
#define WIFI_SSID "nom du partage de connexion"
#define WIFI_PASS "mot de passe"
Ce sont les seuls paramètres à modifier dans les codes. 
Il faudra télécharger plusieurs librairies: 
Adafruit_BME280.h
RTClib.h
SPI.h
SD.h
Adafruit IO Arduino
Il faudra également rajouter la carte: 
aller dans file/fichier > Preferences/Préférences > Settings/paramètres puis coller la ligne suivante dans Additional boards manager/gestionnaire de cartes additionnelles (devancé d'une virgule s'il y a déjà un lien à cet emplacement):  
http://arduino.esp8266.com/stable/package_esp8266com_index.json
