
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <MFRC522.h>

#ifndef STASSID
#define STASSID "Lucasp30"
#define STAPSK  "lucas92i"

#define SS_PIN 15
#define RST_PIN 4

#endif



const int LEDR = 16;  //LED rouge
const int LEDV = 5;   // LED verte
const int nb = 0;    // nombre d'IDs présent sur le fichier
const bool access= 0; // Booléen autorisation de l'accès ou non 
const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "192.168.43.99";
const uint16_t port = 80;

byte newID[4]; //On initialise l'array ou l'on va stocker l'ID lu par le lecteur
byte checkID[4] // On initialise l'array qui servira à comparer

//Declaration
MFRC522 rfid(SS_PIN, RST_PIN); 
ESP8266WiFiMulti WiFiMulti;



void setup() {

  //Initialisation 
  Serial.begin(9600);
  rfid.PCD_Init();

  pinMode(LEDR, OUTPUT);      // set the LED pin mode
  pinMode(LEDV,OUTPUT);

  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
  

}

 void loop() {

    // Initialise la boucle si aucun badge n'est présent 
  if ( !rfid.PICC_IsNewCardPresent())
    return;

  // Vérifie présence d'un nouveau badge 
  if ( !rfid.PICC_ReadCardSerial())
    return;

  // Enregistrer l’ID du badge (4 octets) 
  for (byte i = 0; i < 4; i++) {
    newID[i] = rfid.uid.uidByte[i];
  }
    
  Serial.println(F("Badge détecté"));

    // Affichage de l'identifiant 
  Serial.println(" L'UID du tag est:");
  for (byte i = 0; i < 4; i++) 
  {
    Serial.print(newID[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  //CONNEXION 
  
  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    Serial.println("wait 5 sec...");
    delay(5000);
    return;
  }

  nb = client.readBytesUntil('\r'); // La première ligne du fichier indique le nombre d'id stocké sur le serveur.
  Serial.println(nb);

  for (byte i = 0; i < nb; i++) // On parcourt tout les ID incrit sur la page web
  {
    for (byte j = 0; j < 4; j++) 
    {
      checkID[i]=client.readBytes 
    }
    if ((newID[0]==checkID[0])&&(newID[1]==checkID[1])&&(newID[2]==checkID[2])&&(newID[03]==checkID[3])) // on compare chacun des 4 bits de l'ID détécter avec tout les id enregistrés.
    {
      access=1; // l'ID correspond, on autorise l'accès
    digitalWrite(LEDV, HIGH);
    delay(2000);
    digitalWrite(LEDV, LOW);
    Serial.println("Accès autorisé !");
    }
    else  //L'ID ne corresspond pas, l'accès est refusé.
    {
    digitalWrite(LEDR, HIGH);
    delay(2000);
    digitalWrite(LEDR, LOW);
    Serial.println("Accès refusé !");
    }
  }
  // Re-Init RFID
  rfid.PICC_HaltA(); // Halt PICC
  rfid.PCD_StopCrypto1(); // Stop encryption on PCD
  
  Serial.println("closing connection");
  client.stop();

  Serial.println("wait 5 sec...");
  delay(5000);

}
