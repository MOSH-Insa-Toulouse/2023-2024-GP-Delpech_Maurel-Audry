#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <SoftwareSerial.h>


//Définition des pins analogiques pour les capteurs

#define PIN_GRAPH_SENSOR A0 // Capteur graphite sur l'entrée analogique A0
#define PIN_FLEX_SENSOR A1 // Capteur de flexion sur l'entrée A1

// Définition des pins numériques pour l'encodeur rotatoire
#define CLK  2  
#define DT  4  
#define Switch 5 

//Définition des broches RXD et TXD du module Bluetooth HC-05
#define bluetoothTx 7 // Broche TXD du module Bluetooth connectée à la broche 7 d'Arduino
#define bluetoothRx 8 // Broche RXD du module Bluetooth connectée à la broche 8 d'Arduino

// Déclaration d'un objet de type SoftwareSerial pour la communication avec le module Bluetooth
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

// Déclaration de l'écran OLED avec la bibliothèque AdaFruit

#define SCREEN_WIDTH 128    // Taille horizontale de l'écran OLED  
#define SCREEN_HEIGHT 64    // Taille verticale 
#define OLED_RESET    -1    // Reset de l'écran est le même que celui de l'Arduino Uno
#define SCREEN_ADDRESS 0x3C // Adresse I2C de l'écran

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Déclaration pour les menus de l'écran
int selectedItem = 0;
bool inSubMenu = false;
const int numItems = 2;
const char *menuItems[] = {"Afficher les mesures", "Modifier resistance"};

// Déclaration pour le potentiomètre digital
const byte csPin           = 10;      
const int  maxPositions    = 256;     
const long rAB             = 50000.0;    
const byte rWiper          = 125;     
const byte pot0            = 0x11;    // pot0 addr // B 0001 0001
const byte pot0Shutdown    = 0x21;    // pot0 shutdown // B 0010 0001
int pos_res_variable = 5; // Fixe la résistance initiale du potentiomètre digital

// Déclaration pour le capteur de flexion et le capteur graphite
const float VCC = 5;
const float flexR_DIV = 39000.0;
const float flexflatResistance = 25000.0;
const float flexbendResistance = 100000.0;
float tension_graphite = 0;
float tension_flex = 0;
long resistance_variable = 0;
float resistance_graphite = 0;
long resistance_flex = 0;
char resultGraphite[15];
char buffer[15];
char resultFlex[15];
char buffer_flex[15];

// Déclaration pour l'encodeur rotatoire
volatile unsigned int lastStateSW; // variable qui enregistre l'état du bouton à la boucle précédente

/* ~~~~ Initialisation du programme ~~~~~ */

void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);
  
 // Initialisation des pins 
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  pinMode(Switch, INPUT_PULLUP);
  pinMode(PIN_GRAPH_SENSOR, INPUT);
  pinMode(PIN_FLEX_SENSOR, INPUT);
    
  lastStateSW = digitalRead(Switch); // Vérifie l'état du bouton au lancement du programme

  attachInterrupt(0, doEncoder, RISING); //Définit l'interruption lorsque l'encodeur rotatoire tourne
 
 // Initialisation de l'écran
 if(!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  oled.setTextSize(1);
  oled.display();
  delay(2000);
  oled.clearDisplay();

 // Initialisation de la communication avec le potentiomètre digital
  digitalWrite(csPin, HIGH);        
  pinMode(csPin, OUTPUT);           
  SPI.begin();

}

/* ~~~~ Programme principal ~~~~~ */

void loop() {

  // Lecture de l'état du bouton pour passer du menu et aus sous-menu
  int stateSW = digitalRead(Switch);
  if (stateSW != lastStateSW && stateSW == LOW) {
    Serial.println(F("Clic!"));
    inSubMenu = !inSubMenu;
  }
  lastStateSW = stateSW;

  setPotWiper(pot0,pos_res_variable); // Modifie la résistance du potentiomètre

 // Mesures des capteurs et résistance variable
 tension_graphite = analogRead(PIN_GRAPH_SENSOR)*VCC/1023.0 ;
 resistance_variable = ((rAB * pos_res_variable) / maxPositions ) + rWiper;
 resistance_graphite = ((1+ (100000/resistance_variable))*100000*(VCC/tension_graphite)-110000)/1000000;
  
 tension_flex = analogRead(PIN_FLEX_SENSOR) *VCC/1023;

 resistance_flex = flexR_DIV * (VCC / tension_flex - 1);
 

 // Gestion de l'affichage sur l'écran
  oled.clearDisplay();

 if (!inSubMenu) {
    for (int i = 0; i < numItems; ++i) {
      if (i == selectedItem) {
        oled.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      } else {
        oled.setTextColor(SSD1306_WHITE);
      }
      oled.setCursor(0, i * 10);
      oled.println(menuItems[i]);
    }
  } 
  else {
    oled.setTextColor(SSD1306_WHITE);
    oled.setCursor(0,0);

    switch(selectedItem){ 

      case(0):
        oled.println(F("Tension graphite :  "));
        oled.print(tension_graphite);
        oled.println(F(" V"));
        oled.println();
        oled.println (F("Resistance graphite :"));
        oled.print(resistance_graphite);
        oled.println(F(" MOhms"));
        oled.println();
        oled.println(F("Resistance flex : "));
        oled.print(resistance_flex);
        oled.println(F(" Ohms"));
        break;

      case(1):             
        
        oled.println(F("Resistance variable :"));
        oled.print(resistance_variable);
        oled.println(F(" Ohms"));
        oled.println();
        oled.println(F("Tension graphite :  "));
        oled.print(tension_graphite);
        oled.println(F(" V"));
                        
        break;

      default:
        oled.println(F("la reponse D"));
        break;

    }
    
  }
  
  oled.display();
  

  // Bluetooth
  dtostrf(tension_graphite, 4, 2, buffer); // Transforme la mesure float en une chaîne de caractères    
    
  sprintf(resultGraphite,"g%s\n", buffer); //remplit le tableau resultGraphite de la mesure au format g"mesure"\n pour que l'appli puisse la lire
  bluetooth.write(resultGraphite); // envoie le contenu du tableau vers le module bluetooth
  delay(100);    
  
  dtostrf(tension_flex, 4, 2, buffer_flex);
  sprintf(resultFlex,"f%s\n",buffer_flex);
  bluetooth.write(resultFlex);
    
  if (bluetooth.available()) {
    // Lit les données reçues depuis le module Bluetooth
    char receivedChar = bluetooth.read();
    
    // Affiche les données reçues sur le moniteur série
    Serial.print(receivedChar);
  }

  delay(100);
}

/* ~~~~ Fonctions supplémentaires ~~~~ */


// Fonction lors de l'interrupt de l'encodeur rotatoire
void doEncoder() {
  // Cas du menu principal
  if(!inSubMenu){ 
    if (digitalRead(DT)==HIGH) {
        selectedItem = (selectedItem == 0) ? numItems - 1 : selectedItem - 1;    // Dans le sens horaire fait défiler vers le bas puis boucle vers le premier item de la liste
    }
    else {
          selectedItem = (selectedItem == numItems - 1) ? 0 : selectedItem + 1; // L'inverse pour le sens trigonomètrique
    }
  }
  /* Cas du sous-menu de choix du gain

  La rotation permet, par incrément de 1, de changer une position entre 0 et 255 correspondant
  aux différentes valeurs de résistances possibles du potentiomètre digital.

  */
  else {
    if (selectedItem == 1) {
      if (digitalRead(DT)==HIGH) {      
        pos_res_variable++;
        if (pos_res_variable >= 255) {
          pos_res_variable= 255;
        }
      }
      else {
      
      pos_res_variable--;
      if (pos_res_variable <= 0) {
       pos_res_variable= 0;
      }
      }
    }
      
  }  
}

// Fonction de modulation de la résistance du potentiomètre digital
void setPotWiper(int addr, int pos) {
  pos = constrain(pos, 0, 255);            
  digitalWrite(csPin, LOW);                
  SPI.transfer(addr);                      
  SPI.transfer(pos);
  digitalWrite(csPin, HIGH);               
}

