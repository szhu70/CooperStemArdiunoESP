const int p1Button = 7;
const int p2Button = 8;

const int goLED = 10;

const int p1LEDs[3] = {2, 3, 4};
const int p2LEDs[3] = {13, 12, 11};

int p1Score = 0;
int p2Score = 0;


void setup() {

  pinMode(p1Button, INPUT_PULLUP); // internal resistors
  pinMode(p2Button, INPUT_PULLUP);

  pinMode(goLED, OUTPUT);

  for (int i = 0; i < 3; i++) {
    pinMode(p1LEDs[i], OUTPUT);
    pinMode(p2LEDs[i], OUTPUT);
  }

  randomSeed(analogRead(A0)); // non connected slots provide random electrical noise, we use this for random.

  resetBoard();
}


void loop() {

  // Wait for both players to release buttons
  waitForRelease();

  delay(500);


  // Random waiting time before GO
  unsigned long waitTime = random(2000, 6001);
  unsigned long startTime = millis();


  // False start detection
  while (millis() - startTime < waitTime) {

    if (digitalRead(p1Button) == LOW) {

      awardPoint(2);

      if (checkWinner()) {
        return;
      }

      endRound();
      return;
    }

    if (digitalRead(p2Button) == LOW) {

      awardPoint(1);

      if (checkWinner()) {
        return;
      }

      endRound();
      return;
    }
  }


  // GO signal
  analogWrite(goLED, 100);


  // Reaction phase
  while (true) {

    if (digitalRead(p1Button) == LOW) {

      awardPoint(1);
      break;
    }


    if (digitalRead(p2Button) == LOW) {

      awardPoint(2);
      break;
    }
  }


  digitalWrite(goLED, LOW);


  if (checkWinner()) {
    return;
  }


  endRound();
}


void awardPoint(int player) {

  if (player == 1 && p1Score < 3) {

    digitalWrite(p1LEDs[p1Score], HIGH);
    p1Score++;
  }


  if (player == 2 && p2Score < 3) {

    digitalWrite(p2LEDs[p2Score], HIGH);
    p2Score++;
  }
}


bool checkWinner() {

  if (p1Score >= 3) {

    celebrate(1);
    resetGame();
    return true;
  }


  if (p2Score >= 3) {

    celebrate(2);
    resetGame();
    return true;
  }


  return false;
}


void celebrate(int player) {


  for (int j = 0; j < 8; j++) {


    for (int i = 0; i < 3; i++) {

      if (player == 1) {
        digitalWrite(p1LEDs[i], HIGH);
      }
      else {
        digitalWrite(p2LEDs[i], HIGH);
      }
    }


    delay(200);


    for (int i = 0; i < 3; i++) {

      if (player == 1) {
        digitalWrite(p1LEDs[i], LOW);
      }
      else {
        digitalWrite(p2LEDs[i], LOW);
      }
    }


    delay(200);
  }
}


void endRound() {

  delay(1000);

  waitForRelease();

  delay(500);
}

void waitForRelease() {

  while (digitalRead(p1Button) == LOW ||
         digitalRead(p2Button) == LOW) {
  }
}

void resetGame() {

  delay(1000);

  p1Score = 0;
  p2Score = 0;

  resetBoard();

  delay(1000);
}

void resetBoard() {

  digitalWrite(goLED, LOW);


  for (int i = 0; i < 3; i++) {

    digitalWrite(p1LEDs[i], LOW);
    digitalWrite(p2LEDs[i], LOW);
  }
}