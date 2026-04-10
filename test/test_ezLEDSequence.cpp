#include <unity.h>
#include <vector>
#include <functional>

#ifdef ARDUINO
#include <Arduino.h>
#include <ezLED.h>
#include <ezLEDSequence.h>
#else

#define LED_OFF 0
#define LED_ON  1
#define LED_IDLE     0
#define LED_DELAY    1
#define LED_FADING   2
#define LED_BLINKING 3
#define CTRL_ANODE   0
#define CTRL_CATHODE 1

#define SEQ_MODE_CONTINUOUS 0
#define SEQ_MODE_TIMED      1
#define SEQ_STATE_IDLE      0
#define SEQ_STATE_DELAY    1
#define SEQ_STATE_RUNNING   2
#define SEQ_STATE_PAUSED   3
#define SEQ_STATE_STEP_ON   4
#define SEQ_STATE_STEP_OFF 5

void delay(unsigned long ms);
unsigned long millis();

struct StepInfo {
    int stepIndex;
    int stepRepetition;
    int stepTotalRepetitions;
    unsigned long duration;
    unsigned long pause;
    int intensity;
};

struct SequenceInfo {
    int currentStepIndex;
    int currentStepRepetition;
    int sequenceRepetition;
    int totalSteps;
};

struct Step {
    unsigned long duration;
    unsigned long pause;
    int intensity;
    unsigned int repetitions;
};

class ezLED {
public:
    ezLED(int pin, int mode = CTRL_ANODE);
    void useAnalog(bool forceAnalog);
    void setAnalogMode(bool analogMode);
    void turnON(unsigned long delayTime = 0);
    void turnOFF(unsigned long delayTime = 0);
    void toggle(unsigned long delayTime = 0);
    void fade(int fadeFrom, int fadeTo, unsigned long fadeTime, unsigned long delayTime = 0);
    void blink(unsigned long onTime, unsigned long offTime, unsigned long delayTime = 0);
    void blinkInPeriod(unsigned long onTime, unsigned long offTime, unsigned long blinkTime, unsigned long delayTime = 0, std::function<void()> afterBlinkCallback = nullptr);
    void blinkNumberOfTimes(unsigned long onTime, unsigned long offTime, unsigned int numberOfTimes, unsigned long delayTime = 0, std::function<void()> afterBlinkCallback = nullptr);
    void setBrightness(int brightness);
    void setOnTurnOnCallback(void (*callback)());
    void cancel();
    int getOnOff();
    int getState();
    int getPin();
    void loop();
    
    int _mockOnOff;
};

class ezLEDSequence {
private:
    ezLED* _led;
    std::vector<Step> _steps;
    
    unsigned char _sequenceMode;
    unsigned int _sequenceRepetitions;
    unsigned long _sequenceDuration;
    unsigned long _delayTime;
    
    unsigned char _sequenceState;
    unsigned long _lastTime;
    unsigned long _sequenceTimer;
    
    int _currentStepIndex;
    int _currentStepRepetition;
    int _currentSequenceRepetition;
    
    unsigned long _stepOnTime;
    unsigned long _stepOffTime;
    
    std::function<void(StepInfo)> _onStepEndCallback;
    std::function<void(SequenceInfo)> _onSequenceEndCallback;
    
    void executeStep();
    void nextStep();
    void finishSequence();
    
public:
    ezLEDSequence(ezLED* led);
    
    void addStep(unsigned long duration, unsigned long pause, unsigned int repetitions = 1, int intensity = -1);
    void clear();
    
    void startContinuous(unsigned int repetitions = 0);
    void startTimed(unsigned long durationMs);
    void start(unsigned long delayTime = 0);
    
    void pause();
    void resume();
    void stop();
    void cancel();
    
    void setOnStepEndCallback(std::function<void(StepInfo)> callback);
    void setOnSequenceEndCallback(std::function<void(SequenceInfo)> callback);
    
    unsigned char getState();
    int getCurrentStepIndex();
    int getCurrentStepRepetition();
    int getSequenceRepetition();
    int getTotalSteps();
    
    void loop();
};

#endif

static unsigned long mockMillisValue = 0;

unsigned long millis() {
    return mockMillisValue;
}

void setMillis(unsigned long value) {
    mockMillisValue = value;
}

void advanceMillis(unsigned long ms) {
    mockMillisValue += ms;
}

void setUp(void) {
    mockMillisValue = 0;
}

void tearDown(void) {
}

void test_constructor_initializes_correctly() {
    ezLED led(13);
    ezLEDSequence seq(&led);
    
    TEST_ASSERT_EQUAL(SEQ_STATE_IDLE, seq.getState());
    TEST_ASSERT_EQUAL(0, seq.getCurrentStepIndex());
    TEST_ASSERT_EQUAL(0, seq.getCurrentStepRepetition());
    TEST_ASSERT_EQUAL(0, seq.getSequenceRepetition());
    TEST_ASSERT_EQUAL(0, seq.getTotalSteps());
}

void test_addStep_increases_step_count() {
    ezLED led(13);
    ezLEDSequence seq(&led);
    
    seq.addStep(100, 50, 1, 255);
    TEST_ASSERT_EQUAL(1, seq.getTotalSteps());
    
    seq.addStep(200, 100, 2, 128);
    TEST_ASSERT_EQUAL(2, seq.getTotalSteps());
}

void test_start_with_no_steps_does_nothing() {
    ezLED led(13);
    ezLEDSequence seq(&led);
    
    seq.start();
    
    TEST_ASSERT_EQUAL(SEQ_STATE_IDLE, seq.getState());
}

void test_startContinuous_starts_sequence() {
    ezLED led(13);
    ezLEDSequence seq(&led);
    
    seq.addStep(100, 50, 1);
    seq.startContinuous(1);
    
    TEST_ASSERT_EQUAL(SEQ_STATE_STEP_ON, seq.getState());
    TEST_ASSERT_EQUAL(1, seq.getSequenceRepetition());
    TEST_ASSERT_EQUAL(0, seq.getCurrentStepIndex());
}

void test_startContinuous_infinite_mode() {
    ezLED led(13);
    ezLEDSequence seq(&led);
    
    seq.addStep(100, 50, 1);
    seq.startContinuous(0);
    
    TEST_ASSERT_EQUAL(SEQ_STATE_STEP_ON, seq.getState());
}

void test_startTimed_starts_sequence() {
    ezLED led(13);
    ezLEDSequence seq(&led);
    
    seq.addStep(100, 50, 1);
    seq.startTimed(5000);
    
    TEST_ASSERT_EQUAL(SEQ_STATE_STEP_ON, seq.getState());
}

void test_pause_and_resume() {
    ezLED led(13);
    ezLEDSequence seq(&led);
    
    seq.addStep(100, 50, 1);
    seq.startContinuous(1);
    
    TEST_ASSERT_EQUAL(SEQ_STATE_STEP_ON, seq.getState());
    
    seq.pause();
    TEST_ASSERT_EQUAL(SEQ_STATE_PAUSED, seq.getState());
    
    seq.resume();
    TEST_ASSERT_EQUAL(SEQ_STATE_RUNNING, seq.getState());
}

void test_stop_resets_state() {
    ezLED led(13);
    ezLEDSequence seq(&led);
    
    seq.addStep(100, 50, 1);
    seq.startContinuous(1);
    
    TEST_ASSERT_EQUAL(SEQ_STATE_STEP_ON, seq.getState());
    
    seq.stop();
    TEST_ASSERT_EQUAL(SEQ_STATE_IDLE, seq.getState());
}

void test_cancel_clears_steps() {
    ezLED led(13);
    ezLEDSequence seq(&led);
    
    seq.addStep(100, 50, 1);
    seq.addStep(200, 100, 1);
    TEST_ASSERT_EQUAL(2, seq.getTotalSteps());
    
    seq.cancel();
    TEST_ASSERT_EQUAL(0, seq.getTotalSteps());
    TEST_ASSERT_EQUAL(SEQ_STATE_IDLE, seq.getState());
}

void test_clear_removes_steps() {
    ezLED led(13);
    ezLEDSequence seq(&led);
    
    seq.addStep(100, 50, 1);
    seq.addStep(200, 100, 1);
    TEST_ASSERT_EQUAL(2, seq.getTotalSteps());
    
    seq.clear();
    TEST_ASSERT_EQUAL(0, seq.getTotalSteps());
}

void test_step_transitions() {
    ezLED led(13);
    ezLEDSequence seq(&led);
    
    seq.addStep(100, 50, 1);
    seq.startContinuous(1);
    
    TEST_ASSERT_EQUAL(SEQ_STATE_STEP_ON, seq.getState());
    
    advanceMillis(100);
    seq.loop();
    
    TEST_ASSERT_EQUAL(SEQ_STATE_STEP_OFF, seq.getState());
    
    advanceMillis(50);
    seq.loop();
    
    TEST_ASSERT_EQUAL(SEQ_STATE_STEP_ON, seq.getState());
    TEST_ASSERT_EQUAL(0, seq.getCurrentStepIndex());
}

void test_sequence_repetitions() {
    ezLED led(13);
    ezLEDSequence seq(&led);
    
    seq.addStep(100, 50, 1);
    seq.startContinuous(2);
    
    for (int i = 0; i < 4; i++) {
        advanceMillis(100);
        seq.loop();
        advanceMillis(50);
        seq.loop();
    }
    
    TEST_ASSERT_EQUAL(2, seq.getSequenceRepetition());
    TEST_ASSERT_EQUAL(SEQ_STATE_IDLE, seq.getState());
}

void test_delay_start() {
    ezLED led(13);
    ezLEDSequence seq(&led);
    
    seq.addStep(100, 50, 1);
    seq.start(1000);
    
    TEST_ASSERT_EQUAL(SEQ_STATE_DELAY, seq.getState());
    
    advanceMillis(500);
    seq.loop();
    TEST_ASSERT_EQUAL(SEQ_STATE_DELAY, seq.getState());
    
    advanceMillis(500);
    seq.loop();
    TEST_ASSERT_EQUAL(SEQ_STATE_STEP_ON, seq.getState());
}

void test_pause_from_invalid_states_does_nothing() {
    ezLED led(13);
    ezLEDSequence seq(&led);
    
    seq.pause();
    TEST_ASSERT_EQUAL(SEQ_STATE_IDLE, seq.getState());
    
    seq.addStep(100, 50, 1);
    seq.start(1000);
    
    seq.pause();
    TEST_ASSERT_EQUAL(SEQ_STATE_DELAY, seq.getState());
}

void test_resume_from_invalid_states_does_nothing() {
    ezLED led(13);
    ezLEDSequence seq(&led);
    
    seq.resume();
    TEST_ASSERT_EQUAL(SEQ_STATE_IDLE, seq.getState());
}

void test_step_repetitions() {
    ezLED led(13);
    ezLEDSequence seq(&led);
    
    seq.addStep(100, 50, 3);
    seq.startContinuous(1);
    
    for (int i = 0; i < 3; i++) {
        TEST_ASSERT_EQUAL(i + 1, seq.getCurrentStepRepetition());
        advanceMillis(100);
        seq.loop();
        advanceMillis(50);
        seq.loop();
    }
    
    TEST_ASSERT_EQUAL(1, seq.getCurrentStepIndex());
}

void test_onStepEndCallback() {
    ezLED led(13);
    ezLEDSequence seq(&led);
    
    bool callbackFired = false;
    seq.setOnStepEndCallback([&callbackFired](StepInfo info) {
        callbackFired = true;
        TEST_ASSERT_EQUAL(0, info.stepIndex);
        TEST_ASSERT_EQUAL(1, info.stepRepetition);
        TEST_ASSERT_EQUAL(1, info.stepTotalRepetitions);
    });
    
    seq.addStep(100, 50, 1);
    seq.startContinuous(1);
    
    advanceMillis(100);
    seq.loop();
    
    TEST_ASSERT_TRUE(callbackFired);
}

void test_onSequenceEndCallback() {
    ezLED led(13);
    ezLEDSequence seq(&led);
    
    bool callbackFired = false;
    seq.setOnSequenceEndCallback([&callbackFired](SequenceInfo info) {
        callbackFired = true;
    });
    
    seq.addStep(100, 50, 1);
    seq.startContinuous(1);
    
    advanceMillis(100);
    seq.loop();
    advanceMillis(50);
    seq.loop();
    
    TEST_ASSERT_TRUE(callbackFired);
}

void test_timed_mode_finishes_after_duration() {
    ezLED led(13);
    ezLEDSequence seq(&led);
    
    seq.addStep(100, 50, 1);
    seq.startTimed(200);
    
    TEST_ASSERT_EQUAL(SEQ_STATE_STEP_ON, seq.getState());
    
    advanceMillis(100);
    seq.loop();
    advanceMillis(50);
    seq.loop();
    advanceMillis(100);
    seq.loop();
    
    TEST_ASSERT_EQUAL(SEQ_STATE_IDLE, seq.getState());
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_constructor_initializes_correctly);
    RUN_TEST(test_addStep_increases_step_count);
    RUN_TEST(test_start_with_no_steps_does_nothing);
    RUN_TEST(test_startContinuous_starts_sequence);
    RUN_TEST(test_startContinuous_infinite_mode);
    RUN_TEST(test_startTimed_starts_sequence);
    RUN_TEST(test_pause_and_resume);
    RUN_TEST(test_stop_resets_state);
    RUN_TEST(test_cancel_clears_steps);
    RUN_TEST(test_clear_removes_steps);
    RUN_TEST(test_step_transitions);
    RUN_TEST(test_sequence_repetitions);
    RUN_TEST(test_delay_start);
    RUN_TEST(test_pause_from_invalid_states_does_nothing);
    RUN_TEST(test_resume_from_invalid_states_does_nothing);
    RUN_TEST(test_step_repetitions);
    RUN_TEST(test_onStepEndCallback);
    RUN_TEST(test_onSequenceEndCallback);
    RUN_TEST(test_timed_mode_finishes_after_duration);
    
    return UNITY_END();
}