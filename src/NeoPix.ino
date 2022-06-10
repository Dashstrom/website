#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h>
#endif
#define OFF -1


#define PIN_LED 2 /* Pin of band LED */
#define PIN_FLAME 8 /* Pin of sensor */
#define PIN_RESET 7 /* Pin of sensor */

#define LED_NUMBER 36 /* Number of LED */
#define COLORS_SIZE 3 /* Number of colors inside a palette */
#define TRAIN_SIZE 4 /* Size of block light */
#define STEP 1 /* Number of step light */
#define DELAY 15 /* Speed of Light */
#define SENSITIVITY 100 /* Number of loops before check time */
#define GAP 1 /* Gap between two wave */
#define COLORS_NUMBER 3 /* Number of colors palettes */
int COLORS [COLORS_NUMBER][COLORS_SIZE][3] = {
  {{255,0,0}, {0,255,0}, {0,0,255}},
  {{153,51,255}, {51,255,153}, {153,51,255}},
  {{255, 255, 0}, {255,0,255}, {0, 255, 255}}
};

#define PIXELS_SIZE (LED_NUMBER + TRAIN_SIZE)
int PIXELS [PIXELS_SIZE];
int COOLDOWN = 0;
int LOOP = 0;
int PREVIOUS_FIRE = LOW;
int NO_ACTIVITY = 0;

Adafruit_NeoPixel pixels(LED_NUMBER, PIN_LED, NEO_GRB + NEO_KHZ800);

void(* resetFunc) (void) = 0;

/* Run Only Once */
void setup() {
  Serial.begin(9600);
  pinMode(PIN_FLAME, INPUT);//define FLAME input pin
  pinMode(PIN_RESET, INPUT);//define FLAME input pin
  digitalWrite(PIN_RESET, HIGH);

  clear_pixels();
  pixels.begin();
  pixels.clear();
  pixels.show();
}

/* Main loop */
void loop() {
   int fire = detect_fire();
   if (fire == LOW && COOLDOWN <= 0 && fire != PREVIOUS_FIRE) {
    push_pixels();
   }
   move_pixels();
   show_pixels();
   if (COOLDOWN > 0) {
    COOLDOWN --;
   }
   PREVIOUS_FIRE = fire;
   check_activity();
}


/* Detect fire during DELAY milliseconds, return LOW if nothing is detected else return HIGH */
int detect_fire() {
  int fire = HIGH;
  int start_miliseconds = millis();
  while (millis() - start_miliseconds < DELAY) {
    for (int i = 0; i < SENSITIVITY; i++) {
      if (fire != LOW) {
        fire = digitalRead(PIN_FLAME);
      }
    }
  }
  return fire;
}

/* Clear all pixels */
void clear_pixels() {
  for(int i = 0; i < PIXELS_SIZE; i++) {
    PIXELS[i] = OFF;
  }
  LOOP = 0;
}

/* Push TRAIN_SIZE pixels im PIXELS array */
void push_pixels() {
  for (int i = 0; i < TRAIN_SIZE; i++) {
    PIXELS[i] = LOOP;
  }
  LOOP = (LOOP + 1) % COLORS_NUMBER;
  COOLDOWN = TRAIN_SIZE + GAP;
}

/* Move all pixels */
void move_pixels() {
  for (int s = 0; s < STEP; s++) {
    PIXELS[PIXELS_SIZE - 1] = OFF;
    for(int i = PIXELS_SIZE - 2; i >= 0; i--) {
      PIXELS[i+1] = PIXELS[i];
    }
    PIXELS[0] = OFF;
  }
}

/* Get Color */
int* get_color(int type, int i) {
    return COLORS[type][((i - TRAIN_SIZE) * TRAIN_SIZE / LED_NUMBER)];
}

/* Actualize all pixels with LED */
void show_pixels() {
    for(int i = 0; i < LED_NUMBER; i++){
      int j = i + TRAIN_SIZE;
      if (PIXELS[j] != OFF) {
        int *rgb = get_color(PIXELS[j], i);
        pixels.setPixelColor(i, pixels.Color(rgb[0], rgb[1], rgb[2]));
      } else {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      }
    }
    pixels.show();
}

/* Get pixels activity */
bool is_active() {
  for(int i = 0; i < PIXELS_SIZE; i++) {
    if (PIXELS[i] != OFF) {
      return true;
    }
  }
  return false;
}

/* Check if arduino is inactive, if it then it reset */
void check_activity() {
  if (!is_active()) {
    NO_ACTIVITY ++;
  } else {
    NO_ACTIVITY = 0;
  }
  if (NO_ACTIVITY > 200) {
    Serial.flush();
    digitalWrite(PIN_RESET, HIGH);
    resetFunc();
  }
} 
