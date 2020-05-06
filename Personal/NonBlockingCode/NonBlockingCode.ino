/* =====================================================================
 * -- Name ------ : Heart Beat
 * -- Date ------ : May 6, 2020
 * -- Author ---- : Carlos Mendez
 * -- Description : This execute a program without blocking code (delays)
 ===================================================================== */

const int heart_beat = 13; 
int heart_beat_state = LOW; 
int heart_beat_count = 0;
const int heart_beat_timer = 1000;

void setup() {
  pinMode(heart_beat, OUTPUT);
  digitalWrite(heart_beat, LOW);
}

void loop() {
  if (heart_beat_count >= heart_beat_timer){
    if (heart_beat_state == LOW) {
      heart_beat_state = HIGH;
    } else {
      heart_beat_state = LOW;
    }
    digitalWrite(heart_beat, heart_beat_state);
    
    heart_beat_count = 0;
  }
  
  heart_beat_count++;
  delay(1);
}
