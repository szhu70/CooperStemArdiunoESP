#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define JOYX A0
#define JOYY A1
#define BUTTON 8

const byte WIDTH = 5;
const byte HEIGHT = 8;
const byte MAX_SNAKE = WIDTH * HEIGHT;

struct Point {
  int x;
  int y;
};

Point snake[MAX_SNAKE];
Point food;

byte snakeLength;

int dx;
int dy;

bool gameOver;

byte bitmap[8];

unsigned long lastMove = 0;
const unsigned long speed = 500;

void spawnFood() {
  if (snakeLength >= MAX_SNAKE) return;
  while (true) {
    food.x = random(WIDTH);
    food.y = random(HEIGHT);
    bool used = false;
    for (byte i = 0; i < snakeLength; i++) {
      if (snake[i].x == food.x && snake[i].y == food.y) {
        used = true;
        break;
      }
    }
    if (!used)
      return;
  }
}

void resetGame() {
  snakeLength = 3;
  snake[0] = {2, 4};
  snake[1] = {1, 4};
  snake[2] = {0, 4};

  dx = 1;
  dy = 0;

  gameOver = false;

  lastMove = millis();

  spawnFood();

  lcd.clear();
}

void setup() {
  pinMode(BUTTON, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  randomSeed(analogRead(A3));

  resetGame();
}

void readJoystick() {
  int x = analogRead(JOYX);
  int y = analogRead(JOYY);
  if (x < 300 && dx != 1) {
    dx = -1;
    dy = 0;
  }
  else if (x > 700 && dx != -1) {
    dx = 1;
    dy = 0;
  }
  else if (y < 300 && dy != 1) {
    dx = 0;
    dy = -1;
  }
  else if (y > 700 && dy != -1) {
    dx = 0;
    dy = 1;
  }
}

void moveSnake() {
  Point head = snake[0];
  head.x += dx;
  head.y += dy;

  if (head.x < 0 || head.x >= WIDTH ||
      head.y < 0 || head.y >= HEIGHT) {
    gameOver = true;
    return;
  }

  for (byte i = 0; i < snakeLength; i++) {
    if (snake[i].x == head.x &&
        snake[i].y == head.y) {
      gameOver = true;
      return;
    }
  }

  for (int i = snakeLength-1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }

  snake[0] = head;

  if (head.x == food.x && head.y == food.y) {
    if (snakeLength < MAX_SNAKE)
      snakeLength++;
    if (snakeLength == MAX_SNAKE) {
      gameOver = true;
      return;
    }
    spawnFood();
  }
}

void drawGame() {
  memset(bitmap, 0, sizeof(bitmap)); // resets bitmap
  bitmap[food.y] |= (1 << (4 - food.x)); // binary operators, binary or and turning specific indicies on.

  for (byte i = 0; i < snakeLength; i++) {
    bitmap[snake[i].y] |= (1 << (4 - snake[i].x));
  }

  lcd.createChar(0, bitmap);
  lcd.setCursor(0, 0);
  lcd.write(byte(0));
}

void showGameOver() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over");
  lcd.setCursor(0, 1);
  lcd.print("Press Button");

  while (digitalRead(BUTTON) == HIGH) {
    delay(10);
  }

  delay(200);

  while (digitalRead(BUTTON) == LOW) {
    delay(10);
  }
  resetGame();
  drawGame();
}

void loop() {
  if (gameOver) {
    showGameOver();
    return;
  }

  readJoystick();

  if (millis() - lastMove >= speed) {
    lastMove = millis();
    moveSnake();
    if (!gameOver)
      drawGame();
  }
}