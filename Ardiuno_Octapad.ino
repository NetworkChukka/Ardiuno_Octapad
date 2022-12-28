// Copyright NetworkChukka

#define NUMBER_OF_PADS 8      
#define HI_HAT_PEDAL_PIN 2      
#define HI_HAT_PAD_PIN 2        
#define HI_HAT_CLOSED_NOTE 63  
#define VELOCITY_ACTIVE 1      
#define LOG_MAPPING 0           
#define MIDI_CHANNEL 1          

#define MIDI_MAX_VELOCITY 127
#define MIDI_NOTE_ON 0b1001    
#define MIDI_NOTE_OFF 0b1000    
// Copyright NetworkChukka
uint8_t padNote[NUMBER_OF_PADS] = {76, 81, 77, 54, 66, 69, 80, 65};               
uint16_t padThreshold[NUMBER_OF_PADS] = {750, 750, 750, 1022, 1022, 300, 920, 800}; 
uint16_t padCycles[NUMBER_OF_PADS] = {30, 30, 30, 40, 30, 30, 30, 30};             

uint8_t activePad;                          // each bit represents a pad state
uint8_t activeHiHat;                        // hi-hat state
uint16_t padCurrentCycles[NUMBER_OF_PADS];   // number of cycles since the pad was triggered

void setup() {
  pinMode(HI_HAT_PEDAL_PIN, INPUT_PULLUP);                                 
  Serial.begin(57600);                                                    
}

void loop() {
  for (uint8_t pin = 0; pin < NUMBER_OF_PADS; pin++) {                     
    uint16_t val = analogRead(pin);                                         
    if ((val > padThreshold[pin]) && (!padActive(pin))) {                 

      val = VELOCITY_ACTIVE ? velocityAlgorithm(val,LOG_MAPPING) : MIDI_MAX_VELOCITY;  
      uint8_t activeHiHat = checkHiHat(pin) ? 1 : 0;                       
      uint8_t note = activeHiHat ? HI_HAT_CLOSED_NOTE : padNote[pin];      

      midi_tx_note_on(note, val);                                          
      padCurrentCycles[pin] = 0;                                           
      activePad |= 1 << pin;                                              
    }

    if (padActive(pin)) {                                                  
      padCurrentCycles[pin] += 1;                                          

      if (padCurrentCycles[pin] >= padCycles[pin]) {                       
        if (pin == HI_HAT_PAD_PIN && activeHiHat) {                        
          midi_tx_note_off(HI_HAT_CLOSED_NOTE);                            
        }
        else {
          midi_tx_note_off(padNote[pin]);                                  
        }

        activePad &= ~(1 << pin);                                          
      }
    }
  }
}

// Copyright NetworkChukka
uint8_t velocityAlgorithm(uint16_t val, uint8_t logswitch) {
  if (logswitch) {
     return log(val + 1)/ log(1024) * 127;
  }
    return (val - 0) * (127 - 0) / (1023 - 0) + 0;
}

uint8_t checkHiHat(uint8_t currentPin) {                                   
  return (currentPin == HI_HAT_PAD_PIN && pedalPressed()) ? 1 : 0;
}
// Copyright NetworkChukka
uint8_t pedalPressed() {                                                   
  return digitalRead(HI_HAT_PEDAL_PIN) ? 0 : 1;
}

uint8_t padActive(uint8_t currentPin) {                                    
  return (activePad >> currentPin) & 1;
}
// Copyright NetworkChukka
void midi_tx_note_on(uint8_t pitch, uint8_t velocity) {                    
  Serial.write((MIDI_NOTE_ON << 4) | (MIDI_CHANNEL - 1));
  Serial.write(pitch);
  Serial.write(MIDI_MAX_VELOCITY);
}
// Copyright NetworkChukka
void midi_tx_note_off(uint8_t pitch) {                                     // Copyright NetworkChukka
  Serial.write((MIDI_NOTE_OFF << 4) | (MIDI_CHANNEL - 1));
  Serial.write(pitch);
  Serial.write(MIDI_MAX_VELOCITY);
}
