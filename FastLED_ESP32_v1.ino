
byte brightness;
uint8_t mode;             //режим
uint8_t remember_mode;    //запоминаем режим, для повторения 
uint8_t submode;          //подрежим
bool submode3_flag;       //флаг, для понимания программой, что включен 3 режим.

#include <FastLED.h>

#define DATA_PIN    25              //пин подключения
#define DATA_PIN_2    26              //пин подключения



// #define READ_DATA_PIN 27

// #define FIRST_STRIP_PIN 25
// #define SECOND_STRIP_PIN 26



// #define DATA_PIN    12              //пин подключения
//#define CLK_PIN   4
#define LED_TYPE    WS2812          //тип чипа 
#define COLOR_ORDER GRB             //порядок цветов
#define NUM_LEDS    300             //количество ледов
#define BRIGHTNESS  155             //базовая яркость

#define FRAMES_PER_SECOND  120      //к/с для 1 из эффеектов

CRGB leds[NUM_LEDS];                //задаем массив ленты



// -----------------Кнопки пульта-----------------
// #if REMOTE_TYPE == 1
#define BUTT_BR_PLUS    0x1FE20DF  //Brightness ↑
#define BUTT_BR_MINUS   0x1FEA05F  //Brightness ↓
#define BUTT_OFF        0x1FE58A7  //Off
#define BUTT_ON         0x1FE48B7  //On

#define BUTT_FLASH      0x1FE7887  //Flash
#define BUTT_STROBE     0x1FE807F  //Strobe
#define BUTT_FADE       0x1FE40BF  //Fade
#define BUTT_SMOOTH     0x1FEC03F  //Smooth

#define BUTT_WHITE      0x1FE609F  //White
#define BUTT_RED        0x1FEE01F  //Red
#define BUTT_BLUE       0x1FE10EF  //Blue
#define BUTT_GREEN      0x1FE906F  //Green
#define BUTT_1          0x1FE50AF  //Red2/Brown 1
#define BUTT_2          0x1FED827  //Blue2/Lime 2
#define BUTT_3          0x1FEF807  //Green2/ 3 
#define BUTT_4          0x1FE30CF  //Red3/Orange 4
#define BUTT_5          0x1FEB04F  //Blue3/Aqua 5
#define BUTT_6          0x1FE708F  //Green3/Purple 6
#define BUTT_7          0x1FE00FF  //Red4/Yellow 7
#define BUTT_8          0x1FEF00F  //Blue4/Navy 8
#define BUTT_9          0x1FE9867  //Green4/Rose 9
// #endif


// #define USE_IRREMOTE_HPP_AS_PLAIN_INCLUDE
#include <IRremote.hpp>
IRrecv irrecv(4);       // указываем вывод, к которому подключен приемник
decode_results IRDATA;  //данные с приемника записываются сюда
unsigned long LastCase; //для повторения предыдущей команды, если получили команду 0xFFFFFFFF






void setup() {

  // pinMode(READ_DATA_PIN, INPUT);
  // pinMode(FIRST_STRIP_PIN, OUTPUT);
  // pinMode(SECOND_STRIP_PIN, OUTPUT);
  // attachInterrupt (0, govnocode,CHANGE);


  irrecv.enableIRIn(); // запускаем прием
  Serial.begin(115200);  // выставляем скорость COM порта

  // яркость (0-255)
  //задаем прочие переменные
  brightness = 155;
  mode = 0;
  submode = 0;
  submode3_flag = 0;


  //инициализируем ленту
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS)    
    .setCorrection(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);

  FastLED.addLeds<LED_TYPE,DATA_PIN_2,COLOR_ORDER>(leds, NUM_LEDS)    
    .setCorrection(TypicalLEDStrip)
    .setDither(BRIGHTNESS < 255);

  // set master brightness control
  FastLED.showColor(CRGB::White);
  FastLED.setBrightness(BRIGHTNESS);

}




// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };      //задаем включенные эффекты в режим 2
//и несколько необходимых переменных для этого режима
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns





void loop() {

  switch_between_IRDATA();
  repete_mode();

}






void repete_mode() {
  switch (mode) {

    default :
      // EVERY_N_MILLISECONDS( 1000 ) { Serial.println("DEFAULT"); }
    break;

    case 1 : //Fade
      pride();
      FastLED.show();
    break;

    case 2 : //Strobe
      DemoReel100();
      FastLED.show();
    break;

    case 3 : // Smooth- Flash+ (+-submodes)
      EVERY_N_MILLISECONDS( 20 ) { gHue++; }
      switch (submode) {

        default:
          submode = 1;
          
        break;

        case 1 :
          rainbow();
          FastLED.show();
        break;

        case 2 :
          rainbowWithGlitter();
          FastLED.show();
        break;

        case 3 :
          confetti();
          FastLED.show();
        break;

        case 4 :
          sinelon();
          FastLED.show();
        break;

        case 5 :
          juggle();
          FastLED.show();
        break;

        case 6 :
          bpm();
          FastLED.show();
        break;

        case 7 :
          smoothColorChange();
          FastLED.show();
        break;
      }
    break;
  }
}

void switch_between_IRDATA() {

  if ( irrecv.decode( &IRDATA )) { // если данные пришли
    Serial.println(IRDATA.value, HEX); // печатаем данные
    
    if (IRDATA.value == 0xFFFFFFFF) {
      IRDATA.value = LastCase;
    }

    switch (IRDATA.value) {

        default :
            remember_mode = mode;
            mode = 0;
        break;

        case BUTT_BR_PLUS :
            if (brightness <=250) {
                brightness = brightness + 5;
                FastLED.setBrightness(brightness);
            }
            break;


        case BUTT_BR_MINUS :
            if (brightness >=5) {
                brightness = brightness - 5;
                FastLED.setBrightness(brightness);
            }
            break;


        case BUTT_OFF :
            // FastLED.clear();
            FastLED.setBrightness(0);
            // submode3_flag = 0;
            break;


        case BUTT_ON :
            FastLED.setBrightness(brightness);
            mode = remember_mode;
            Serial.println("mode is"); 
            Serial.println(mode); 
            FastLED.show();  
            break;

        
        case BUTT_FLASH :
            
            mode = 3;
            if (submode3_flag == 0) { 
                submode3_flag = 1;
                submode = 1;
            } else { 
              if (submode < 7) { 
                submode++; } else {
                  submode = 1;}
              }

            delay(200);   
            break;

        case BUTT_SMOOTH :
            
            mode = 3;
            if (submode3_flag == 0) { 
                submode3_flag = 1;
                submode = 7;
            } else { 
               if (submode > 1) { 
                submode--; } else {
                  submode = 7;}
              }

            delay(200);  
            break;            


        case BUTT_STROBE :
            mode = 2;
            submode3_flag = 0;
            delay(200);  
            break;


        case BUTT_FADE :
            mode = 1;
            submode3_flag = 0;
            delay(200);  
            break;


        case BUTT_WHITE :
            for (int i = 0; i < NUM_LEDS; i++) {
              leds[i] = CRGB::White;
            }
            mode = 0;
            submode3_flag = 0;
            break;


        case BUTT_RED :
            for (int i = 0; i < NUM_LEDS; i++) {
              leds[i] = CRGB::Red;
            }
            mode = 0;
            submode3_flag = 0;
            break;


        case BUTT_BLUE :
            for (int i = 0; i < NUM_LEDS; i++) {
              leds[i] = CRGB::Blue;
            }
            mode = 0;
            submode3_flag = 0;
            break;


        case BUTT_GREEN :
            for (int i = 0; i < NUM_LEDS; i++) {
              leds[i] = CRGB::Green;
            }
            mode = 0;
            submode3_flag = 0;
            break;


        case BUTT_1 :
            for (int i = 0; i < NUM_LEDS; i++) {
              leds[i] = CRGB(200, 20, 0);  //Brown
            }
            mode = 0;
            submode3_flag = 0;
            break;


        case BUTT_2 :
            for (int i = 0; i < NUM_LEDS; i++) {
              leds[i] = CRGB(0, 255, 20);
            }
            mode = 0;
            submode3_flag = 0;
            break;


        case BUTT_3 :
            for (int i = 0; i < NUM_LEDS; i++) {
              leds[i] = CRGB(0, 150, 255);
            }
            mode = 0;
            submode3_flag = 0;
            break;


        case BUTT_4 :
            for (int i = 0; i < NUM_LEDS; i++) {
              leds[i] = CRGB(255,80,0); //Orange
            }
            mode = 0;
            submode3_flag = 0;
            break;


        case BUTT_5 :
            for (int i = 0; i < NUM_LEDS; i++) {
              leds[i] = CRGB(0, 255, 255);
            }
            mode = 0;
            submode3_flag = 0;
            break;


        case BUTT_6 :
            for (int i = 0; i < NUM_LEDS; i++) {
              leds[i] = CRGB(128, 0, 255);
            }
            mode = 0;
            submode3_flag = 0;
            break;


        case BUTT_7 :
            for (int i = 0; i < NUM_LEDS; i++) {
              leds[i] = CRGB::Yellow;
            }
            mode = 0;
            submode3_flag = 0;
            break;


        case BUTT_8 :
            for (int i = 0; i < NUM_LEDS; i++) {
              leds[i] = CRGB(50,50,255);
            }
            mode = 0;
            submode3_flag = 0;
            break;


        case BUTT_9 :
            for (int i = 0; i < NUM_LEDS; i++) {
              leds[i] = CRGB(255, 0, 225);
            }
            mode = 0;
            submode3_flag = 0;
            break;

      }
    FastLED.show(); 
    LastCase = IRDATA.value;
        irrecv.resume(); // принимаем следующую команду
        // Serial.println(mode); 
    delay(40);
  } 
}



































uint8_t hue;  //переменная для перелива эффектов в режиме 2 и 3


//плавная смена цветов
void smoothColorChange() {  
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV (hue, 255, 255);
  }
  EVERY_N_MILLISECONDS(70) {
    hue++;
  }
}

//Моя жизнь -- мои правила
void pride() {
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);
  }
  // irrecv.resume(); // принимаем следующую команду
}



#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

//режим 2 -- постоянная смена эффектов подремов 3 подряд автоматически
void DemoReel100() {
 // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}


//для перехода от эффекта к эффекту в режиме 2
void nextPattern() {
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

//радуга
void rainbow() {
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

//радуга с бликами
void rainbowWithGlitter() {
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

//внутрянка для добавления бликов
void addGlitter( fract8 chanceOfGlitter) {
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

//блики отдельно, конфетти
void confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

//
void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm() {
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  uint8_t dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

// void govnocode(){

//   // if (PIND&1<<2) { PORTD|=1<<PB3; PORTD&=~(1<<PB4); }
//   //    else { PORTD|=1<<PB4; PORTD&=~(1<<PB3); }
//   Serial.println("GOVNO");
//   bool a = digitalRead(READ_DATA_PIN);
//   digitalWrite(FIRST_STRIP_PIN,a);
//   digitalWrite(SECOND_STRIP_PIN,a);


// }


