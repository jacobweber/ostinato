#include "MidiProcessor.h"

void MidiProcessor::init(double sampleRate) {
    notes.clear();
    currentNote = 0;
    lastNoteValue = -1;
    time = 0;
    rate = static_cast<float>(sampleRate);
    DBG("init");
}

void MidiProcessor::process(int numSamples, juce::MidiBuffer &midi, float speed) {
    auto noteDuration = static_cast<int>(std::ceil(rate * 0.25f * (0.1f + (1.0f - (speed)))));

    for (const auto metadata: midi) {
        const auto msg = metadata.getMessage();

        if (msg.isNoteOn()) {
            DBG(msg.getDescription());
            notes.add(msg.getNoteNumber());
        } else if (msg.isNoteOff()) {
            notes.removeValue(msg.getNoteNumber());
        }
    }

    midi.clear();

    if ((time + numSamples) >= noteDuration) {
        auto offset = juce::jmax(0, juce::jmin((int) (noteDuration - time), numSamples - 1));

        if (lastNoteValue > 0) {
            midi.addEvent(juce::MidiMessage::noteOff(1, lastNoteValue), offset);
            lastNoteValue = -1;
        }

        if (notes.size() > 0) {
            currentNote = (currentNote + 1) % notes.size();
            lastNoteValue = notes[currentNote];
            midi.addEvent(juce::MidiMessage::noteOn(1, lastNoteValue, (juce::uint8) 50), offset);
        }
    }

    time = (time + numSamples) % noteDuration;
}
