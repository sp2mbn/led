/*
 * Copyright (c) 2021, ArduinoGetStarted.com. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of the ArduinoGetStarted.com nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ARDUINOGETSTARTED.COM "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ARDUINOGETSTARTED.COM BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ezLEDSequence_h
#define ezLEDSequence_h

#include <Arduino.h>
#include <ezLED.h>
#include <vector>

#define SEQ_MODE_CONTINUOUS 0
#define SEQ_MODE_TIMED      1

#define SEQ_STATE_IDLE      0
#define SEQ_STATE_DELAY    1
#define SEQ_STATE_RUNNING   2
#define SEQ_STATE_PAUSED   3
#define SEQ_STATE_STEP_ON   4
#define SEQ_STATE_STEP_OFF 5

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

class ezLEDSequence
{
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
