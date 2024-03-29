#include "Stretcher.h"
#include "State.h"
#include "Constants.h"
#include "Step.h"

Stretcher::Stretcher(State &_state) : state(_state) {
}

void Stretcher::setStepNum(int origStepNum) {
    nextStepNum = origStepNum;
    reuseNextStep = false;
    tieActive = false;
}

int Stretcher::getNumVoices() const {
    return numVoices;
}

int Stretcher::getNumSteps() const {
    return numSteps;
}

int Stretcher::getOrigStepNum() const {
    return prev.stepNum;
}

int Stretcher::getNextStepNum() const {
    return nextStepNum;
}

void Stretcher::setSkipLastStepIfMatchesFirst(bool skip) {
    skipLastStepIfMatchesFirst = skip;
}

void Stretcher::getStretchedVoices(const StepSettings &stepSettings, int origNumVoices, int numHeldNotes, CurrentStep &outStep) {
    int numVoices = std::min(numHeldNotes, MAX_ACTUAL_VOICES);
    double origVoiceSizeY =
            numHeldNotes == 1 ? 1 : static_cast<double>(numVoices - 1) / static_cast<double>(origNumVoices - 1);

    outStep.numVoices = numVoices;
    for (size_t voiceNum = 0; voiceNum < static_cast<size_t>(numVoices); voiceNum++) {
        outStep.voices[voiceNum] = false;
    }

    int newVoiceNum = 0;
    for (size_t origVoiceNum = 0; origVoiceNum < static_cast<size_t>(origNumVoices); origVoiceNum++) {
        if (stepSettings.voices[origVoiceNum]) {
            double curVoiceY = origVoiceSizeY * static_cast<double>(origVoiceNum);
            newVoiceNum = std::min(static_cast<int>(std::round(curVoiceY)), numVoices - 1);
            outStep.voices[static_cast<size_t>(newVoiceNum)] = true;
        }
    }
}

void Stretcher::getNextStretchedStep(int numHeldNotes, CurrentStep &outStep) {
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

    recalcStretchInfo(numHeldNotes, state.stepsParameter->getIndex() + 1,
                        state.voicesParameter->getIndex() + 1);

    if (nextStepNum >= numSteps) {
        nextStepNum = 0;
    }

    if (!reuseNextStep || nextStepNum == 0) {
        reuseNextStep = true;

        int prevOrigStepNum;
        if (numSteps > origNumSteps) {
            prevOrigStepNum = static_cast<int>((static_cast<double>(nextStepNum + 1) + roundingOffset) /
                                                        origStepSizeX) - 1; // ugly
        } else {
            prevOrigStepNum = static_cast<int>((static_cast<double>(nextStepNum) + roundingOffset) /
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

    int nextOrigStepNum = static_cast<int>((static_cast<double>(nextStepNum + 1) + roundingOffset) /
                                                    origStepSizeX);

    if (nextOrigStepNum > next.stepNum) { // passed the next original step, so recalc things
        prev = next;
        next.stepNum = nextOrigStepNum;
        next.x = static_cast<double>(next.stepNum) * origStepSizeX;

        tieActive = state.stepState[static_cast<size_t>(prev.stepNum)].tieParameter->get();
        if (next.stepNum == origNumSteps) {
            // when on last original step, act as if there's one more step with the same settings
        } else if (tieActive) {
            // when tied, keep next the same as prev
        } else {
            updateOrigStepFromState(next, state, next.stepNum);
        }
    }

    DBG("stretched step " << nextStepNum << " (orig: " << prev.stepNum << "-" << next.stepNum << ", X: "
                            << prev.x << "-" << next.x << ")");
    updateStretchedStep(nextStepNum, outStep);

    nextStepNum++;
    if (nextStepNum >= numSteps) {
        nextStepNum = 0;
    }
}

Stretcher::StretchedResult Stretcher::stretch(int numHeldNotes) {
    return stretch(numHeldNotes, 0);
}

Stretcher::StretchedResult Stretcher::stretch(int numHeldNotes, int generateSteps) {
    recalcStretchInfo(numHeldNotes, state.stepsParameter->getIndex() + 1,
                        state.voicesParameter->getIndex() + 1);

    if (generateSteps == 0) generateSteps = numSteps;

    Stretcher::StretchedResult result;
    for (size_t stepNum = 0; stepNum < static_cast<size_t>(generateSteps); stepNum++) {
        CurrentStep currentStep;
        getNextStretchedStep(numNotes, currentStep);
        result.steps.push_back(currentStep);
    }

    result.numSteps = numSteps;
    result.numVoices = numVoices;
    return result;
}

void Stretcher::recalcStretchInfo(int _numNotes, int _origNumSteps, int _origNumVoices) {
    if (numNotes == _numNotes && origNumSteps == _origNumSteps && origNumVoices == _origNumVoices
        && (!skipLastStepIfMatchesFirst || nextStepNum < numSteps - 1))
        return;
    numNotes = _numNotes;
    origNumSteps = _origNumSteps;
    origNumVoices = _origNumVoices;

    numVoices = std::min(numNotes, MAX_ACTUAL_VOICES);
    // we'll round up; extra steps will make it more accurate
    numSteps = static_cast<int>(1 + std::ceil(
            static_cast<double>((origNumSteps - 1) * (numNotes - 1))
            / static_cast<double>(origNumVoices - 1)));
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

bool Stretcher::firstLastOrigStepsSame() {
    for (size_t voiceNum = 0; voiceNum < static_cast<size_t>(origNumVoices); voiceNum++) {
        if (state.stepState[0].voiceParameters[voiceNum]->get() !=
            state.stepState[static_cast<size_t>(origNumSteps) - 1].voiceParameters[voiceNum]->get())
            return false;
    }
    return true;
}

void Stretcher::updateStretchedStep(int stepNum, CurrentStep &outStep) {
    outStep.numVoices = numVoices;
    for (size_t voiceNum = 0; voiceNum < static_cast<size_t>(numVoices); voiceNum++) {
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
    for (size_t lineNum = 0; lineNum < static_cast<size_t>(prev.numActiveVoices); lineNum++) {
        double prevVoiceY = prev.activeVoicesY[lineNum];
        double nextVoiceY =
                lineNum < static_cast<size_t>(next.numActiveVoices) ? next.activeVoicesY[lineNum] : prevVoiceY;
        double slope = (nextVoiceY - prevVoiceY) / (next.x - prev.x);
        double curVoiceY = prevVoiceY + slope * (curStepX - prev.x);
        DBG("  voice " << curVoiceY << " (orig: " << prevVoiceY << "-" << nextVoiceY << ")");
        int voiceNum = std::min(static_cast<int>(std::round(curVoiceY)), numVoices - 1);
        outStep.voices[static_cast<size_t>(voiceNum)] = true;
    }

    double lengthSlope = (next.length - prev.length) / (next.x - prev.x);
    outStep.length = prev.length + lengthSlope * (curStepX - prev.x);

    double volSlope = (next.volume - prev.volume) / (next.x - prev.x);
    outStep.volume = prev.volume + volSlope * (curStepX - prev.x);

    outStep.octave = prev.octave;
    outStep.power = true;
    outStep.tie = false;
}

void Stretcher::updateOrigStepFromState(Stretcher::OrigStep &outStep, State &_state, int stepNum) const {
    outStep.numActiveVoices = 0;
    StepState step = _state.stepState[static_cast<size_t>(stepNum)];
    if (step.powerParameter->get()) {
        for (size_t origVoiceNum = 0; origVoiceNum < static_cast<size_t>(origNumVoices); origVoiceNum++) {
            if (step.voiceParameters[origVoiceNum]->get()) {
                outStep.activeVoicesY[static_cast<size_t>(outStep.numActiveVoices++)] =
                        origVoiceSizeY * static_cast<double>(origVoiceNum);
            }
        }
    }
    outStep.length = step.lengthParameter->get();
    outStep.volume = step.volParameter->get();
    outStep.octave = step.octaveParameter->getIndex();
}
