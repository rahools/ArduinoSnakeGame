#include <LedControl.h>

///////////////////////////////// HARDWARE PINS AND PARAMETERS

// Joystick PINs
#define VRX     A0
#define VRY     A1
#define SW      2

// Display PINs
#define CLK     8
#define CS      9
#define DIN     10

// Matrix Size
#define SIZE    8
#define ADVANCE_DELAY 20

///////////////////////////////// SOFTWARE PARAMETERS

// Max Size of Snake = 8 X 8
// 2D Array to store x, y Positions
int snake[SIZE*SIZE][2];

// Store Snake Lenght
int length;

// Food x, y Postion
int food[2];

// x, y Velocity of Snake
int v[2];

// Game Stat bool
bool is_game_over = false;

// Game Pause bool
bool is_game_pause = false;

long current_time;
long prev_advance;
int blink_count;
const short messageSpeed = 5;

// Initialize LED Matrix
LedControl lc = LedControl(DIN, CLK, CS, 1);

///////////////////////////////// HELPER FUNCTIONS

// Initialize Game
void init_game() {
    prev_advance = current_time = 0;
    blink_count = 3;

    // Initial Snake Lenght
    int half = SIZE / 2;
    length = SIZE / 3;

    // Assign Initial Snake x, y Postion
    for (int i = 0; i < length; i++) {
        snake[i][0] = half - 1;
        snake[i][1] = half + i;
    }

    // Assign Initial Food x, y Position
    food[0] = half + 1;
    food[1] = half - 1;

    // Assign Initial Snake Velocity
    v[0] = 0;
    v[1] = -1;
}

// Render Game State on LED Matrix
void render() {
    for (int i = 0; i < length; i++)
        lc.setLed(0, snake[i][0], snake[i][1], 1);
    lc.setLed(0, food[0], food[1], 1);
}

// Clear LED Matrix
void clearScreen() {
    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            lc.setLed(0, x, y, 0);
        }
    }
}

// Moves The Snake Forward
// Returns True, If The Game Is Over
bool advance() {
    int head[2] = {snake[0][0] + v[0], snake[0][1] + v[1]};

    // If Snake Goes Out of Bounds, For x Position
    if (head[0] < 0 || head[0] >= SIZE) {
        delay(1000);
        showGameOverMessage();
        return true;
    }

    // If Snake Goes Out of Bounds, For y Position
    if (head[1] < 0 || head[1] >= SIZE) {
        delay(1000);
        showGameOverMessage();
        return true;
    }

    // Detect Snake Eating Itself
    for (int i = 0; i < length; i++) {
        if (snake[i][0] == head[0] && snake[i][1] == head[1]) {
            delay(1000);
            showGameOverMessage();
            return true;
        }
    }

    // Detect if Snake eats Food
    bool grow = (head[0] == food[0] && head[1] == food[1]);

    // Increase Snkae length if it Eats Food
    if (grow) {
        // Grow The Snake
        length++;
        // Place Next Food
        randomSeed(current_time);    
        food[0] = random(SIZE);
        food[1] = random(SIZE);
    }

    // Move Snake Body
    for (int i = length - 1; i >= 0; i--) {
        snake[i + 1][0] = snake[i][0];
        snake[i + 1][1] = snake[i][1];
    }

    // Move Snake Head According to velocity
    snake[0][0] += v[0];
    snake[0][1] += v[1];

    return false;
}

///////////////////////////////// SETUP FUNC

void setup() {
    // PINs Setup for Joystick
    pinMode(SW, INPUT_PULLUP);
    pinMode(VRX, INPUT);
    pinMode(VRY, INPUT);

    // Setup Joystick Switch Interrupt
    attachInterrupt(digitalPinToInterrupt(SW), gameFunc, RISING);

    // Setup LED Matrix
    lc.shutdown(0, false);
    lc.setIntensity(0, 8);

    init_game();
    render();
}

///////////////////////////////// LOOP FUNCTIONS

void loop() {
    if (!is_game_over && !is_game_pause) {
        clearScreen();
        render();

        if (current_time - prev_advance > ADVANCE_DELAY) {
            is_game_over = advance();
            prev_advance = current_time;    
        }

        else {
            // Blink to Indicate Game Start
            while (blink_count > 0) {
                clearScreen();
                delay(300);
                render();
                delay(300);
                blink_count--;
            }
        }

        readControls();
        current_time++;
    } 
    
}

///////////////////////////////// MORE HELPER FUNCTIONS

// Restart or Pause the Game Based on Game State
void gameFunc() {  
    // Restarts Game
    if(is_game_pause == false && is_game_over == true){
        init_game();
        is_game_over = false;
    }

    // Pauses Game
    else if(is_game_pause == false && is_game_over == false){
        is_game_pause = true;
    }

    // Un-Pauses Game
    else if(is_game_pause == true && is_game_over == false){
        prev_advance = current_time = 0;
        blink_count = 3;
        is_game_pause = false;
    }
}

// Get Movement Info From Joystick
void readControls() {
    int dx = map(analogRead(VRX), 0, 906, 2, -2);
    int dy = map(analogRead(VRY), 0, 906, -2, 2);

    // Get dx, dy
    //      1 : if Joystick positive
    //     -1 : if Joystick negetive 
    if (dx != 0) {dx = dx / abs(dx);}
    if (dy != 0) {dy = dy / abs(dy);}

    // if there is dy and x velocity not equal 0
    //      Stop x Movement
    //      Update y Movement
    if (dy != 0 && v[0] != 0) {
        v[0] = 0;
        v[1] = dy;    
    }
    
    // if there is dx and y velocity not equal 0
    //      Stop y Movement
    //      Update x Movement
    if (dx != 0 && v[1] != 0) {
        v[0] = dx;
        v[1] = 0;
    }
}

// HardCodded Game Over Message
const PROGMEM bool gameOverMessage[8][90] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,1,1,0,0,0,1,1,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,0,0,1,1,0,0,1,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,1,0,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,1,1,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,1,1,0,1,0,1,1,0,0,1,1,1,1,1,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,0,1,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,1,1,1,1,0,0,0,1,1,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,0,0,0,0,1,1,1,1,1,1,0,0,1,1,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0}
}; 

// Display Gmae Over Message
void showGameOverMessage() {
    for (int d = 0; d < sizeof(gameOverMessage[0]) - 7; d++) {
        for (int col = 0; col < 8; col++) {
            delay(messageSpeed);
            for (int row = 0; row < 8; row++) {
                // this reads the byte from the PROGMEM and displays it on the screen
                lc.setLed(0, row, col, pgm_read_byte(&(gameOverMessage[row][col + d])));
            }
        }
    }
}
