#include <Servo.h>
#include <math.h>

#include <DHT.h>

Servo servo;

bool dark = true;

// int tolerance = 200;
//   unsigned long x = 0;
//   unsigned long y = 0;

DHT dht(7, DHT11);




void setup(){
  Serial.begin(9600);
  servo.attach(9);

  dht.begin();

}

// void loop(){
  // servo.write(0);
  // x = analogRead(A0);
  // y = analogRead(A1);

  // Serial.println((((atan2(y-512,x-512) * 180 / 3.1415) + 180) / 2));

  // Serial.println(analogRead(A4));

  // if (abs(x-512.0) > tolerance || abs(y-512.0) > tolerance){
  //   servo.write(constrain((((atan2(y-512.0,x-512.0) * 180 / 3.1415) + 180) / 2), 0, 180));
  // }

  // delay(250);

// }

// void loop(){
//   Serial.println(analogRead(A4));

//   if(analogRead(A5) > 40) dark = false;
//     else dark = true;

//   if (dark){
//     if (analogRead(A4) > 40){
//       servo.write(35);
//       delay(200);
//     } else {
//       servo.write(0);
//     }
//   } else {
//     if (analogRead(A4) < 40){
//       servo.write(25);
//       delay(75);
//     } else {
//       servo.write(0);
//     }
//   }
// }

void loop(){
  delay(2000);

  Serial.print("% humiditiy: ");
  Serial.println(dht.readHumidity());
  Serial.print("temperature: ");
  Serial.println(dht.readTemperature(true));
}
