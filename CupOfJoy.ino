#include <MusicWithoutDelay.h>
#include <synth.h>
#include <tables.h>

struct JoystickDTO {
  int JoyAngle;
  float JoyForce;

  JoystickDTO(int pinX, int pinY) {
    float nX = analogRead(pinX) - 512;
    float nY = analogRead(pinY) - 512;
  
    JoyAngle = (atan2(nX, nY)+PI) * 180/PI;
    JoyForce = sqrt((nX*nX) + (nY*nY));
  }
};


//const char arpeggioScaleC[] PROGMEM = ":d=16:a,c#,e,a,c#,e,a,c#,e,a,c#,e,e,g#,b,e,g#,b,e,g#,b,e,g#,b";
const int Notes[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5};

//MusicWithoutDelay instrument(arpeggioScaleC);
MusicWithoutDelay buzzer_1;
MusicWithoutDelay buzzer_2;

float sustainVolume_1;
float sustainVolume_2;


void setup() {
  //instrument.begin(CHB, TRIANGLE, ENVELOPE0, 0);
  buzzer_1.begin(CHB, TRIANGLE, ENVELOPE0, 0);
  buzzer_2.begin(TRIANGLE, ENVELOPE0, 0);
  Serial.begin(9600);
}

void loop() {
  JoystickDTO joystick_1(A0, A1);
  JoystickDTO joystick_2(A2, A3);
  
  int noteId = (joystick_1.JoyAngle+22)/45;
  if (joystick_1.JoyAngle < 22 || joystick_1.JoyAngle > 337) {
    noteId = 0;
  }

  int note = Notes[noteId];
  double mappedFrequency = buzzer_1.getNoteAsFrequency(note);

  if (joystick_1.JoyForce > 50) {
    sustainVolume_1 = max(sustainVolume_1, min(127, map(joystick_1.JoyForce, 50,512,0,127)));
    buzzer_1.setFrequency(mappedFrequency); 
  }

  if (sustainVolume_1 > 0) {
    --sustainVolume_1;
  }
  
  buzzer_1.setVolume(sustainVolume_1);

  
  // ==============
  noteId = (joystick_2.JoyAngle+22)/45;
  if (joystick_2.JoyAngle < 22 || joystick_2.JoyAngle > 337) {
    noteId = 0;
  }

  note = Notes[noteId];
  mappedFrequency = buzzer_2.getNoteAsFrequency(note);

  if (joystick_2.JoyForce > 50) {
    sustainVolume_2 = max(sustainVolume_2, min(127, map(joystick_2.JoyForce, 50,512,0,127)));
    buzzer_2.setFrequency(mappedFrequency); 
  }

  if (sustainVolume_2 > 0) {
    --sustainVolume_2;
  }
  
  buzzer_2.setVolume(sustainVolume_2);
  // ===============
  
  
  buzzer_1.update();
  buzzer_2.update();
}
