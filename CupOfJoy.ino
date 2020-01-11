#include <MusicWithoutDelay.h>
#include <synth.h>
#include <tables.h>

const char arpeggioScaleC[] PROGMEM = ":d=16:a,c#,e,a,c#,e,a,c#,e,a,c#,e,e,g#,b,e,g#,b,e,g#,b,e,g#,b";
MusicWithoutDelay instrument(arpeggioScaleC);
MusicWithoutDelay buzzer;

int joy1Horizontal;
int joy1VerticalDelta;
int joy1Vertical;
float sustainVolume;

void setup() {
  //instrument.begin(CHB, TRIANGLE, ENVELOPE0, 0);
  buzzer.begin(CHB, TRIANGLE, ENVELOPE0, 0);
  Serial.begin(9600);
}

void ReadJoystickValues() {
  joy1Horizontal = analogRead(A0);
  
  int newVerticalRead = analogRead(A1);
  joy1VerticalDelta = joy1Vertical - newVerticalRead;
  joy1Vertical = newVerticalRead;
}

void loop() {
  ReadJoystickValues();

  float lowFrequency = buzzer.getNoteAsFrequency(NOTE_A4);
  float highFrequeny = buzzer.getNoteAsFrequency(NOTE_E5);

  int mappedFrequency = map(joy1Horizontal, 0,1023,lowFrequency,highFrequeny);

  if (joy1Vertical > 600) {
    sustainVolume = max(sustainVolume, map(joy1Vertical, 600,1023,0,127));
  }

  if (sustainVolume > 0) {
    --sustainVolume;
  }

  Serial.print(joy1VerticalDelta);
  Serial.print(",");
  Serial.println(sustainVolume);

  buzzer.setFrequency(mappedFrequency);
  buzzer.setVolume(sustainVolume);// 0-127

  //instrument.update();
  buzzer.update();
}
