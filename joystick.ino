int x = map(analogRead(A0), 0, 1023, 0, 255);
int y = map(analogRead(A1), 0, 1023, 0, 255);


void setup(){
  Serial.begin(9600);
}

void loop(){
  x = map(abs(analogRead(A0)-512), 0, 512, 0, 255);
  y = map(abs(analogRead(A1)-512), 0, 512, 0, 255);

  // int magnitude = analogRead();

  if (analogRead(A0) >= 512 && abs(analogRead(A0)-512) > 30) analogWrite(5, x);
    else digitalWrite(5,0);
  if (analogRead(A0) < 512 && abs(analogRead(A0)-512) > 30) analogWrite(9,x);
      else digitalWrite(9,0);

  if (analogRead(A1) >= 512 && abs(analogRead(A1)-512) > 30) analogWrite(6,y);
      else digitalWrite(6,0);

  if (analogRead(A1) < 512 && abs(analogRead(A1)-512) > 30) analogWrite(3,y);
      else digitalWrite(3,0);

}
