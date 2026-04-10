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
 * DISCLAIMED. IN NO EVENT SHALL ARDUINOGETSTARTED.COM BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <ezLEDSequence.h>

ezLEDSequence::ezLEDSequence(ezLED* led) {
    _led = led;
    _steps.clear();
    
    _sequenceMode = SEQ_MODE_CONTINUOUS;
    _sequenceRepetitions = 0;
    _sequenceDuration = 0;
    _delayTime = 0;
    
    _sequenceState = SEQ_STATE_IDLE;
    _lastTime = 0;
    _sequenceTimer = 0;
    
    _currentStepIndex = 0;
    _currentStepRepetition = 0;
    _currentSequenceRepetition = 0;
    
    _stepOnTime = 0;
    _stepOffTime = 0;
    
    _onStepEndCallback = nullptr;
    _onSequenceEndCallback = nullptr;
}

void ezLEDSequence::addStep(unsigned long duration, unsigned long pause, unsigned int repetitions, int intensity) {
    Step step;
    step.duration = duration;
    step.pause = pause;
    step.intensity = intensity;
    step.repetitions = repetitions;
    _steps.push_back(step);
}

void ezLEDSequence::clear() {
    _steps.clear();
    stop();
}

void ezLEDSequence::startContinuous(unsigned int repetitions) {
    _sequenceMode = SEQ_MODE_CONTINUOUS;
    _sequenceRepetitions = repetitions;
    start();
}

void ezLEDSequence::startTimed(unsigned long durationMs) {
    _sequenceMode = SEQ_MODE_TIMED;
    _sequenceDuration = durationMs;
    start();
}

void ezLEDSequence::start(unsigned long delayTime) {
    if (_steps.empty()) {
        return;
    }
    
    _delayTime = delayTime;
    _currentStepIndex = 0;
    _currentStepRepetition = 3;
    _currentSequenceRepetition = 0;
    
    if (_delayTime > 0) {
        _sequenceState = SEQ_STATE_DELAY;
        _lastTime = millis();
    } else {
        _sequenceState = SEQ_STATE_RUNNING;
        _lastTime = millis();
        _sequenceTimer = millis();
        _currentSequenceRepetition = 1;
        executeStep();
    }
}

void ezLEDSequence::pause() {
    if (_sequenceState == SEQ_STATE_RUNNING || 
        _sequenceState == SEQ_STATE_STEP_ON || 
        _sequenceState == SEQ_STATE_STEP_OFF) {
        _sequenceState = SEQ_STATE_PAUSED;
    }
}

void ezLEDSequence::resume() {
    if (_sequenceState == SEQ_STATE_PAUSED) {
        _sequenceState = SEQ_STATE_RUNNING;
        _lastTime = millis();
    }
}

void ezLEDSequence::stop() {
    _sequenceState = SEQ_STATE_IDLE;
    _led->turnOFF();
}

void ezLEDSequence::cancel() {
    stop();
    _steps.clear();
    _currentStepIndex = 0;
    _currentStepRepetition = 0;
    _currentSequenceRepetition = 0;
}

void ezLEDSequence::setOnStepEndCallback(std::function<void(StepInfo)> callback) {
    _onStepEndCallback = callback;
}

void ezLEDSequence::setOnSequenceEndCallback(std::function<void(SequenceInfo)> callback) {
    _onSequenceEndCallback = callback;
}

unsigned char ezLEDSequence::getState() {
    return _sequenceState;
}

int ezLEDSequence::getCurrentStepIndex() {
    return _currentStepIndex;
}

int ezLEDSequence::getCurrentStepRepetition() {
    return _currentStepRepetition;
}

int ezLEDSequence::getSequenceRepetition() {
    return _currentSequenceRepetition;
}

int ezLEDSequence::getTotalSteps() {
    return _steps.size();
}

void ezLEDSequence::executeStep() {
    if (_currentStepIndex >= (int)_steps.size()) {
        finishSequence();
        return;
    }
    
    Step& step = _steps[_currentStepIndex];
    _stepOnTime = step.duration;
    _stepOffTime = step.pause;
    _currentStepRepetition = 1;
    
    if (step.intensity >= 0) {
        _led->setBrightness(step.intensity);
    }
    _led->turnON();
    _sequenceState = SEQ_STATE_STEP_ON;
    _lastTime = millis();
}

void ezLEDSequence::nextStep() {
    if (_onStepEndCallback) {
        Step& step = _steps[_currentStepIndex];
        StepInfo info;
        info.stepIndex = _currentStepIndex;
        info.stepRepetition = _currentStepRepetition;
        info.stepTotalRepetitions = step.repetitions;
        info.duration = step.duration;
        info.pause = step.pause;
        info.intensity = step.intensity;
        _onStepEndCallback(info);
    }
    
    if (_currentStepRepetition >= (int)_steps[_currentStepIndex].repetitions) {
        _currentStepIndex++;
    } else {
        _currentStepRepetition++;
    }
    
    if (_currentStepIndex >= (int)_steps.size()) {
        _currentStepIndex = 0;
        
        if (_sequenceMode == SEQ_MODE_CONTINUOUS && _sequenceRepetitions == 0) {
            _currentSequenceRepetition++;
            executeStep();
        } else if (_sequenceMode == SEQ_MODE_CONTINUOUS && _currentSequenceRepetition < (int)_sequenceRepetitions) {
            _currentSequenceRepetition++;
            executeStep();
        } else if (_sequenceMode == SEQ_MODE_TIMED) {
            if ((unsigned long)(millis() - _sequenceTimer) >= _sequenceDuration) {
                finishSequence();
            } else {
                _currentSequenceRepetition++;
                executeStep();
            }
        } else {
            finishSequence();
        }
    } else {
        _led->turnOFF();
        _sequenceState = SEQ_STATE_STEP_OFF;
        _lastTime = millis();
    }
}

void ezLEDSequence::finishSequence() {
    _led->turnOFF();
    _sequenceState = SEQ_STATE_IDLE;
    
    if (_onSequenceEndCallback) {
        SequenceInfo info;
        info.currentStepIndex = _currentStepIndex;
        info.currentStepRepetition = _currentStepRepetition;
        info.sequenceRepetition = _currentSequenceRepetition;
        info.totalSteps = _steps.size();
        _onSequenceEndCallback(info);
    }
}

void ezLEDSequence::loop() {
    if (_sequenceState == SEQ_STATE_IDLE) {
        return;
    }
    
    if (_sequenceState == SEQ_STATE_DELAY) {
        if ((unsigned long)(millis() - _lastTime) >= _delayTime) {
            _sequenceState = SEQ_STATE_RUNNING;
            _lastTime = millis();
            _sequenceTimer = millis();
            _currentSequenceRepetition = 1;
            executeStep();
        }
        return;
    }
    
    if (_sequenceState == SEQ_STATE_PAUSED) {
        return;
    }
    
    if (_sequenceState == SEQ_STATE_STEP_ON) {
        if ((unsigned long)(millis() - _lastTime) >= _stepOnTime) {
            _led->turnOFF();
            _sequenceState = SEQ_STATE_STEP_OFF;
            _lastTime = millis();
            nextStep();
            if (_sequenceState != SEQ_STATE_IDLE) {
                executeStep();
            }
        }
        return;
    }
    
    if (_sequenceState == SEQ_STATE_STEP_OFF) {
        if ((unsigned long)(millis() - _lastTime) >= _stepOffTime) {
            nextStep();
            if (_sequenceState != SEQ_STATE_IDLE) {
                executeStep();
            }
        }
        return;
    }
    
    if (_sequenceMode == SEQ_MODE_TIMED && _sequenceState == SEQ_STATE_RUNNING) {
        if ((unsigned long)(millis() - _sequenceTimer) >= _sequenceDuration) {
            if (_sequenceState == SEQ_STATE_STEP_ON || _sequenceState == SEQ_STATE_STEP_OFF) {
                return;
            }
            finishSequence();
        }
    }
}
