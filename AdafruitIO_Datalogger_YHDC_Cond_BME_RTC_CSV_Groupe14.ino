#include <Adafruit_BME280.h>                            // Inclusion de la librairie BME280 d'Adafruit
#include "RTClib.h"                                     //inclusion librairie horloge
#include <SPI.h>                                        //inclusion lib communication
#include <SD.h>                                         //inclusion lib carte SD

const int chipSelect = 15;

RTC_DS3231 rtc;                                        

// Constantes du programme
#define adresseI2CduBME280                0x76            // Adresse I2C du BME280 (0x76, dans mon cas, ce qui est souvent la valeur par défaut)
#define pressionAuNiveauDeLaMerEnHpa      1024.90         // https://fr.wikipedia.org/wiki/Pression_atmospherique (1013.25 hPa en moyenne, valeur "par défaut")
#define delaiRafraichissementAffichage    2000            // Délai de rafraîchissement de l'affichage (en millisecondes)
unsigned long myTime;
int val[100];
int sensorValue1 = 0;
// Instanciation de la librairie BME280
Adafruit_BME280 bme;

#include "config.h"

//délais pour les différentes exécutions
int delai1 = 0 ;     
int delai2 = 0;

// setup des feeds
AdafruitIO_Feed *Temperature = io.feed("Temperature");
AdafruitIO_Feed *Humidite = io.feed("Humidite");
AdafruitIO_Feed *Altitude = io.feed("Altitude");
AdafruitIO_Feed *Pression = io.feed("Pression");
AdafruitIO_Feed *Iefficace = io.feed("Intensite efficace");


float Vefficace = 0;
float tensionSortieConditionneur = 0;
float Ieff = 0;
float max_v = 0;
float VmaxD = 0;
float VeffD = 0;
float Imax = 0;

// ========================
// Initialisation programme
// ========================

void setup() {
  Serial.begin(9600);

  #ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
  #endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  while(!Serial);
  Serial.println("Programme de test du BME280");
  Serial.println("===========================");
  Serial.println();

  // Initialisation du BME280
  Serial.print(F("Initialisation du BME280, à l'adresse [0x"));
  Serial.print(adresseI2CduBME280, HEX);
  Serial.println(F("]"));
  
  if(!bme.begin(adresseI2CduBME280)) {
    Serial.println(F("--> ÉCHEC…"));
    while(1);                              // Arrêt du programme, en cas d'échec de l'initialisation
  } else {
    Serial.println(F("--> RÉUSSIE !"));
  }
  Serial.println();

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port onl
  }

  //SD CARD
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  // END SD CARD

  // wait for serial monitor to open
  while(! Serial);

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }


pinMode(A0, INPUT);  //connecter le capteur pour le TP3

}

// ========================
// boucle du programme
// ========================
void loop() {
  float adc = analogRead(A0);   //valeur analogique reçue
  float voltage = adc /1023;    // conversion de bits en volts 
  Ieff = voltage*15;            //valeur de l'intensité efficace (15 est le coef entre V/I, se situe sur la pince bleue)

//création de la chaine de caractères qui sera écrite dans la carte SD avec: le temps unix, la température, l'humidité, la pression, l'altitude, l'intensité efficace
  DateTime now = rtc.now();
  String dataString = "";
  dataString += String(now.unixtime());
  dataString += ";";

  dataString += String(bme.readTemperature());
  dataString += ";";

  dataString += String(bme.readHumidity());
  dataString += ";";

  dataString += String(bme.readPressure() / 100.0F);
  dataString += ";";

  dataString += String(bme.readAltitude(pressionAuNiveauDeLaMerEnHpa));
  dataString+=";";

  dataString += Ieff;
  dataString += ";";

  if(millis()-delai2 >= 2000){    //toutes les 2 secondes on upload nos valeurs sur le dashboard
    File dataFile = SD.open("datalogGroupe14.txt", FILE_WRITE); //on ouvre la carte sd numériquement

    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(dataString); //on rentre notre chaine de caractères
      dataFile.close();   
      // print to the serial port too:
      Serial.println(dataString);
    }
    // if the file isn't open, pop up an error:
    else {
      Serial.println("error opening datalog.txt");
    }
    //END OF WRITING
    delai2 = millis();
  }  

  if(millis()-delai1 >= 30000){    //toutes les 30 secondes on upload nos valeurs sur le dashboard
      io.run();  
      // save to the feed on Adafruit IO, which will be seen on the online dashboard
      Temperature->save(bme.readTemperature());
      Humidite->save(bme.readHumidity());
      Altitude->save(bme.readAltitude(pressionAuNiveauDeLaMerEnHpa));
      Pression->save(bme.readPressure() / 100.0F);
      Iefficace ->save(Ieff);
      delai1 = millis();
  }
}
