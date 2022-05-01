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

void it(int i){
	myServo.write(i);
	distance = calculateDistance();

	//Serial.println(sizeof(i) + sizeof(distance));
	Serial.print('a');
	Serial.print(i);
	Serial.print(" ");
	Serial.print('d');
	Serial.print(distance);
	Serial.print("\n");



	char toRead = Serial.read(); 
	if(toRead == 'k'){
		Serial.flush();
		exit(0);
	}
	else{
		if(toRead == 's'){
			toRead = 97;
			while(Serial.read() != 't'){
				delay(100);
				Serial.write('.');
			}
			toRead = 97;
		}
	}
}

void loop() {
	char toRead = 97;

	while(true){
		for(int i=0;i<=180;i++){
			delay(5);
			it(i);  
		}

		for(int i=180;i>=0;i--){
			delay(5);
			it(i);
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
	//timeout is 15ms
	duration = pulseIn(echoPin, HIGH, 15000);

	distance= duration*0.034/2;
	return distance;
}
