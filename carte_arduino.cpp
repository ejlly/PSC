#include <Servo.h>

const int trigPin = 11;
const int echoPin = 12;

long duration;
int distance;

Servo myServo; 
void setup() {
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  Serial.begin(9600);
  myServo.attach(9); 

}
void loop() {
  
  for(int k(0); k<3; k++){
  for(int i=0;i<=100;i++){  
  
  myServo.write(i);
  delay(30);
  distance = calculateDistance();

  //Serial.println(sizeof(i) + sizeof(distance));
  Serial.print(i);
  Serial.print(" ");
  Serial.print(distance);
  Serial.print("\n");
  }

  for(int i=100;i>=0;i--){
  
  myServo.write(i);
  delay(30);
  distance = calculateDistance();
  Serial.print(i);
  Serial.print(" ");
  Serial.print(distance);
  Serial.print("\n");
  
  }
  } 

  exit(0);
  
}

int calculateDistance(){ 
  
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH); 
  
  distance= duration*0.034/2;
  return distance;
}
