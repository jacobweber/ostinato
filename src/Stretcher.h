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

    StretchedResult stretch(State &state, size_t numNotes) {
        // with oldNumSteps = 3 and numSteps = 5:
        // oldStepSizeX = 2
        // stepNum and stepX = 0 1 2 3 4
        // oldStepX          = 0   2   4
        // oldStepNum        = 0   1   2

        // with oldNumSteps = 6 and numSteps = 3:
        // oldStepSizeX = .4
        // stepNum and stepX = 0          1           2
        // oldStepX          = 0  0.4  0.8  1.2  1.6  2
        // oldStepNum        = 0   1    2    3    4   5

        // mult = .3,  at 1,   1.15 intdiv .3 = 3, 3 * .3 = .9
        // mult = .3,  at .8,   .95 intdiv .3 = 3, 3 * .3 = .9
        // mult = .3,  at 1.1, 1.25 intdiv .3 = 4, 4 * .3 = 1.2

        size_t oldNumSteps = static_cast<size_t>(state.stepsParameter->getIndex() + 1);
        size_t oldNumVoices = static_cast<size_t>(state.voicesParameter->getIndex() + 1);
        // we'll round up; extra steps will make it more accurate
        size_t numSteps = static_cast<size_t>(1 + std::ceil(
                static_cast<double>((oldNumSteps - 1) * (numNotes - 1)) / (oldNumVoices - 1)));
        size_t numVoices = numNotes;

        StretchedResult result;
        result.numSteps = numSteps;
        result.numVoices = numVoices;
        result.steps.reserve(numSteps);

        double oldStepSizeX = static_cast<double>(numSteps - 1) / static_cast<double>(oldNumSteps - 1);
        double oldVoiceSizeY = static_cast<double>(numVoices - 1) / static_cast<double>(oldNumVoices - 1);
        double roundingOffset = numSteps > oldNumSteps ? oldStepSizeX - 1 : oldStepSizeX / 2;

        size_t prevOldStepNum = 0;
        // find closest old step to next new step (1)
        size_t nextOldStepNum = static_cast<size_t>((1 + roundingOffset) / oldStepSizeX);

        double prevOldStepX = 0;
        double nextOldStepX = nextOldStepNum * oldStepSizeX;
        std::vector<double> prevOldStepActiveVoicesY{};
        prevOldStepActiveVoicesY.reserve(props::MAX_VOICES);
        std::vector<double> nextOldStepActiveVoicesY{};
        nextOldStepActiveVoicesY.reserve(props::MAX_VOICES);

        for (size_t prevOldVoiceNum = 0; prevOldVoiceNum < oldNumVoices; prevOldVoiceNum++) {
            if (state.stepState[prevOldStepNum].voiceParameters[prevOldVoiceNum]->get()) {
                prevOldStepActiveVoicesY.push_back(oldVoiceSizeY * (oldNumVoices - 1 - prevOldVoiceNum));
            }
        }

        if (nextOldStepNum == oldNumSteps) {
            // when on last old step, act as if there's one more step with the same voices
            nextOldStepActiveVoicesY = prevOldStepActiveVoicesY;
        } else {
            for (size_t nextOldVoiceNum = 0; nextOldVoiceNum < oldNumVoices; nextOldVoiceNum++) {
                if (state.stepState[nextOldStepNum].voiceParameters[nextOldVoiceNum]->get()) {
                    nextOldStepActiveVoicesY.push_back(oldVoiceSizeY * (oldNumVoices - 1 - nextOldVoiceNum));
                }
            }
        }

        for (size_t stepNum = 0; stepNum < result.numSteps; stepNum++) {
            double curStepX = static_cast<double>(stepNum);

            // find closest old step to next new step
            size_t updatedNextOldStepNum = static_cast<size_t>((stepNum + 1 + roundingOffset) / oldStepSizeX);

            if (updatedNextOldStepNum > nextOldStepNum) { // passed the next old step, so recalc things
                prevOldStepNum = nextOldStepNum;
                nextOldStepNum = updatedNextOldStepNum;
                prevOldStepX = nextOldStepX;
                nextOldStepX = nextOldStepNum * oldStepSizeX;

                prevOldStepActiveVoicesY = nextOldStepActiveVoicesY;
                if (nextOldStepNum == oldNumSteps) {
                    // when on last old step, act as if there's one more step with the same voices
                } else {
                    nextOldStepActiveVoicesY.clear();
                    for (size_t nextOldVoiceNum = 0; nextOldVoiceNum < oldNumVoices; nextOldVoiceNum++) {
                        if (state.stepState[nextOldStepNum].voiceParameters[nextOldVoiceNum]->get()) {
                            nextOldStepActiveVoicesY.push_back(oldVoiceSizeY * (oldNumVoices - 1 - nextOldVoiceNum));
                        }
                    }
                }
            }
            DBG("--- step " << curStepX << " (old: " << prevOldStepNum << " - " << nextOldStepNum << ", X: "
                            << prevOldStepX << " - " << nextOldStepX << ")");

            StretchedStep step;
            step.voices.insert(step.voices.end(), numVoices, false);
            step.length = state.stepState[prevOldStepNum].lengthParameter->get(); // should scale
            step.volume = state.stepState[prevOldStepNum].volParameter->get(); // should scale

            // with oldNumVoices = 3 and numVoices = 5:
            // oldVoiceSize = 2
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
            for (size_t lineNum = 0; lineNum < prevOldStepActiveVoicesY.size(); lineNum++) {
                double prevVoiceY = prevOldStepActiveVoicesY[lineNum];
                double nextVoiceY =
                        lineNum < nextOldStepActiveVoicesY.size() ? nextOldStepActiveVoicesY[lineNum] : prevVoiceY;
                double slope = (nextVoiceY - prevVoiceY) / (nextOldStepX - prevOldStepX);
                double curVoiceY = prevVoiceY + slope * (curStepX - prevOldStepX);
                DBG("   voice " << curVoiceY << " (old: " << prevVoiceY << " - " << nextVoiceY);
                size_t voiceNum =
                        numVoices - 1 - std::min(static_cast<size_t>(std::round(curVoiceY)), numVoices - 1);
                step.voices[voiceNum] = true;
            }
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
