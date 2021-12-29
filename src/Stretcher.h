#pragma once

#include "State.h"
#include "Props.h"
#include "Step.h"

class Stretcher {
public:
    Stretcher(State &_state, bool skipLast) : nextStepIndex(0), state(_state), reuseNextStep(false),
                                              skipLastStepIfMatchesFirst(skipLast) {
    }

    struct StretchedResult {
        std::vector<Step> steps;
        size_t numSteps;
        size_t numVoices;
    };

    struct OrigStep {
        size_t stepNum;
        double x;
        std::array<double, props::MAX_VOICES> activeVoicesY;
        size_t numActiveVoices;
        double length;
        double volume;
        int octave;
    };

    void setStepIndex(size_t origStepIndex) {
        nextStepIndex = origStepIndex;
        reuseNextStep = false;
    }

    void getNextStretchedStep(size_t numHeldNotes, Step &outStep) {
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

        recalcStretchInfo(numHeldNotes, static_cast<size_t>(state.stepsParameter->getIndex() + 1),
                          static_cast<size_t>(state.voicesParameter->getIndex() + 1));
        if (skipLastStepIfMatchesFirst && firstLastOrigStepsSame()) numSteps--;

        if (nextStepIndex >= numSteps) {
            nextStepIndex = 0;
            reuseNextStep = false;
        }

        if (!reuseNextStep) {
            reuseNextStep = true;

            size_t prevOrigStepNum;
            if (numSteps > origNumSteps) {
                prevOrigStepNum = static_cast<size_t>((nextStepIndex + 1 + roundingOffset) /
                                                      origStepSizeX) - 1; // ugly
            } else {
                prevOrigStepNum = static_cast<size_t>((nextStepIndex + roundingOffset) / origStepSizeX);
            }

            // will immediately become prev
            next.stepNum = prevOrigStepNum;
            next.x = next.stepNum * origStepSizeX;
            updateOrigStepFromState(next, state, next.stepNum);
        }

        size_t nextOrigStepNum = static_cast<size_t>((nextStepIndex + 1 + roundingOffset) /
                                                     origStepSizeX);

        if (nextOrigStepNum > next.stepNum) { // passed the next original step, so recalc things
            prev = next;
            next.stepNum = nextOrigStepNum;
            next.x = next.stepNum * origStepSizeX;

            if (next.stepNum == origNumSteps) {
                // when on last original step, act as if there's one more step with the same settings
            } else if (state.stepState[prev.stepNum].tieParameter->get()) {
                // keep next the same as prev
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
            reuseNextStep = false;
        }
    }

    StretchedResult stretch(size_t numHeldNotes) {
        recalcStretchInfo(numHeldNotes, static_cast<size_t>(state.stepsParameter->getIndex() + 1),
                          static_cast<size_t>(state.voicesParameter->getIndex() + 1));

        StretchedResult result;
        result.steps.reserve(numSteps);
        result.numSteps = numSteps;
        result.numVoices = numVoices;

        reset(0);
        for (size_t stepNum = 0; stepNum < numSteps; stepNum++) {
            Step currentStep;
            getNextStretchedStep(numNotes, currentStep);
            result.steps.push_back(currentStep);
        }

        return result;
    }

private:
    void recalcStretchInfo(size_t _numNotes, size_t _origNumSteps, size_t _origNumVoices) {
        if (numNotes == _numNotes && origNumSteps == _origNumSteps && origNumVoices == _origNumVoices) return;
        numNotes = _numNotes;
        origNumSteps = _origNumSteps;
        origNumVoices = _origNumVoices;

        numVoices = std::min(numNotes, MAX_ACTUAL_VOICES);
        // we'll round up; extra steps will make it more accurate
        numSteps = static_cast<size_t>(1 + std::ceil(
                static_cast<double>((origNumSteps - 1) * (numNotes - 1)) / (origNumVoices - 1)));
        // avoid dividing by 0 here and later
        origStepSizeX =
                numNotes == 1 ? 1 : static_cast<double>(numSteps - 1) / static_cast<double>(origNumSteps - 1);
        origVoiceSizeY =
                numNotes == 1 ? 1 : static_cast<double>(numVoices - 1) / static_cast<double>(origNumVoices - 1);
        roundingOffset = numSteps > origNumSteps ? origStepSizeX - 1 : origStepSizeX / 2;
        DBG("stretching " << origNumSteps << "x" << origNumVoices << " to " << numSteps << "x" << numVoices);
    }

    bool firstLastOrigStepsSame() {
        for (size_t voiceNum = 0; voiceNum < origNumVoices; voiceNum++) {
            if (state.stepState[0].voiceParameters[voiceNum]->get() !=
                state.stepState[origNumSteps - 1].voiceParameters[voiceNum]->get())
                return false;
        }
        return true;
    }

    void updateStretchedStep(size_t stepNum, Step &outStep) {
        for (size_t voiceNum = 0; voiceNum < numVoices; voiceNum++) {
            outStep.voices[voiceNum] = false;
        }

        double curStepX = static_cast<double>(stepNum);

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
        for (size_t lineNum = 0; lineNum < prev.numActiveVoices; lineNum++) {
            double prevVoiceY = prev.activeVoicesY[lineNum];
            double nextVoiceY =
                    lineNum < next.numActiveVoices ? next.activeVoicesY[lineNum] : prevVoiceY;
            double slope = (nextVoiceY - prevVoiceY) / (next.x - prev.x);
            double curVoiceY = prevVoiceY + slope * (curStepX - prev.x);
            DBG("  voice " << curVoiceY << " (orig: " << prevVoiceY << " - " << nextVoiceY << ")");
            size_t voiceNum = numVoices - 1 - std::min(static_cast<size_t>(std::round(curVoiceY)), numVoices - 1);
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

    void updateOrigStepFromState(OrigStep &outStep, State &_state, size_t stepNum) {
        outStep.numActiveVoices = 0;
        if (_state.stepState[stepNum].powerParameter->get()) {
            for (size_t origVoiceNum = 0; origVoiceNum < origNumVoices; origVoiceNum++) {
                if (_state.stepState[stepNum].voiceParameters[origVoiceNum]->get()) {
                    outStep.activeVoicesY[outStep.numActiveVoices++] =
                            origVoiceSizeY * (origNumVoices - 1 - origVoiceNum);
                }
            }
        }
        outStep.length = _state.stepState[stepNum].lengthParameter->get();
        outStep.volume = _state.stepState[stepNum].volParameter->get();
        outStep.octave = _state.stepState[stepNum].octaveParameter->getIndex();
    }

public:
    size_t nextStepIndex;
    size_t numVoices;
    size_t numSteps;
    size_t numNotes;
    OrigStep prev;
    OrigStep next;

private:
    State &state;
    bool reuseNextStep;
    bool skipLastStepIfMatchesFirst;
    size_t origNumSteps;
    size_t origNumVoices;
    double origStepSizeX;
    double origVoiceSizeY;
    double roundingOffset;
};
