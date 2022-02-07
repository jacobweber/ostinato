#pragma once

#include "State.h"
#include "Constants.h"
#include "Step.h"

class Stretcher {
public:
    explicit Stretcher(State &_state);

    struct StretchedResult {
        std::vector<CurrentStep> steps;
        int numSteps = 0;
        int numVoices = 0;
    };

    struct OrigStep {
        stepnum_t stepNum;
        double x;
        std::array<double, constants::MAX_VOICES> activeVoicesY;
        int numActiveVoices;
        double length;
        double volume;
        int octave;
    };

    void setStepIndex(int origStepIndex);

    int getNumVoices() const;

    int getNumSteps() const;

    stepnum_t getOrigStepIndex() const;

    int getNextStepIndex() const;

    void setSkipLastStepIfMatchesFirst(bool skip);

    static void getStretchedVoices(State &state, int stepNum, int numHeldNotes, CurrentStep &outStep);

    void getNextStretchedStep(int numHeldNotes, CurrentStep &outStep);

    Stretcher::StretchedResult stretch(int numHeldNotes);

    Stretcher::StretchedResult stretch(int numHeldNotes, int generateSteps);

private:
    void recalcStretchInfo(int _numNotes, int _origNumSteps, int _origNumVoices);

    bool firstLastOrigStepsSame();

    void updateStretchedStep(stepnum_t stepNum, CurrentStep &outStep);

    void updateOrigStepFromState(OrigStep &outStep, State &_state, stepnum_t stepNum) const;

private:
    State &state;

    bool skipLastStepIfMatchesFirst = true;
    int numVoices = 0;
    int numSteps = 0;
    int numNotes = 0;
    OrigStep prev{};
    OrigStep next{};

    int origNumSteps = 0;
    int origNumVoices = 0;
    double origStepSizeX = 0.0;
    double origVoiceSizeY = 0.0;
    double roundingOffset = 0.0;

    bool tieActive = false;
    int nextStepIndex = 0;
    bool reuseNextStep = false;
};
