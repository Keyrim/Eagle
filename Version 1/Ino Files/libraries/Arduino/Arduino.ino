#include <Servo.h>
#include<Wire.h>

const int MPU=0x68;  // I2C address of the MPU-6050
bool etatSignal [4];
unsigned long curentTime,timer [4] ;
int signal [4] ;

//fonction qui permet de declaré les pins commme interuptions
void pciSetup(byte pin)
{
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}

//definition des vartiables global
const unsigned char constante_moyenne_glissante = 50 ;
unsigned char i = 0;
int valeursX[constante_moyenne_glissante];
int valeursY[constante_moyenne_glissante];
float somme[2] ;    //0 pour x et 1 pour y
float angles[2] ;    //0 pour x et 1 pour y 
unsigned long loop_timer = 0;
//Variables lecture série
String mot ="" ;
bool data_available = false ;

//Consigne des moteurs et signals telecomandes
int commandes_moteur[3] = {1000, 80, 80};
int signals_telecomande[4];
float batterie_tension[3];
unsigned char compteur_donne_recu = 1;

//Declare mes moteurs avec la librairie Servo
Servo ESC;
Servo Servo1;
Servo Servo2;

//fonction qui met a jour les valuers d'angles X et Y
void update_angles()
{
  float AcX,AcY,AcZ,GyX,GyY,GyZ,AcTotal, Tmp;
  float AcXangle , AcYangle;
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x3B);                                                    //Send the requested starting register
  Wire.endTransmission();                                              //End the transmission
  Wire.requestFrom(0x68,14);                                           //Request 14 bytes from the MPU-6050
  while(Wire.available() < 14);                                        //Wait until all the bytes are received
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  AcTotal = sqrt(AcX*AcX + AcZ*AcZ + AcY*AcY);
  AcXangle = asin(AcX/AcTotal) * 57.296 ;
  AcYangle = asin(AcY/AcTotal) * -57.296 ;
  somme[0]-=valeursY[i];
  somme[1]-=valeursX[i];
  valeursX[i] = AcXangle ;
  valeursY[i] = AcYangle ; 
  somme[1]+= valeursX[i];
  somme[0]+= valeursY[i];
  angles[0] = somme[1] / constante_moyenne_glissante; // constante_moyenne_glissante
  angles[1] = somme[0] / constante_moyenne_glissante; // constante_moyenne_glissante
  i++;
  if(i == constante_moyenne_glissante ) i = 0 ;
}

void update_batterie()
{
  //a faire
}

void update_serial()
{
  while(Serial.available()>0)
  {
    char msg = Serial.read();    
    if(msg == '\n')
    {
      int nombre = mot.toInt();    
      int indice = nombre & 15 ;
      nombre -= indice ;    
      nombre = nombre >> 4; 
      commandes_moteur[indice] = nombre ;
      digitalWrite(13, !digitalRead(13));
      mot = "";
      compteur_donne_recu += 1;
    }
    else mot.concat(msg) ;  
  }
  
}

void write(char indice, int value)
{  
  String string = String(value << 4 + indice);
  string.concat('\n');
  Serial.print(string);
}

void setup(){
  //Initialisation liaison avec le mpu ainsi qu'avec le Pi 
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);

  //On attend de recevoir une consigne de depart avant de faire quoi que ce soit
  //Et on fait clignoter une petite lumiere pour dire qu'on est la quoi
  unsigned int led = 0 ;
  bool led_state = true ;
  pinMode(13, OUTPUT);
  

  //On associe les servos a leurs pin
  ESC.attach(5);
  Servo1.attach(6);
  Servo2.attach(7);
    
  //On utilise la fonction déclaré precedement pour initaliser les interuptions sur les pin suivants
  pciSetup(10);
  pciSetup(8);
  pciSetup(9);
  pciSetup(11);

  loop_timer = micros();
}


void loop(){
  //On met a jour les données
  update_angles();
  //update_batterie();

  //On lit les données , on envoi les notres
  update_serial();  
  write(0, angles[0]);
  write(1, angles[1]);
  write(2, signals_telecomande[0]);
  write(3, signals_telecomande[1]);
  write(4, signals_telecomande[2]);
  write(5, signals_telecomande[3]);
    
  
  //On fait tourner les moteurs comme il le faut
  ESC.writeMicroseconds(signals_telecomande[2]-30);
  Servo1.writeMicroseconds(signals_telecomande[0]);
  Servo2.writeMicroseconds(signals_telecomande[1]);
  /*Serial.print(signals_telecomande[2]);
  Serial.print("_____");
  Serial.print(signals_telecomande[0]);
  Serial.print("_____");
  Serial.println(signals_telecomande[1]);*/
  delay(20);

}



//fonction qui met a jour les signaux d'entrée de la télécomande apr détéction d'interuption sur les pins du port B
ISR (PCINT0_vect) // handle pin change interrupt for D8 to D13 here
 {    curentTime = micros();

 for(int n=0 ; n <4; n++){
     if(digitalRead(n+8)==1){
      if(etatSignal[n]==0){
        etatSignal[n] = 1 ;
        timer[n] = curentTime ;
      }
     }
     else if(etatSignal[n]==1){
      etatSignal[n] = 0 ;
      signals_telecomande[n] = micros()-timer[n] ;
     }
  } 
 }
