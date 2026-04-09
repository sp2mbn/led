#include <ezLED.h>
#include <ezLEDSequence.h>

ezLED led(13);
ezLEDSequence sequence(&led);

void onStepEnd(StepInfo info) {
    Serial.print("Step ended: index=");
    Serial.print(info.stepIndex);
    Serial.print(", repetition=");
    Serial.print(info.stepRepetition);
    Serial.print("/");
    Serial.println(info.stepTotalRepetitions);
}

void onSequenceEnd(SequenceInfo info) {
    Serial.print("Sequence ended: totalSteps=");
    Serial.print(info.totalSteps);
    Serial.print(", sequenceRepetitions=");
    Serial.println(info.sequenceRepetition);
}

void setup() {
    Serial.begin(115200);
    
    sequence.addStep(200, 100, 2, 255);
    sequence.addStep(100, 200, 1, 128);
    sequence.addStep(500, 500, 1, 255);
    
    sequence.setOnStepEndCallback(onStepEnd);
    sequence.setOnSequenceEndCallback(onSequenceEnd);
    
    sequence.startContinuous(3);
}

void loop() {
    sequence.loop();
    led.loop();
}
