#include "MidiProcessor.h"

const size_t NUM_STEPS = 4;
const size_t NUM_VOICES = 4;
const size_t STEP_VALUE = 4; // 1 step per quarter, make float?
const double PLAY_DELAY_SEC = 0.1; // so cycle starts at first note when you press multiple notes at once

void MidiProcessor::init(double sr) {
    sampleRate = sr;

    pressedNotes.clear();
    playingNotes.clear();

    cycleOn = false;
    transportOn = false;
    prevPpqPos = 0;
    nextStepIndex = 0;
}

void MidiProcessor::stopPlaying(juce::MidiBuffer &midi, int offset) {
    for (auto const &i: playingNotes) {
        midi.addEvent(juce::MidiMessage::noteOff(1, i), offset);
    }
    playingNotes.clear();
}

void
MidiProcessor::process(int numSamples, juce::MidiBuffer &midi,
                       const juce::AudioPlayHead::CurrentPositionInfo &posInfo) {
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
            prevPpqPos = posInfo.ppqPosition;
            return; // skip a frame so we can calculate ppq per frame
        }
    } else if (transportOn) { // stopped transport
        stopPlaying(midi, 0);
        transportOn = false;
    }

    if (!cycleOn) {
        if (pressedNotes.size() > 0) { // notes pressed, so start cycle
            cycleOn = true;
            nextStepIndex = 0;
            if (transportOn) {
                nextStepPpqPos = std::ceil(posInfo.ppqPosition);
                // start at next beat; don't try to align to bars
                // don't calculate in samples, since tempo may change
            } else {
                samplesUntilNextStep = static_cast<int>(PLAY_DELAY_SEC * sampleRate);
            }
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

    // see if we're playing a step within this frame
    int sampleOffsetWithinFrame = -1;
    if (transportOn) {
        double ppqPerFrame = posInfo.ppqPosition - prevPpqPos; // changes depending on tempo/meter
        prevPpqPos = posInfo.ppqPosition;
        double frameEndPpqPos = posInfo.ppqPosition + ppqPerFrame;
        if (nextStepPpqPos < frameEndPpqPos) {
            double ppqOffset = nextStepPpqPos - posInfo.ppqPosition;
            sampleOffsetWithinFrame = static_cast<int>(std::floor(numSamples * (ppqOffset / ppqPerFrame)));
        }
    } else {
        if (samplesUntilNextStep < numSamples) {
            sampleOffsetWithinFrame = samplesUntilNextStep;
        } else {
            samplesUntilNextStep -= numSamples;
        }
    }

    if (sampleOffsetWithinFrame != -1) {
        // play a step within this frame
        size_t voiceNum = nextStepIndex % NUM_VOICES; // for now
        auto noteIndex = static_cast<size_t>(juce::jmin((int) voiceNum, pressedNotes.size() -
                                                                        1)); // repeat top note if we don't have enough
        int noteValue = pressedNotes[static_cast<int>(noteIndex)];
        stopPlaying(midi, samplesUntilNextStep);
        midi.addEvent(juce::MidiMessage::noteOn(1, noteValue, (juce::uint8) 90), sampleOffsetWithinFrame);
        playingNotes.add(noteValue);

        // prepare next step
        nextStepIndex = (nextStepIndex + 1) % NUM_STEPS;
        if (transportOn) {
            double ppqPosPerStep = 4.0f / STEP_VALUE;
            nextStepPpqPos += ppqPosPerStep;
        } else {
            double stepsPerBeat = static_cast<double>(STEP_VALUE) / posInfo.timeSigDenominator;
            samplesUntilNextStep = static_cast<int>(std::ceil(
                    sampleRate /
                    ((posInfo.bpm / 60) * stepsPerBeat)));
        }
    }
}
