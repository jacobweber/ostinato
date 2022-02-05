#pragma once

#include "State.h"
#include "Constants.h"
#include "Step.h"

class Stretcher {
public:
    explicit Stretcher(State &_state) : state(_state) {
    }

    struct StretchedResult {
        std::vector<CurrentStep> steps;
        stepnum_t numSteps = 0;
        voicenum_t numVoices = 0;
    };

    struct OrigStep {
        stepnum_t stepNum;
        double x;
        std::array<double, constants::MAX_VOICES> activeVoicesY;
        voicenum_t numActiveVoices;
        double length;
        double volume;
        int octave;
    };

    void setStepIndex(stepnum_t origStepIndex) {
        nextStepIndex = origStepIndex;
        reuseNextStep = false;
        tieActive = false;
    }

    voicenum_t getNumVoices() const {
        return numVoices;
    }

    stepnum_t getNumSteps() const {
        return numSteps;
    }

    stepnum_t getOrigStepIndex() const {
        return prev.stepNum;
    }

    stepnum_t getNextStepIndex() const {
        return nextStepIndex;
    }

    void setSkipLastStepIfMatchesFirst(bool skip) {
        skipLastStepIfMatchesFirst = skip;
    }

    static void getStretchedVoices(State &state, stepnum_t stepNum, size_t numHeldNotes, std::array<bool, MAX_ACTUAL_VOICES> &outVoices, voicenum_t &outNumVoices) {
        voicenum_t origNumVoices = static_cast<voicenum_t>(state.voicesParameter->getIndex()) + 1;
        outNumVoices = std::min(numHeldNotes, MAX_ACTUAL_VOICES);
        double origVoiceSizeY =
                numHeldNotes == 1 ? 1 : static_cast<double>(outNumVoices - 1) / static_cast<double>(origNumVoices - 1);

        for (voicenum_t voiceNum = 0; voiceNum < outNumVoices; voiceNum++) {
            outVoices[voiceNum] = false;
        }

        voicenum_t newVoiceNum = 0;
        for (voicenum_t origVoiceNum = 0; origVoiceNum < origNumVoices; origVoiceNum++) {
            if (state.stepState[stepNum].voiceParameters[origVoiceNum]->get()) {
                double curVoiceY = origVoiceSizeY * static_cast<double>(origVoiceNum);
                newVoiceNum = std::min(static_cast<voicenum_t>(std::round(curVoiceY)), outNumVoices - 1);
                outVoices[newVoiceNum] = true;
            }
        }
    }

    void getNextStretchedStep(size_t numHeldNotes, CurrentStep &outStep) {
        // with origNumSteps = 3 and numSteps = 5:
        // origStepSizeX = 2
        // stepNum and stepX  = 0 1 2 3 4
        // origStepX          = 0   2   4
        // origStepNum        = 0   1   2

        // with origNumSteps = 6 and numSteps = 3:
        // origStepSizeX = .4
        // stepNum and stepX  = 0          1           2
        // origStepX          = 0  0.4  0.8  1.2  1.6  2
        // origStepNum        = 0   1    2    3    4   5

        // mult = .3,  at 1,   1.15 intdiv .3 = 3, 3 * .3 = .9
        // mult = .3,  at .8,   .95 intdiv .3 = 3, 3 * .3 = .9
        // mult = .3,  at 1.1, 1.25 intdiv .3 = 4, 4 * .3 = 1.2

        recalcStretchInfo(numHeldNotes, static_cast<stepnum_t>(state.stepsParameter->getIndex()) + 1,
                          static_cast<voicenum_t>(state.voicesParameter->getIndex()) + 1);

        if (nextStepIndex >= numSteps) {
            nextStepIndex = 0;
        }

        if (!reuseNextStep || nextStepIndex == 0) {
            reuseNextStep = true;

            stepnum_t prevOrigStepNum;
            if (numSteps > origNumSteps) {
                prevOrigStepNum = static_cast<stepnum_t>((static_cast<double>(nextStepIndex + 1) + roundingOffset) /
                                                         origStepSizeX) - 1; // ugly
            } else {
                prevOrigStepNum = static_cast<stepnum_t>((static_cast<double>(nextStepIndex) + roundingOffset) /
                                                         origStepSizeX);
            }

            // will immediately become prev
            next.stepNum = prevOrigStepNum;
            next.x = static_cast<double>(next.stepNum) * origStepSizeX;
            if (!tieActive) {
                // when tied, keep next the same as prev
                updateOrigStepFromState(next, state, next.stepNum);
            }
        }

        auto nextOrigStepNum = static_cast<stepnum_t>((static_cast<double>(nextStepIndex + 1) + roundingOffset) /
                                                      origStepSizeX);

        if (nextOrigStepNum > next.stepNum) { // passed the next original step, so recalc things
            prev = next;
            next.stepNum = nextOrigStepNum;
            next.x = static_cast<double>(next.stepNum) * origStepSizeX;

            tieActive = state.stepState[prev.stepNum].tieParameter->get();
            if (next.stepNum == origNumSteps) {
                // when on last original step, act as if there's one more step with the same settings
            } else if (tieActive) {
                // when tied, keep next the same as prev
            } else {
                updateOrigStepFromState(next, state, next.stepNum);
            }
        }

        DBG("stretched step " << nextStepIndex << " (orig: " << prev.stepNum << "-" << next.stepNum << ", X: "
                              << prev.x << "-" << next.x << ")");
        updateStretchedStep(nextStepIndex, outStep);

        nextStepIndex++;
        if (nextStepIndex >= numSteps) {
            nextStepIndex = 0;
        }
    }

    StretchedResult stretch(size_t numHeldNotes) {
        return stretch(numHeldNotes, 0);
    }

    StretchedResult stretch(size_t numHeldNotes, stepnum_t generateSteps) {
        recalcStretchInfo(numHeldNotes, static_cast<stepnum_t>(state.stepsParameter->getIndex()) + 1,
                          static_cast<voicenum_t>(state.voicesParameter->getIndex()) + 1);

        if (generateSteps == 0) generateSteps = numSteps;

        StretchedResult result;
        for (stepnum_t stepNum = 0; stepNum < generateSteps; stepNum++) {
            CurrentStep currentStep;
            getNextStretchedStep(numNotes, currentStep);
            result.steps.push_back(currentStep);
        }

        result.numSteps = numSteps;
        result.numVoices = numVoices;
        return result;
    }

private:
    void recalcStretchInfo(size_t _numNotes, stepnum_t _origNumSteps, voicenum_t _origNumVoices) {
        if (numNotes == _numNotes && origNumSteps == _origNumSteps && origNumVoices == _origNumVoices
            && (!skipLastStepIfMatchesFirst || nextStepIndex < numSteps - 1))
            return;
        numNotes = _numNotes;
        origNumSteps = _origNumSteps;
        origNumVoices = _origNumVoices;

        numVoices = std::min(numNotes, MAX_ACTUAL_VOICES);
        // we'll round up; extra steps will make it more accurate
        numSteps = static_cast<stepnum_t>(1 + std::ceil(
                static_cast<double>((origNumSteps - 1) * (numNotes - 1)) / static_cast<double>(origNumVoices - 1)));
        // avoid dividing by 0 here and later
        origStepSizeX =
                numNotes == 1 ? 1 : static_cast<double>(numSteps - 1) / static_cast<double>(origNumSteps - 1);
        origVoiceSizeY =
                numNotes == 1 ? 1 : static_cast<double>(numVoices - 1) / static_cast<double>(origNumVoices - 1);
        roundingOffset = numSteps > origNumSteps ? origStepSizeX - 1 : origStepSizeX / 2;

        bool skipLast = skipLastStepIfMatchesFirst && firstLastOrigStepsSame();
        if (skipLast) numSteps--;
        DBG("stretching " << origNumSteps << "x" << origNumVoices << " to " << numSteps << "x" << numVoices
                          << (skipLast ? " (skip last)" : ""));
    }

    bool firstLastOrigStepsSame() {
        for (voicenum_t voiceNum = 0; voiceNum < origNumVoices; voiceNum++) {
            if (state.stepState[0].voiceParameters[voiceNum]->get() !=
                state.stepState[origNumSteps - 1].voiceParameters[voiceNum]->get())
                return false;
        }
        return true;
    }

    void updateStretchedStep(stepnum_t stepNum, CurrentStep &outStep) {
        for (voicenum_t voiceNum = 0; voiceNum < numVoices; voiceNum++) {
            outStep.voices[voiceNum] = false;
        }

        auto curStepX = static_cast<double>(stepNum);

        // with origNumVoices = 3 and numVoices = 5:
        // origVoiceSize = 2
        // prev: new:  next:     prev: new:  next:  prevY: curY:
        // -     .     *         -     .     *      4      4
        //       .  /                  . ---               3
        // -     *     *         * --- *     -      2      2
        //    /  .                  /  .                   1
        // *     .     -         * --- *     -      0      0

        // start at top
        // find another prev-outStep voice that's on, and another next-outStep voice that's on
        // if we have both, draw line between them
        // if we have a prev voice but not a next one, draw horizontal line
        // if we have a next voice but not a prev one, ignore it
        // find this outStep's Y position on that line, and round it to a voice num for this outStep
        for (voicenum_t lineNum = 0; lineNum < prev.numActiveVoices; lineNum++) {
            double prevVoiceY = prev.activeVoicesY[lineNum];
            double nextVoiceY =
                    lineNum < next.numActiveVoices ? next.activeVoicesY[lineNum] : prevVoiceY;
            double slope = (nextVoiceY - prevVoiceY) / (next.x - prev.x);
            double curVoiceY = prevVoiceY + slope * (curStepX - prev.x);
            DBG("  voice " << curVoiceY << " (orig: " << prevVoiceY << "-" << nextVoiceY << ")");
            voicenum_t voiceNum = std::min(static_cast<voicenum_t>(std::round(curVoiceY)), numVoices - 1);
            outStep.voices[voiceNum] = true;
        }

        double lengthSlope = (next.length - prev.length) / (next.x - prev.x);
        outStep.length = prev.length + lengthSlope * (curStepX - prev.x);

        double volSlope = (next.volume - prev.volume) / (next.x - prev.x);
        outStep.volume = prev.volume + volSlope * (curStepX - prev.x);

        outStep.octave = prev.octave;
        outStep.power = true;
        outStep.tie = false;
    }

    void updateOrigStepFromState(OrigStep &outStep, State &_state, stepnum_t stepNum) const {
        outStep.numActiveVoices = 0;
        if (_state.stepState[stepNum].powerParameter->get()) {
            for (voicenum_t origVoiceNum = 0; origVoiceNum < origNumVoices; origVoiceNum++) {
                if (_state.stepState[stepNum].voiceParameters[origVoiceNum]->get()) {
                    outStep.activeVoicesY[outStep.numActiveVoices++] =
                            origVoiceSizeY * static_cast<double>(origVoiceNum);
                }
            }
        }
        outStep.length = _state.stepState[stepNum].lengthParameter->get();
        outStep.volume = _state.stepState[stepNum].volParameter->get();
        outStep.octave = _state.stepState[stepNum].octaveParameter->getIndex();
    }

private:
    State &state;

    bool skipLastStepIfMatchesFirst = true;
    voicenum_t numVoices = 0;
    stepnum_t numSteps = 0;
    size_t numNotes = 0;
    OrigStep prev{};
    OrigStep next{};

    stepnum_t origNumSteps = 0;
    voicenum_t origNumVoices = 0;
    double origStepSizeX = 0.0;
    double origVoiceSizeY = 0.0;
    double roundingOffset = 0.0;

    bool tieActive = false;
    stepnum_t nextStepIndex = 0;
    bool reuseNextStep = false;
};
