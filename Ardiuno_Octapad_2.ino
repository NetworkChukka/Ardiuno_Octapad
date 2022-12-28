//Networkchukka
#define PIEZ_CNT 8 
#define DRUM_CHANEL 8
#define ANOTHER_MUTE_PIN_
#define SHOW_
#define CRASH 0 
// #define CRASHC 54
#define PIN_CRASH_STOP 9 
#define OPEN_HAT 90 
//#define OPENING_HAT 200 
#define KANAL_HAT 1 
#define KNOPKA_HAT 100 
#define LED 02

int kick_treshold[PIEZ_CNT]={200,200,200,200,200,600,200,200};


long kick_mute_time[PIEZ_CNT]={60000,50000,50000,50000,50000,50000,50000,50000};


byte kick_note[PIEZ_CNT]={10,20,30,40,50,60,70,80};


bool mute[PIEZ_CNT]={1,1,1,1,1,1,1,1};


const uint8_t kick_pin[PIEZ_CNT]={A0,A1,A2,A3,A4,A5,A6,A7};

#ifdef ANOTHER_MUTE_PIN
  const uint8_t stop_pin[PIEZ_CNT]={A0,A1,A2,A3,A4,A5,A6,A7};
#endif

int kick_val[PIEZ_CNT]; 
int velocity;

uint32_t tm[PIEZ_CNT], tm_led;
byte cnt=0, piez_idx=0;

void noteOn(int chan, int note, int velocity) {
    Serial.write( (0x90 | chan) );
    Serial.write(note);
    Serial.write(velocity);
}

void noteOff(int chan, int note, int velocity) {
    Serial.write( (0x80 | chan) );
    Serial.write(note);
    Serial.write(velocity);
}


void setup() {
  Serial.begin(115200); 
  //Serial.begin(31250); 
   pinMode(LED, OUTPUT);
  
   pinMode(PIN_CRASH_STOP, INPUT);
   pinMode(KNOPKA_HAT, INPUT); 
   
  digitalWrite(LED, LOW);
  for (byte i=0; i<PIEZ_CNT; i++) {
    digitalWrite(kick_pin[i], LOW);
    pinMode(kick_pin[i], INPUT);
#ifdef ANOTHER_MUTE_PIN   
    digitalWrite(stop_pin[i], LOW);
    pinMode(stop_pin[i], INPUT);   
#endif   
  }
 
  

  
  ADCSRA |= (1 << ADPS2);                     
  ADCSRA &= ~ ((1 << ADPS1) | (1 << ADPS0));  
}


void loop(){
  
if (mute[CRASH] == false) { 
      if (digitalRead[PIN_CRASH_STOP] == LOW) { 
        noteOn (DRUM_CHANEL,kick_note[CRASH],0); 
        //noteOn (DRUM_CHANEL,kick_note[CRASHC],127); 
                                              }   
                           } 

    if(piez_idx>0) piez_idx--; else piez_idx=PIEZ_CNT-1; 
 
    int adc = analogRead(kick_pin[piez_idx]);
    if (adc > kick_treshold[piez_idx]) {
      if ( tm[piez_idx] == 0 ){
        tm[piez_idx] = micros() + 2000;
        kick_val[piez_idx] = 0;
        digitalWrite(LED, HIGH);
        tm_led = millis()+10;
      }
    }

    if (tm_led > 0) {
      if (millis() > tm_led) {
        tm_led = 0;
        digitalWrite(LED, LOW);
      }
    }
       
    if ( tm[piez_idx] > 0 ) {   
      if ( micros() > tm[piez_idx] ) {
        if ( mute[piez_idx] ) {
          mute[piez_idx] = false;
          velocity = 127;
          
          if (velocity > 127) velocity = 127;
          if (piez_idx == KANAL_HAT){
              if (digitalRead(KNOPKA_HAT) == LOW ){ 
              //
                }
                else {
                  noteOn(DRUM_CHANEL,kick_note[piez_idx],velocity); 
                }
              }
          else {           
          noteOn(DRUM_CHANEL, kick_note[piez_idx], velocity); 
          }
          tm[piez_idx] += kick_mute_time[piez_idx];
          

          #ifdef ANOTHER_MUTE_PIN
            pinMode(stop_pin[piez_idx], OUTPUT); 
          #else
            pinMode(kick_pin[piez_idx], OUTPUT); 
          #endif
          #ifdef SHOW
            cnt++;
            velocity = kick_val[piez_idx];
            Serial.print("IDX="); Serial.print(chan);
            Serial.print(" Vel="); Serial.print(velocity);
            Serial.print(" cnt="); Serial.println(note);
          #endif
        }
      }
      if ( kick_val[piez_idx] < adc ) {
        kick_val[piez_idx] = adc; 
      }

      if ( micros() >  tm[piez_idx] ) {
        
        tm[piez_idx] = 0;
        mute[piez_idx] = true;
        
        #ifdef ANOTHER_MUTE_PIN
          pinMode(stop_pin[piez_idx], INPUT);
        #else
          pinMode(kick_pin[piez_idx], INPUT);  
        #endif   
            
      }
    }
}
