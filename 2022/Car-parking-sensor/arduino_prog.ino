long duration;
float distance;

void setup() {

  pinMode(13,OUTPUT); // RED LED PIN
  pinMode(12,OUTPUT); // GREEN LED PIN

  pinMode(11,INPUT); // ECHO PIN
  pinMode(10,OUTPUT); // TRIGGER PIN
  
  Serial.begin(9600);
}

void loop() {

  digitalWrite(10,LOW);
  delayMicroseconds(2);

  digitalWrite(10,HIGH);
  delayMicroseconds(10);
  digitalWrite(10,LOW);

  duration = pulseIn(11,HIGH);
  distance = duration*0.034/2; // 0.034 [cm/mikrosec]
  
  Serial.print("Distance: ");
  if(distance >= 50 || distance <= 4){
    Serial.println("Out of range");
    }
   else {
    Serial.print(distance);
    Serial.println(" [cm]");
    delay(300);
    }

    if(distance <= 17){
    digitalWrite(13,HIGH);
    digitalWrite(12,LOW);
    tone(2,2400);
    delay(200);
    digitalWrite(13,LOW);
    digitalWrite(12,HIGH);
    delay(200);
    noTone(2);
    digitalWrite(12,LOW);
   }
    
}