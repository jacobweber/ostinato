#pragma once

struct MidiValue {
    int note, channel;

    bool operator==(const MidiValue &r) const { return note == r.note && channel == r.channel; }

    bool operator>(const MidiValue &r) const { return note > r.note || channel > r.channel; }

    bool operator<(const MidiValue &r) const { return !operator>=(r); }

    bool operator>=(const MidiValue &r) const { return note >= r.note && channel >= r.channel; }
};
