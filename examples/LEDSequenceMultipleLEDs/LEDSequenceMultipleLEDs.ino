#include <ezLED.h>
#include <ezLEDSequence.h>

ezLED led1(13);
ezLED led2(12);
ezLED led3(11);

ezLEDSequence seq1(&led1);
ezLEDSequence seq2(&led2);
ezLEDSequence seq3(&led3);

void setup() {
    Serial.begin(115200);
    
    seq1.addStep(100, 100, 5);
    seq2.addStep(100, 100, 5);
    seq3.addStep(100, 100, 5);
    
    seq1.startContinuous(0);
    seq2.startContinuous(0);
    seq3.startContinuous(0);
}

void loop() {
    seq1.loop();
    seq2.loop();
    seq3.loop();
    
    led1.loop();
    led2.loop();
    led3.loop();
}
