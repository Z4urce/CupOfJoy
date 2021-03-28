#include <MusicWithoutDelay.h>
#include <synth.h>
#include <tables.h>

struct JoystickDTO {
  float JoyAngle;
  float JoyForce;

  JoystickDTO(int pinX, int pinY) {
    float nX = analogRead(pinX) - 512;
    float nY = analogRead(pinY) - 512;

    /*
    Serial.print(nX);
    Serial.print("\t"); // a space ' ' or  tab '\t' character is printed between the two values.
    Serial.print(nY);
    Serial.print("\t");
    */
    
    JoyAngle = (atan2(nX, nY)+PI) * (180.0/PI);
    JoyForce = min(sqrt((nX*nX) + (nY*nY)), 512);
  }

  // The direction the joystick point at divided to 8 segments
  int getSegment() {
    if (JoyAngle < 22.5 || JoyAngle > 337.5) {
      return 0;
    }

    return (JoyAngle+22.5)/45;
  }

  bool isTilted() {
    return JoyForce > 50;
  }

  int getTransposedNote(int transposition) {
    if (!isTilted()){
      return -1;
    }
    
    return 60 + getSegment() + transposition;
  }
};

class ToneGenerator {
public:
  MusicWithoutDelay Buzzer;
  float Volume;

  void update() {
    Buzzer.setVolume(Volume);
    Buzzer.update();
  }
};

ToneGenerator toneGenerators[3];

void setup() {
  toneGenerators[0].Buzzer.begin(CHB, TRIANGLE, ENVELOPE0, 0);
  
  for(int i = 1; i < (sizeof(toneGenerators)/sizeof(*toneGenerators)); i++){
    toneGenerators[i].Buzzer.begin(TRIANGLE, ENVELOPE0, 0);
  }
  
  Serial.begin(9600);
}

void loop() {
  JoystickDTO joysticks[] = {{A0, A1}, {A2, A3}, {A4, A5}, {A6, A7}};
  //Serial.println(0);

  int notes[] = { joysticks[0].getTransposedNote(0),
                  joysticks[1].getTransposedNote(4),
                  joysticks[2].getTransposedNote(7)};
  strum(notes, joysticks[3]);
}


void strum(int notes[], JoystickDTO strumJoystick){
  bool isTilted = strumJoystick.isTilted();
  
  for(int i = 0; i < 3; i++){
    if (isTilted && notes[i] > 0) {
      double mappedFrequency = toneGenerators[i].Buzzer.getNoteAsFrequency(notes[i] + strumJoystick.getSegment());
      toneGenerators[i].Volume = max(toneGenerators[i].Volume, min(127, map(strumJoystick.JoyForce, 50,512,0,127)));
      toneGenerators[i].Buzzer.setFrequency(mappedFrequency);
    }
    else if (toneGenerators[i].Volume > 0){
      --toneGenerators[i].Volume;
    }
    
    toneGenerators[i].update();
  }
}
