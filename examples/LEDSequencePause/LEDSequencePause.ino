#include <ezLED.h>
#include <ezLEDSequence.h>

ezLED led(13);
ezLEDSequence sequence(&led);

const int BUTTON_PIN = 2;

void onStepEnd(StepInfo info) {
    Serial.print("Step: ");
    Serial.println(info.stepIndex);
}

void onSequenceEnd(SequenceInfo info) {
    Serial.println("Sequence finished!");
}

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    
    sequence.addStep(200, 100, 3);
    sequence.addStep(100, 200, 2);
    sequence.addStep(500, 500, 1);
    
    sequence.setOnStepEndCallback(onStepEnd);
    sequence.setOnSequenceEndCallback(onSequenceEnd);
    
    sequence.startContinuous(2);
    Serial.println("Sequence started!");
}

void loop() {
    sequence.loop();
    led.loop();
}
