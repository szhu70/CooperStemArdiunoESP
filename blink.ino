int state = LOW;

unsigned long m;
int count = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(13, OUTPUT);

  m = millis();

}

void loop() {
  if (m - count > 500){
    if (state == LOW) state = HIGH;
        else state = LOW;
    digitalWrite(13, state);
    count = millis();

  } else {
    m = millis();
  }
}
