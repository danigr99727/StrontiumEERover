#include <SD.h>                      
#define SD_ChipSelectPin 4 
#include <TMRpcm.h>           
#include <SPI.h>
TMRpcm tmrpcm; 

unsigned char velocity = 255;
unsigned char dire = 0;
bool r = false;
bool l = false;

bool x = true;
bool y = true;

static unsigned long Time;
static unsigned long startTime;
static unsigned long infoTime;
int refreshing = 1;

short value;
short ir;
short rada;
short radb;
short rad;
short maxim;
short list[8];
short count;

unsigned short usvalue;
unsigned short magnetvalue;
unsigned short irperiod;
unsigned short rperiod;


unsigned short rfreq;
unsigned short irfreq;
unsigned char carrierfreq;

bool up = false;
bool down = false;

void setup() {
  tmrpcm.speakerPin = 9; //5,6,11 or 46 on Mega, 9 on Uno, Nano, etc
  if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
    Serial.println("SD fail");  
    return;   // don't do anything more if not
  }
  //tmrpcm.quality(1);
  tmrpcm.play("job.wav");
  tmrpcm.setVolume(5);
  
  pinMode (2, OUTPUT);      // Left
  pinMode (3, OUTPUT);      // Left Speed
  pinMode (7, OUTPUT);      // Right
  pinMode (5, OUTPUT);      // Right Speed
  pinMode (6, OUTPUT);      // MOSFET gate
  Serial.begin(9600);
}


void loop() {
  if (!(tmrpcm.isPlaying())){
    tmrpcm.disable();
    }
  usvalue = analogRead(A2);
  if (usvalue < 15){
    usvalue = 0;
    }
  magnetvalue = analogRead(A3);
  if (micros() - Time > 200000){
    Time = micros();
    maxim = 0;
    unsigned short list[8] = {0,0,0,0,0,0,0,0};
    count = 0;
    up = false;
    down = false;
    if (x){
      irfreq = 0;
      ir = 0;
      x = false;
      while(micros() - Time < 50000){
        value = analogRead(A0);
        if (value > 300){
          down = true;
          }
        if (!up && down && value < 300){
          ir = 300-value;
          startTime = micros();
          up = true;
          down = false;
          }
        if (up && down && value < 300){
          irperiod = micros()-startTime;
          //Serial.println(String(irperiod));
          /*while (value <= maxim){
            value = analogRead(A0);
            if (value < maxim){
              maxim = value;
              }
            }*/
          ir = 300-value;
          if (irperiod<2000){
            irfreq = 607;
            }
          else{
            irfreq = 421;
            }
          break;
        }
      }
      digitalWrite (6, LOW);
    }
    else if (y){
      rfreq = 0;
      rada = 0;
      y = false;
      while(micros() - Time < 50000 && count<8){
        value = analogRead(A1);
        //Serial.println(value);
        if (value < 490){
          down = true;
          }
        if (!up && down && value > 490){
          startTime = micros();
          up = true;
          down = false;
          }
        if (up && down && value > 490){
          rperiod = micros()-startTime;
          //Serial.println(String(rperiod));
           while (value >= maxim){
            value = analogRead(A1);
            if (value > maxim){
              maxim = value;
              }
            }
           list[count] = maxim;
           count++;
        }
      }
        for (int i = 0; i<count; i++){
          rada+=list[i];
          }
          rada=rada/(count+1);
         if (ir>0){
          ir-=355;
         }
         //Serial.println("IR: "+ String(1000000/(irperiod))+" " + String(ir));
        //Serial.println("RADA: "+ String(1000000/(rperiod))+" " + String(rada));
   digitalWrite (6, HIGH);
   }
    else{
      rfreq = 0;
      radb = 0;
      x = true;
      y = true;
      while(micros() - Time < 50000 && count<8){
        value = analogRead(A1);
        //Serial.println(value);
        if (value < 490){
          down = true;
          }
        if (!up && down && value > 490){
          startTime = micros();
          up = true;
          down = false;
          }
        if (up && down && value > 490){
          //Serial.println(String(rperiod));
          rperiod = micros()-startTime;
           while (value >= maxim){
            value = analogRead(A1);
            if (value > maxim){
              maxim = value;
              }
            }
           list[count] = maxim;
           count++;
        }
      }
        for (int i = 0; i<count; i++){
          radb+=list[i];
          }
          radb=radb/(count+1);
        //Serial.println("RADB: "+ String(1000000/(rperiod))+" " + String(radb));

    }
    Time = micros();
  }
  if (!(rada == 0 && radb == 0)){
    if (rada<radb){
      carrierfreq = 67;
      rad = radb-350;
      }
    else{
      carrierfreq = 103;
      rad = rada-350;
      }
    if (rperiod<6100){
      rfreq = 239;
      }
    else{
      rfreq = 151;
      }
  }
   else{
    rad = 0;
    rfreq = 0;
    carrierfreq = 0;
    }
  //Serial.print("  IR ->> Frequency =" + String(irfreq) + "  Value =" + String(ir));
  //Serial.print("   Radio ->> Frequency =" + String(rfreq) + "  Value =" + String(rada));

    if (millis()-infoTime>500){
      infoTime = millis();
      Serial.print("$ " + String(carrierfreq)+" "+String(rfreq)+" "+String(rad)+" "+String(irfreq)+" "+String(ir)+" "+String(usvalue)+" "+String(magnetvalue) + " ");
      }

    if (Serial.available()) {
      char c = Serial.read();
      //Serial.print(c);
  
      if (c=='s'){
        r = false;
        l = false;
        }
      if ((c=='F') || (dire == 1 && c=='s')) {
        if (l == true){
          digitalWrite(2, LOW);
          digitalWrite(7, HIGH);
        }
        else if (r == true) {
          digitalWrite(2, HIGH);
          digitalWrite(7, LOW);
        }
        
        else{
        digitalWrite(2, HIGH);
        digitalWrite(7, HIGH);
        }
        analogWrite(3, velocity);
        analogWrite(5, velocity);
        dire = 1;
        tmrpcm.play("bing.wav");
      }
      if ((c=='B') || (dire == 2 && c=='s')) {
        if (l == true){
          digitalWrite(2, HIGH);
          digitalWrite(7, LOW);
        }
        else if (r == true) {
          digitalWrite(2, LOW);
          digitalWrite(7, HIGH);
        }
        else{
        digitalWrite(2, LOW);
        digitalWrite(7, LOW);
        }
        analogWrite(3, velocity);
        analogWrite(5, velocity);
        dire = 2;
        tmrpcm.play("bong.wav");
      }
      if (c=='L') {
        l = true;
        if (dire == 1) {
          digitalWrite(2, LOW);
        }
        if (dire == 2) {
          digitalWrite(2, HIGH);
        }
        /*if (dire == 0) {
          l = true;
          }*/
      }
      if (c=='R') {
        r = true;
        if (dire == 1) {
          digitalWrite(7, LOW);
        }
        if (dire == 2) {
          digitalWrite(7, HIGH);
        }
        /*if (dire == 0) {
          r = true;
          }*/
      }
      if (c=='S') {
        analogWrite(3, 0);
        analogWrite(5, 0);
        dire = 0;
      }
      if (c=='1') {
        velocity = 100;
        tmrpcm.play("off.wav");
      }
      if (c=='2') {
        tmrpcm.play("tuff.wav");
        velocity = 150;
      }
      if (c=='3') {
        velocity = 200;
        tmrpcm.play("thanks.wav");
      }
      if (c=='4') {
        velocity = 255;
        tmrpcm.play("stamina.wav");
      }
      if (c=='s' && dire == 0){
        r = false;
        l = false;
        }
      /*if (c=='g') {
        //tmrpcm.play("Gaborite.wav");
      }
      if (c=='n') {
        //tmrpcm.play("Nucinkisite.wav");
      }
      if (c=='d') {
        //tmrpcm.play("Durranium.wav");
      }
      if (c=='b') {
        //tmrpcm.play("Brookesite.wav");
      }
      if (c=='c') {
        //tmrpcm.play("Chengtium.wav");
      }
      if (c=='y') {
        //tmrpcm.play("Yeatmanine.wav");
      }*/
    }
  }



