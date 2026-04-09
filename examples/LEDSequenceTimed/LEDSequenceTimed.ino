#include <ezLED.h>
#include <ezLEDSequence.h>

ezLED led(13);
ezLEDSequence sequence(&led);

void onStepEnd(StepInfo info) {
    Serial.print("Step: ");
    Serial.print(info.stepIndex);
    Serial.print(" (");
    Serial.print(info.stepRepetition);
    Serial.print("/");
    Serial.print(info.stepTotalRepetitions);
    Serial.print(") intensity: ");
    Serial.println(info.intensity);
}

void onSequenceEnd(SequenceInfo info) {
    Serial.print("Sequence finished! Steps: ");
    Serial.print(info.totalSteps);
    Serial.print(", repetitions: ");
    Serial.println(info.sequenceRepetition);
}

void setup() {
    Serial.begin(115200);
    
    sequence.addStep(200, 100, 2, 255);
    sequence.addStep(100, 200, 1, 128);
    sequence.addStep(500, 500, 1, 255);
    
    sequence.setOnStepEndCallback(onStepEnd);
    sequence.setOnSequenceEndCallback(onSequenceEnd);
    
    sequence.startTimed(3000);
}

void loop() {
    sequence.loop();
    led.loop();
}
