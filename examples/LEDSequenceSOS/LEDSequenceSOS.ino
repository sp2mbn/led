#include <ezLED.h>
#include <ezLEDSequence.h>

ezLED led(13);
ezLEDSequence sos(&led);

void setup() {
    Serial.begin(115200);
    led.useAnalog(true);
    
    sos.addStep(100, 100, 3);
    sos.addStep(300, 200, 3);
    sos.addStep(100, 100, 3);
    
    sos.startContinuous(0);
    Serial.println("SOS sequence started!");
}

void loop() {
    sos.loop();
    led.loop();
}
