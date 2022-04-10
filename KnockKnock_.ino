#define DEBUG

const byte sensorPin=A0;
const byte ledPin=13;
const byte buzzerPin=12;
//const byte lockPin=4;
//shave and hair cut,two bites
//const int numKnocksInPattern=7;
//const int secretKnockPattern[numKnockInPattern-1]={512,256,256,512,1024,512};

//happy birthday to you
//const int numKnockInPattern=6;
//const int secretKnockPattern[numKnockInPattern-1]={683,341,1024,1024,1024};

//in the air tonight drum solo
const int numKnocksInPattern=10;
const int secretKnockPattern[numKnocksInPattern-1]={512,1024,512,1024,512,1024,512,1024,1024};

const int sensorThreshold=15;
const int knockErrorTolerance=128;
const int averageKnockErrorTolerance=128;
const int minGapBetweenKnocks=125;
const int maxGapBetweenKnocks=1500;

int baseReading=512;
int inputPattern[numKnocksInPattern-1];

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
#ifdef DEBUG
Serial.println(F("Entering Setup"));
#endif

#ifdef DEBUG
Serial.println(F("setting pin modes"));
delay(500);
#endif
pinMode(sensorPin,INPUT);
pinMode(ledPin,OUTPUT);
pinMode(buzzerPin,OUTPUT);
//pinMode(lockPin,OUTPUT);

#ifdef DEBUG
Serial.println(F("playing back knock pattern"));
delay(500);
#endif
playbackKnockPattern(secretKnockPattern,500);
 #ifdef DEBUG 
 Serial.println(F("calibrating sensor"));
 #endif

 int numSamples=10;
 int sumReadings=0;
 for(int i=0;i<numSamples;i++){
  sumReadings+=analogRead(sensorPin);
  delay(100);
 }

float avgReading=(float)sumReadings/numSamples;
baseReading=avgReading;

#ifdef DEBUG
Serial.print(F("Sensor base reading:"));
Serial.println(avgReading);
Serial.println(F("setup complete"));
#endif


}

void loop() {
  // put your main code here, to run repeatedly:
    if(knockDetected()){
      listenToKnockPattern();
      #ifdef DEBUG
      Serial.print(F("timings before normalisation:"));
      printPattern(inputPattern);
      #endif

      normaliseKnockPattern(inputPattern);
      #ifdef DEBUG
      Serial.print(F("timings after normalisation:"));
      printPattern(inputPattern);
      Serial.print(F("Correct normalised timings:"));
      printPattern(secretKnockPattern);
      #endif

      if(comparePattern(inputPattern,secretKnockPattern)){
        onPuzzleSolved();
      }
      else{
        onIncorrectInput();
      }
    }
      










}

bool knockDetected(){
int knockSensorValue=analogRead(sensorPin);
if(abs(knockSensorValue-baseReading)>sensorThreshold){
return true;
}
else{
  return false;
}

}

bool comparePattern(int inputPattern[],int secretKnockPattern[]){
  for(int i=0;i<numKnocksInPattern-1;i++){
    if(inputPattern[i]==0){
      #ifdef DEBUG 
      Serial.print(F("TEST #1 FAILED. input pattern had"));
      Serial.print(i+1);
      Serial.print(F("knocks.Secret pattern has"));
      Serial.print(numKnocksInPattern);
      Serial.println(F(" knocks."));
      #endif
      return false;
    }

  }
  int totalDelta=0;
  for(int i=0;i<numKnocksInPattern-1;i++){
    int knockDelta=abs(inputPattern[i]-secretKnockPattern[i]);
    if(knockDelta>knockErrorTolerance){
      #ifdef DEBUG
      Serial.print(F("TEST #2 FAILED. input pattern had delay after knock#"));
      Serial.print(i+1);
      Serial.print(F(" of "));
      Serial.print(inputPattern[i]);
      Serial.print(F(". Secret pattern has "));
      Serial.print(secretKnockPattern[i]);
      Serial.println(F("."));
      #endif
      return false;
    }
    totalDelta+=knockDelta;
  }
  if(totalDelta/numKnocksInPattern>averageKnockErrorTolerance){
    #ifdef DEBUG
     Serial.print(F("TEST #3 FAILED. input pattern had average timing error of "));
     Serial.print(totalDelta/numKnocksInPattern);
     Serial.print(F(".Secret pattern had"));
     Serial.print(averageKnockErrorTolerance);
     Serial.println(F("ms."));
     #endif
     return false;
  }
   return true;

}


void listenToKnockPattern(){
  int lastKnockTime=millis();
  int now=millis();
  #ifdef DEBUG
  Serial.print(F("Knock"));
  #endif

  digitalWrite(ledPin,HIGH);
  knockDelay();
  digitalWrite(ledPin,LOW);

  memset(inputPattern,0,sizeof(inputPattern));

  int currentKnockNumber=1;

  while((now-lastKnockTime<maxGapBetweenKnocks) && (currentKnockNumber<numKnocksInPattern)){
    now=millis();
    if(knockDetected()){
      #ifdef DEBUG 
      Serial.print(F(",Knock"));
      #endif

      inputPattern[currentKnockNumber-1]=now-lastKnockTime;
      currentKnockNumber++;
      lastKnockTime=now;

      digitalWrite(ledPin,HIGH);
      knockDelay();
      digitalWrite(ledPin,LOW);
    }

  }
#ifdef DEBUG
Serial.println();
#endif

}






void normaliseKnockPattern(int knockPattern[]){
int maxKnockInterval=0;
for(int i=0;i<numKnocksInPattern-1;i++){
  if(knockPattern[i]>maxKnockInterval){maxKnockInterval=knockPattern[i];}
}
for(int i=0;i<numKnocksInPattern-1;i++){
  knockPattern[i]=map(knockPattern[i],0,maxKnockInterval,0,1024);
  } 
}


void playbackKnockPattern(int knockPattern[],int maxKnockInterval){
int blinkTime=50;
digitalWrite(ledPin,HIGH);
tone(buzzerPin,784,blinkTime);
delay(blinkTime);
digitalWrite(ledPin,LOW);
noTone(buzzerPin);

for(int i=0;i<numKnocksInPattern-1;i++){
    if(secretKnockPattern[i]>0){
        int delayTime=map(secretKnockPattern[i],0,1024,0,maxKnockInterval);
        delayTime=max(delayTime-blinkTime,0);
        delay(delayTime);
        digitalWrite(ledPin,HIGH);
        tone(buzzerPin,784,blinkTime);
        delay(blinkTime);
        digitalWrite(ledPin,LOW);
        noTone(buzzerPin);
      }
    }
}

void onPuzzleSolved(){
  #ifdef DEBUG 
  Serial.println(F("correct pattern entered!"));
  #endif

  tone(buzzerPin,784,200);
  delay(200);
  tone(buzzerPin,1047,200);
}

void onIncorrectInput(){
  #ifdef DEBUG
  Serial.println(F(" entered pattern did not match!"));
  #endif

  tone(buzzerPin,500,200);
  delay(200);
  tone(buzzerPin,400,200);
  for(int i=0;i<5;i++){
    digitalWrite(ledPin,HIGH);
    delay(20);
    digitalWrite(ledPin,LOW);
    delay(20);
  }

}


void printPattern(int pattern[]){
  for(int i=0;i<numKnocksInPattern-1;i++){
    Serial.print(pattern[i]);
    if(i<numKnocksInPattern-2){
      Serial.print(",");
    }
  }
  Serial.println();
}

void knockDelay(){
  int interations=(minGapBetweenKnocks/10);
  for(int i=0;i<interations;i++){
    delay(10);
    analogRead(sensorPin);
  }
  
}
