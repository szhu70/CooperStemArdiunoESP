// --- Game Track Settings ---
const int numLeds = 7;
const int ledPins[numLeds] = {7, 11, 13, 10, 6, A0, A1};
const int targetIndex = 3; // Index 3 is Pin 10 (Your target light)

// --- Player Settings ---
const int p1Button = 2;
const int p2Button = 12;
const int p1ScoreLed = 3;  // PWM Pin (Blue Score)
const int p2ScoreLed = 9;  // PWM Pin (Red Score)

int p1Score = 0;
int p2Score = 0;

// Perceptual brightness lookup table (0, 1, 2, 3 points)
const int brightnessLevels[4] = {0, 15, 75, 255};

// --- State Machine Configuration ---
enum GameState { STARTUP, PLAYING, ROUND_OVER, GAME_OVER };
GameState currentState = STARTUP;

unsigned long stateTimer = 0;     
unsigned long lastMoveTime = 0;   
int moveSpeed = 150;              // Will be randomized between 100 and 250
int currentLed = 0;               // Will be randomized between 0 and 6

// Input tracking
bool p1LastState = HIGH;
bool p2LastState = HIGH;

// Win animation helpers
int flashCounter = 0;
bool flashState = false;

void setup() {
  // Generate true randomness by reading the electrical noise on an unused pin
  randomSeed(analogRead(A2)); 

  // Initialize track pins and force them LOW immediately
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  
  pinMode(p1ScoreLed, OUTPUT);
  pinMode(p2ScoreLed, OUTPUT);
  
  pinMode(p1Button, INPUT_PULLUP);
  pinMode(p2Button, INPUT_PULLUP);
  
  // Clear scores and update score LEDs to completely off
  p1Score = 0;
  p2Score = 0;
  updateScores(); 
  
  // Prime the button history to actual hardware state at boot
  p1LastState = digitalRead(p1Button);
  p2LastState = digitalRead(p2Button);
  
  // Set our state machine to startup mode
  currentState = STARTUP;
  stateTimer = millis();
}

void loop() {
  // 1. ALWAYS READ INPUTS EVERY FRAME (Edge Detection)
  bool p1State = digitalRead(p1Button);
  bool p1Pressed = (p1State == LOW && p1LastState == HIGH);
  p1LastState = p1State;

  bool p2State = digitalRead(p2Button);
  bool p2Pressed = (p2State == LOW && p2LastState == HIGH);
  p2LastState = p2State;

  // 2. RUN THE GAME STATE MACHINE
  switch (currentState) {
    
    case STARTUP:
      for (int i = 0; i < numLeds; i++) {
        digitalWrite(ledPins[i], LOW);
      }
      
      // Once 1.5 seconds passes, start the game fairly!
      if (millis() - stateTimer > 1500) {
        currentState = PLAYING;
        
        // RANDOMIZE ROUND 1
        currentLed = random(0, numLeds); // Random LED from 0 to 6
        moveSpeed = random(100, 251);    // Random speed from 100ms to 250ms
        
        digitalWrite(ledPins[currentLed], HIGH);
        lastMoveTime = millis();
      }
      break;

    case PLAYING:
      // Move the light across the track linearly
      if (millis() - lastMoveTime > moveSpeed) {
        digitalWrite(ledPins[currentLed], LOW); 
        currentLed = (currentLed + 1) % numLeds; 
        digitalWrite(ledPins[currentLed], HIGH); 
        lastMoveTime = millis();
      }

      // Check Player 1 Input
      if (p1Pressed) {
        if (currentLed == targetIndex) {
          p1Score++; // Hit!
        } else {
          p2Score++; // Miss! Penalty to Red
        }
        updateScores();
        triggerRoundOver();
      } 
      // Check Player 2 Input
      else if (p2Pressed) {
        if (currentLed == targetIndex) {
          p2Score++; // Hit!
        } else {
          p1Score++; // Miss! Penalty to Blue
        }
        updateScores();
        triggerRoundOver();
      }
      break;

    case ROUND_OVER:
      // Holds the round-over state for 1 second without freezing the processor
      if (millis() - stateTimer > 1000) {
        if (p1Score >= 3 || p2Score >= 3) {
          currentState = GAME_OVER;
          stateTimer = millis();
          flashCounter = 0;
          flashState = false;
        } else {
          currentState = PLAYING;
          
          // RANDOMIZE NEXT ROUND
          currentLed = random(0, numLeds);
          moveSpeed = random(100, 251); 
          
          digitalWrite(ledPins[currentLed], HIGH);
          lastMoveTime = millis();
        }
      }
      break;

    case GAME_OVER:
      // Non-blocking win animation: Flash the winner's score LED
      if (millis() - stateTimer > 250) {
        stateTimer = millis();
        int winnerPin = (p1Score >= 3) ? p1ScoreLed : p2ScoreLed;
        
        flashState = !flashState;
        analogWrite(winnerPin, flashState ? 255 : 0);
        flashCounter++;

        // Reset the entire game cabinet after 5 complete flashes
        if (flashCounter >= 10) { 
          p1Score = 0;
          p2Score = 0;
          updateScores();
          currentState = STARTUP;
          stateTimer = millis();
        }
      }
      break;
  }
}

// Safely updates the score LED brightness maps
void updateScores() {
  analogWrite(p1ScoreLed, brightnessLevels[constrain(p1Score, 0, 3)]);
  analogWrite(p2ScoreLed, brightnessLevels[constrain(p2Score, 0, 3)]);
}

// Clears track cleanly and shifts the state machine to Round Over
void triggerRoundOver() {
  currentState = ROUND_OVER;
  stateTimer = millis();
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}