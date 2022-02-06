#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

juce::String timeToTimecodeString(double seconds);

juce::String quarterNotePositionToBarsBeatsString(double quarterNotes, int numerator, int denominator);

juce::String updateTimecodeDisplay(juce::AudioPlayHead::CurrentPositionInfo pos);
