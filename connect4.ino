// Connect four game using NeoPixel library
// (c) Anav Mehta
// http://3dwaffles.blogspot.com

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            3
int analogInputPinX   = 3;
int analogInputPinY   = 5;
int digitalInputPin  = 2;
#define PINX 3
#define PINY 5
#define CONFIRM 2
int wait = 1000; 
long analogInputValX;
long analogInputValY;
int digitalInputVal;
//#define UP 6
//#define DOWN 5
//#define RIGHT 7
//#define LEFT 4
//#define CONFIRM 3
#define K 4
#define INPROGRESS (-1)


// How many NeoPixels are attached to the Arduino?
#define SIDE 8
#define NUMPIXELS      8*8
#define SIZE           NUMPIXELS
/* For recording cursor position */
static int csr_x_c = 1; /* current location of cursor in x-axis */
static int csr_y_c = 1; /* current location of cursor in y-axis */
static int csr_x_p = 1; /* last location of cursor in x-axis */
static int csr_y_p = 1; /* last location of cursor in y-axis */
static int steps = 0; /* keep tracking of player’s turn */
static const int player_1 = 1; /* default color for player 1 */
static const int player_2 = 2; /* default color for player 2 */
static char frameBuffer[SIZE]; /* RGB Matrix Grid */
static int rcol[3];
static int gcol[3];
static int bcol[3];
static int currentPlayer = player_1;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 500; // delay for half a second

int map(int x, int y) {
  if (((y-1) % 2) == 0) {
    return((y-1)*8+x-1);
  }
  return((y-1)*8+7-(x-1));
}

/* Place a dot with specified color code on the LED matrix*/
void displayDot (int x, int y, int color) {
  int r, g, b, i;
  //i = (y-1)*8+x-1;
  i = map(x,y);
  //Serial.println(i);
  r = rcol[color];
  g = gcol[color];
  b = bcol[color];
  pixels.setPixelColor(i, pixels.Color(r,g,b)); // Moderately bright green color.

  pixels.show(); // This sends the updated pixel color to the hardware.
}

void turnOff() {
  for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i, pixels.Color(0,0,0)); // Moderately bright green color.
    pixels.show();
  }
}
void sendFrame(char * buff) {
  int r, g, b;
  int player;
  for(int i=0;i<NUMPIXELS;i++){
    player = buff [i];
    if(player != player_1 && player != player_2) {
      //Serial.println(player);
      pixels.setPixelColor(i, pixels.Color(0,0,0)); // Moderately bright green color.
      pixels.show();
      continue;
    }
    r = rcol[player];
    g = gcol[player];
    b = bcol[player];
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(r,g,b)); // Moderately bright green color.

    pixels.show(); // This sends the updated pixel color to the hardware.



  }
}
/* Reset the screen: turn off all LEDs */
void resetScreen (char * buff) {
  for (int i = 0; i < SIZE; i++) {
   buff [i] = 0;
  }
  sendFrame (buff);
}
/* Place a dot with specified color code on the LED matrix*/
void placeDot (int x, int y, char * frame, int color) {
  int i=map(x,y);
  //frame [(y - 1) * 8 + x - 1] = color;
  frame [ i] = color;
  displayDot(x, y, color);
}
/* Remove a dot from the LED matrix (turn a LED off) */
void removeDot (int x, int y, char * frame) {
  int i=map(x,y);
  frame [ i] = 0;
  displayDot(x, y, 0);
  //frame [(y - 1) * 8 + x - 1] = 0;
}

bool occupied (int x, int y, char * frame) {
  int i = map(x,y);
  //int val = (int) frame [(y - 1) * 8 + x - 1];
  int val = (int) frame [i];
  if (val != player_1 && val != player_2) {
    Serial.println ("Target location is empty");
    return false;
  }
  Serial.println ("Target location is occupied!");
return true;
}

/* move cursor up in frame */
void moveup (char * frame) {
  if (csr_y_p < 8 && !occupied (csr_x_p, csr_y_p + 1, frame)) {
    Serial.println ("UP!");
    csr_y_c = csr_y_p + 1;
    removeDot(csr_x_p, csr_y_p, frame);
    placeDot(csr_x_p, csr_y_c, frame, currentPlayer);
    printpos();
    //delay (1000);
  }
}
void printpos() {
    Serial.print ("Cursor position: ");
    Serial.print ("(");
    Serial.print (csr_x_c);
    Serial.print (",");
    Serial.print (csr_y_c);
    Serial.print (",");
    Serial.print(map(csr_x_c, csr_y_c));
    Serial.println (")");
}
/* move cursor down in frame */
void movedown (char * frame) {
  if (csr_y_p > 1 && !occupied (csr_x_p, csr_y_p - 1, frame)) {
    Serial.println ("DOWN!");
    csr_y_c = csr_y_p - 1;
    removeDot(csr_x_p, csr_y_p, frame);
    placeDot(csr_x_p, csr_y_c, frame, currentPlayer);
    printpos();
  }
}
/* move cursor left in frame */
void moveleft (char * frame) {
  if (csr_x_p > 1 && !occupied (csr_x_p - 1, csr_y_p, frame)) {
    Serial.println ("LEFT!");
    csr_x_c = csr_x_p - 1;
    removeDot(csr_x_p, csr_y_p,frame);
    placeDot(csr_x_c, csr_y_p, frame, currentPlayer);
    printpos();
  }
}
/* move cursor right in frame */
void moveright (char * frame) {
  if (csr_x_p < 8 && !occupied (csr_x_p + 1, csr_y_p, frame)) {
    Serial.println ("RIGHT!");
    csr_x_c = csr_x_p + 1;
    removeDot(csr_x_p, csr_y_p, frame);
    placeDot(csr_x_c, csr_y_p, frame, currentPlayer);
    printpos();
  }
}
int getplayer (int x, int y, char * frame) {
  int i=map(x,y);
  return (int) frame [i];
}

/* check if player win game by row */
bool row_win (int player, char * frame) {
  for (int i = 1; i <= SIDE - K + 1; i++) {
    for (int j = 1; j <= SIDE; j++) {
      if (getplayer(i, j, frame) == player) {
        bool win = true;
        for (int k = 1; k < K; k++) {
          if (getplayer(i + k, j, frame) != player) {
            win = false;
            break;
          }
        }
        if (win == true) {
          return true;
        }
      } else {
        continue;
      }
    }
  }
  return false;
}
/* check if player win game by column */
bool col_win (int player, char * frame) {
  for (int i = 1; i <= SIDE; i++) {
    for (int j = 1; j <= SIDE - K + 1; j++) {
      if (getplayer (i, j, frame) == player) {
        bool win = true;
        for (int k = 1; k < K; k++) {
          if (getplayer(i, j + k, frame) != player) {
            win = false;
            break;
          }
        }
        if (win == true) {
          return true;
        }
      } else {
        continue;
      }
    }
  }
  return false;
}
/* check if player win game by diagonal */
bool dia_win (int player, char * frame) {
// check left/up to right/down win
  for (int i = 1; i <= SIDE - K + 1; i++) {
    for (int j = SIDE; j >= SIDE - K; j--) {
      if (getplayer (i, j, frame) == player) {
        bool win = true;
        for (int k = 1; k < K; k++) {
          if (getplayer (i + k, j - k, frame) != player) {
            win = false;
            break;
          }
        }
        if (win == true) {
          return true;
        }
      } else {
        continue;
      }
    }
  }
// check left/down to right/up win
  for (int i = 1; i <= SIDE - K + 1; i++) {
    for (int j = 1; j <= SIDE - K + 1; j++) {
      if (getplayer (i, j, frame) == player) {
        bool win = true;
        for (int k = 1; k < K; k++) {
          if (getplayer (i + k, j + k, frame) != player) {
            win = false;
            break;
          }
        }
        if (win == true) {
          return true;
        }
      } else {
        continue;
      }
    }
  }
  return false;
}

/* check if game is over */
int gameover () {
  if (row_win (player_1, frameBuffer) ||
    col_win (player_1, frameBuffer) ||
    dia_win (player_1, frameBuffer)) {
      for(int i=0;i<5;i++){
        turnOff();
        delay(500);
        sendFrame(frameBuffer);
      }
    return player_1;
  }
  if (row_win (player_2, frameBuffer) ||
    col_win (player_2, frameBuffer) ||
    dia_win (player_2, frameBuffer)) {
      for(int i=0;i<5;i++){
        turnOff();
        delay(500);
        sendFrame(frameBuffer);
      }
    return player_2;
  }
  return INPROGRESS;
}
/* check if
/* Remove a dot from the LED matrix (turn a LED off) */

/* reset game */
void resetgame () {
  csr_x_c = 1;
  csr_y_c = 1;
  csr_x_p = 1;
  csr_y_p = 1;
  steps = 0;
  resetScreen (frameBuffer);
  //displayDot(csr_x_p, csr_y_p, player_1);
  displayDot(csr_x_p, csr_y_p, player_1);
}

void setup() {

  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code
  delayMicroseconds(10);
  rcol[0] = 0;
  gcol[0] = 0;
  bcol[0] = 0;
  rcol[player_1] = 255;
  gcol[player_1] = 0;
  bcol[player_1] = 0;
  rcol[player_2] = 0;
  gcol[player_2] = 255;
  bcol[player_2] = 0;
  pinMode(analogInputPinX,   INPUT);
  pinMode(analogInputPinY,   INPUT);
  pinMode(digitalInputPin,   INPUT_PULLUP);
  currentPlayer = player_1;
  Serial.begin(9600);
  pixels.begin(); // This initializes the NeoPixel library.
  resetgame();


}


void loop() {
  // get the direction of movement

  analogInputValX = 475;
  analogInputValY = 475;

  analogInputValX = analogRead(analogInputPinX);
  analogInputValY = analogRead(analogInputPinY);
  digitalInputVal = digitalRead(digitalInputPin);
  //Serial.println(digitalInputVal);

  if (analogInputValY > 500) {
    moveup (frameBuffer);
  } else if (analogInputValY < 450) {
    movedown (frameBuffer);
  } else if (analogInputValX < 450) {
    moveleft (frameBuffer);
  } else if (analogInputValX > 500) {
    moveright (frameBuffer);
  }
  //delay (1000);
  //csr_x_p = csr_x_c;
  //csr_y_p = csr_y_c;
  
  //Serial.println("here");
  
  if (digitalInputVal == LOW) {
    if (csr_x_p == 1 && csr_y_p == 1) {
    return;
    }
    Serial.println ("CONFIRM!");
    if (steps % 2 == 0) {

      placeDot(csr_x_c, csr_y_c, frameBuffer, player_1);
      steps += 1;
      Serial.println("Player 1 dropped disk!");
      currentPlayer = player_2;

    } else {
      placeDot(csr_x_c, csr_y_c, frameBuffer, player_2);
      steps += 1;
      Serial.println("Player 2 dropped disk!");
      currentPlayer = player_1;

    }

    if (gameover () != INPROGRESS) {
      resetgame ();
      delay (200);
      return;
    }
    //sendFrame (frameBuffer);
    delay (300);
    csr_x_p = 1;
    csr_y_p = 1;
    csr_x_c = 1;
    csr_y_c = 1;
    displayDot(csr_x_p, csr_y_p, currentPlayer);
    return;

  }
  /*
  if (steps % 2 == 0) {
    placeDot (csr_x_c, csr_y_c, frameBuffer, player_1);
  } else {
    placeDot (csr_x_c, csr_y_c, frameBuffer, player_2);
  }
  
  if (csr_x_p != csr_x_c || csr_y_p != csr_y_c) {
    removeDot (csr_x_p, csr_y_p, frameBuffer);
    Serial.print ("Cursor position: ");
    Serial.print ("(");
    Serial.print (csr_x_c);
    Serial.print (",");
    Serial.print (csr_y_c);
    Serial.println (")");
  }*/

  
  delay(200);
  csr_x_p = csr_x_c;
  csr_y_p = csr_y_c;



}
