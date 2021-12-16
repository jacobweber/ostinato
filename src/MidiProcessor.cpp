#include "MidiProcessor.h"

const size_t NUM_STEPS = 4;
const size_t NUM_VOICES = 4;
const size_t STEP_VALUE = 4; // 1 note per quarter
const double PLAY_DELAY_SEC = 0.1; // so cycle starts at first note when you press multiple notes at once

void MidiProcessor::init(double sr) {
    sampleRate = sr;

    pressedNotes.clear();
    playingNotes.clear();

    cycleOn = false;
    transportOn = false;
    nextStepIndex = 0;
}

void MidiProcessor::stopPlaying(juce::MidiBuffer &midi, int offset) {
    for (auto const &i: playingNotes) {
        midi.addEvent(juce::MidiMessage::noteOff(1, i), offset);
    }
    playingNotes.clear();
}

void
MidiProcessor::process(int numSamples, juce::MidiBuffer &midi, const juce::AudioPlayHead::CurrentPositionInfo &posInfo,
                       float speed) {
    for (const auto metadata: midi) {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn()) {
            pressedNotes.add(msg.getNoteNumber());
        } else if (msg.isNoteOff()) {
            pressedNotes.removeValue(msg.getNoteNumber());
        }
    }

    midi.clear();
    if (posInfo.isRecording || posInfo.isPlaying) {
        if (!transportOn) { // started transport
            stopPlaying(midi, 0);
            transportOn = true;
        }
    } else if (transportOn) { // stopped transport
        stopPlaying(midi, 0);
        transportOn = false;
    }

    if (!cycleOn) {
        if (pressedNotes.size() > 0) { // notes pressed, so start cycle
            cycleOn = true;
            nextStepIndex = 0;
            // TODO: transport
            samplesUntilNextStep = static_cast<int>(PLAY_DELAY_SEC * sampleRate);
            return;
        } else {
            return;
        }
    } else {
        if (pressedNotes.size() == 0) { // no notes pressed, so stop cycle
            stopPlaying(midi, 0);
            cycleOn = false;
            return;
        }
    }

    jassert(pressedNotes.size() > 0);
    jassert(nextStepIndex >= 0);

    if (samplesUntilNextStep < numSamples) {
        // play a step within this frame
        size_t voiceNum = nextStepIndex % NUM_VOICES; // for now
        size_t noteIndex = static_cast<size_t>(juce::jmin((int) voiceNum, pressedNotes.size() -
                                                                          1)); // repeat top note if we don't have enough
        int noteValue = pressedNotes[noteIndex];
        stopPlaying(midi, samplesUntilNextStep);
        midi.addEvent(juce::MidiMessage::noteOn(1, noteValue, (juce::uint8) 90), samplesUntilNextStep);
        playingNotes.add(noteValue);

        // prepare next step
        nextStepIndex = (nextStepIndex + 1) % NUM_STEPS;
        samplesUntilNextStep = static_cast<int>(std::ceil(
                sampleRate /
                ((posInfo.bpm / 60) * (STEP_VALUE / static_cast<unsigned long>(posInfo.timeSigDenominator)))));
    } else {
        samplesUntilNextStep -= numSamples;
    }
}
