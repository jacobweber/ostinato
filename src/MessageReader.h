#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "State.h"
#include "Step.h"

// Only use for things that don't need to happen during bounce.
class MessageReader : public juce::Timer {
public:
    explicit MessageReader(State &_state);

    void timerCallback() override;

    void updateSteps(const RecordedSteps &steps);

    void updateSteps(const UpdatedStepSettings &stepSettings);

private:
    State &state;
    RecordedSteps recordedSteps{};
    UpdatedStepSettings updatedStepSettings{};
};
