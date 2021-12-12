#include <juce_audio_utils/juce_audio_utils.h>

juce::String timeToTimecodeString(double seconds)
{
    auto millisecs = juce::roundToInt(seconds * 1000.0);
    auto absMillisecs = std::abs(millisecs);

    return juce::String::formatted("%02d:%02d:%02d.%03d",
                                   millisecs / 3600000,
                                   (absMillisecs / 60000) % 60,
                                   (absMillisecs / 1000) % 60,
                                   absMillisecs % 1000);
}

juce::String quarterNotePositionToBarsBeatsString(double quarterNotes, int numerator, int denominator)
{
    if (numerator == 0 || denominator == 0)
        return "1|1|000";

    auto quarterNotesPerBar = (numerator * 4 / denominator);
    auto beats = (fmod(quarterNotes, quarterNotesPerBar) / quarterNotesPerBar) * numerator;

    auto bar = ((int)quarterNotes) / quarterNotesPerBar + 1;
    auto beat = ((int)beats) + 1;
    auto ticks = ((int)(fmod(beats, 1.0) * 960.0 + 0.5));

    return juce::String::formatted("%d|%d|%03d", bar, beat, ticks);
}

void updateTimecodeDisplay(juce::AudioPlayHead::CurrentPositionInfo pos)
{
    // ppqPosition = 0-based beat number since start; in 6/8 counts 3 beats per bar
    // ppqPositionOfLastBarStart = 0-based beat number of bar start; can be fractional e.g. in 7/8 bar 2 is 3.5

    juce::MemoryOutputStream displayText;

    displayText << juce::String(pos.bpm, 2) << " bpm, "
                << pos.timeSigNumerator << '/' << pos.timeSigDenominator
                << "  -  " << timeToTimecodeString(pos.timeInSeconds)
                << "  -  " << quarterNotePositionToBarsBeatsString(pos.ppqPosition, pos.timeSigNumerator, pos.timeSigDenominator);

    if (pos.isRecording)
        displayText << "  (recording)";
    else if (pos.isPlaying)
        displayText << "  (playing)";

    timecodeDisplayLabel.setText(displayText.toString(), juce::dontSendNotification);
}
