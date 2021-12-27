#pragma once

#include "State.h"
#include "Props.h"

class Stretcher {
public:
    struct StretchedStep {
        std::vector<bool> voices;
        double length;
        double volume;
    };
    struct StretchedResult {
        std::vector<StretchedStep> steps;
        size_t numSteps;
        size_t numVoices;
    };
    struct OrigStep {
        size_t stepNum;
        double x;
        std::vector<double> activeVoicesY{};
        double length;
        double volume;
    };

    StretchedResult stretch(State &state, size_t numNotes) {
        // with origNumSteps = 3 and numSteps = 5:
        // origStepSizeX = 2
        // stepNum and stepX = 0 1 2 3 4
        // origStepX          = 0   2   4
        // origStepNum        = 0   1   2

        // with origNumSteps = 6 and numSteps = 3:
        // origStepSizeX = .4
        // stepNum and stepX = 0          1           2
        // origStepX          = 0  0.4  0.8  1.2  1.6  2
        // origStepNum        = 0   1    2    3    4   5

        // mult = .3,  at 1,   1.15 intdiv .3 = 3, 3 * .3 = .9
        // mult = .3,  at .8,   .95 intdiv .3 = 3, 3 * .3 = .9
        // mult = .3,  at 1.1, 1.25 intdiv .3 = 4, 4 * .3 = 1.2

        size_t origNumSteps = static_cast<size_t>(state.stepsParameter->getIndex() + 1);
        size_t origNumVoices = static_cast<size_t>(state.voicesParameter->getIndex() + 1);
        // we'll round up; extra steps will make it more accurate
        size_t numSteps = static_cast<size_t>(1 + std::ceil(
                static_cast<double>((origNumSteps - 1) * (numNotes - 1)) / (origNumVoices - 1)));
        size_t numVoices = numNotes;

        StretchedResult result;
        result.numSteps = numSteps;
        result.numVoices = numVoices;
        result.steps.reserve(numSteps);

        double origStepSizeX = static_cast<double>(numSteps - 1) / static_cast<double>(origNumSteps - 1);
        double origVoiceSizeY = static_cast<double>(numVoices - 1) / static_cast<double>(origNumVoices - 1);
        double roundingOffset = numSteps > origNumSteps ? origStepSizeX - 1 : origStepSizeX / 2;

        OrigStep prev;
        OrigStep next;
        prev.stepNum = 0;
        // find closest original step to next new step (1)
        next.stepNum = static_cast<size_t>((1 + roundingOffset) / origStepSizeX);

        prev.x = 0;
        next.x = next.stepNum * origStepSizeX;
        prev.activeVoicesY.reserve(props::MAX_VOICES);
        next.activeVoicesY.reserve(props::MAX_VOICES);

        if (state.stepState[prev.stepNum].powerParameter->get()) {
            for (size_t origVoiceNum = 0; origVoiceNum < origNumVoices; origVoiceNum++) {
                if (state.stepState[prev.stepNum].voiceParameters[origVoiceNum]->get()) {
                    prev.activeVoicesY.push_back(origVoiceSizeY * (origNumVoices - 1 - origVoiceNum));
                }
            }
        }

        if (next.stepNum == origNumSteps) {
            // when on last original step, act as if there's one more step with the same voices
            next.activeVoicesY = prev.activeVoicesY;
        } else if (state.stepState[prev.stepNum].tieParameter->get()) {
            next.activeVoicesY = prev.activeVoicesY;
        } else {
            if (state.stepState[next.stepNum].powerParameter->get()) {
                for (size_t origVoiceNum = 0; origVoiceNum < origNumVoices; origVoiceNum++) {
                    if (state.stepState[next.stepNum].voiceParameters[origVoiceNum]->get()) {
                        next.activeVoicesY.push_back(origVoiceSizeY * (origNumVoices - 1 - origVoiceNum));
                    }
                }
            }
        }

        prev.length = state.stepState[prev.stepNum].lengthParameter->get();
        next.length = state.stepState[next.stepNum].lengthParameter->get();

        prev.volume = state.stepState[prev.stepNum].volParameter->get();
        next.volume = state.stepState[next.stepNum].volParameter->get();

        for (size_t stepNum = 0; stepNum < result.numSteps; stepNum++) {
            double curStepX = static_cast<double>(stepNum);

            // find closest original step to next new step
            size_t nextStepNum = static_cast<size_t>((stepNum + 1 + roundingOffset) / origStepSizeX);

            if (nextStepNum > next.stepNum) { // passed the next original step, so recalc things
                prev = next;
                next.stepNum = nextStepNum;
                next.x = next.stepNum * origStepSizeX;

                if (next.stepNum == origNumSteps) {
                    // when on last original step, act as if there's one more step with the same voices etc.
                } else if (state.stepState[prev.stepNum].tieParameter->get()) {
                    // keep next the same as prev
                } else {
                    next.activeVoicesY.clear();
                    if (state.stepState[next.stepNum].powerParameter->get()) {
                        for (size_t origVoiceNum = 0; origVoiceNum < origNumVoices; origVoiceNum++) {
                            if (state.stepState[next.stepNum].voiceParameters[origVoiceNum]->get()) {
                                next.activeVoicesY.push_back(
                                        origVoiceSizeY * (origNumVoices - 1 - origVoiceNum));
                            }
                        }
                    }

                    next.length = state.stepState[next.stepNum].lengthParameter->get();
                    next.volume = state.stepState[next.stepNum].volParameter->get();
                }
            }
            DBG("--- step " << curStepX << " (orig: " << prev.stepNum << " - " << next.stepNum << ", X: "
                            << prev.x << " - " << next.x << ")");

            StretchedStep step;
            step.voices.insert(step.voices.end(), numVoices, false);

            // with origNumVoices = 3 and numVoices = 5:
            // origVoiceSize = 2
            // prev: new:  next:     prev: new:  next:  prevY: curY:
            // -     .     *         -     .     *      4      4
            //       .  /                  . ---               3
            // -     *     *         * --- *     -      2      2
            //    /  .                  /  .                   1
            // *     .     -         * --- *     -      0      0

            // start at top
            // find another prev-step voice that's on, and another next-step voice that's on
            // if we have both, draw line between them
            // if we have a prev voice but not a next one, draw horizontal line
            // if we have a next voice but not a prev one, ignore it
            // find this step's Y position on that line, and round it to a voice num for this step
            for (size_t lineNum = 0; lineNum < prev.activeVoicesY.size(); lineNum++) {
                double prevVoiceY = prev.activeVoicesY[lineNum];
                double nextVoiceY =
                        lineNum < next.activeVoicesY.size() ? next.activeVoicesY[lineNum] : prevVoiceY;
                double slope = (nextVoiceY - prevVoiceY) / (next.x - prev.x);
                double curVoiceY = prevVoiceY + slope * (curStepX - prev.x);
                DBG("   voice " << curVoiceY << " (orig: " << prevVoiceY << " - " << nextVoiceY << ")");
                size_t voiceNum =
                        numVoices - 1 - std::min(static_cast<size_t>(std::round(curVoiceY)), numVoices - 1);
                step.voices[voiceNum] = true;
            }

            double lengthSlope = (next.length - prev.length) / (next.x - prev.x);
            step.length = prev.length + lengthSlope * (curStepX - prev.x);

            double volSlope = (next.volume - prev.volume) / (next.x - prev.x);
            step.volume = prev.volume + volSlope * (curStepX - prev.x);

            result.steps.push_back(step);
        }

        return result;
    }

    juce::String distribute(int numBalls, int numBuckets) {
        char buckets[100];
        for (int i = 0; i < numBuckets; i++) {
            buckets[i] = '.';
        }
        buckets[numBuckets] = '\0';

        // place first
        if (numBalls > 0) {
            buckets[0] = 48 + 1;
            if (numBalls > 1 && numBuckets > 1) {
                // place last
                buckets[numBuckets - 1] = static_cast<char>(48 + numBalls);

                int numFreeBalls = numBalls - 2;
                int numFreeBuckets = numBuckets - 2;
                if (numFreeBalls > 0 && numFreeBuckets > 0) {
                    for (int ballNum = 1; ballNum <= numBalls - 2; ballNum++) {
                        int bucketNum = static_cast<int>(1 +
                                                         std::floor(
                                                                 (ballNum - 1 + 0.5) * numFreeBuckets / numFreeBalls));
                        if (bucketNum < numBuckets) {
                            buckets[bucketNum] = static_cast<char>(48 + ballNum + 1);
                        }
                    }
                }
            }
        }
        return juce::String(buckets);
    }
};
