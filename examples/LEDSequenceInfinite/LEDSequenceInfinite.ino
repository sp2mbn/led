#include <ezLED.h>
#include <ezLEDSequence.h>

ezLED led(13);
ezLEDSequence sequence(&led);

void setup() {
    Serial.begin(115200);
    
    sequence.addStep(200, 100, 2);
    sequence.addStep(100, 200, 1);
    sequence.addStep(500, 500, 1);
    
    sequence.startContinuous(0);
}

void loop() {
    sequence.loop();
    led.loop();
}
