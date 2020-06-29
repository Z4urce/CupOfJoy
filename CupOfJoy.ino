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

class ToneGenerator {
public:
  MusicWithoutDelay Buzzer;
  float Volume;
};


const int Notes[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5};

ToneGenerator toneGenerators[3];

void setup() {
  toneGenerators[0].Buzzer.begin(CHB, TRIANGLE, ENVELOPE0, 0);
  
  for(int i = 1; i < (sizeof(toneGenerators)/sizeof(*toneGenerators)); i++){
    toneGenerators[i].Buzzer.begin(TRIANGLE, ENVELOPE0, 0);
  }
  
  Serial.begin(9600);
}

void loop() {
  JoystickDTO joysticks[] = {{A0, A1}, {A2, A3}, {A4, A5}};
  
  for(int i = 0; i < (sizeof(toneGenerators)/sizeof(*toneGenerators)); i++){
    updateTone(toneGenerators[i], joysticks[i]);
  }
}

void updateTone(ToneGenerator toneGenerator, JoystickDTO joystick){
  int noteId = (joystick.JoyAngle+22)/45;
  if (joystick.JoyAngle < 22 || joystick.JoyAngle > 337) {
    noteId = 0;
  }

  int note = Notes[noteId];
  double mappedFrequency = toneGenerator.Buzzer.getNoteAsFrequency(note);

  if (joystick.JoyForce > 50) {
    toneGenerator.Volume = max(toneGenerator.Volume, min(127, map(joystick.JoyForce, 50,512,0,127)));
    toneGenerator.Buzzer.setFrequency(mappedFrequency); 
  }

  if (toneGenerator.Volume > 0) {
    --toneGenerator.Volume;
  }
  
  toneGenerator.Buzzer.setVolume(toneGenerator.Volume);
  toneGenerator.Buzzer.update();
}
