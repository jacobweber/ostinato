#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

class SpinLockedPosInfo {
public:
    SpinLockedPosInfo() {
        info.resetToDefault();
    }

    // Wait-free, but setting new info may fail if the main thread is currently
    // calling `get`. This is unlikely to matter in practice because
    // we'll be calling `set` much more frequently than `get`.
    void set(const juce::AudioPlayHead::CurrentPositionInfo &newInfo) {
        const juce::SpinLock::ScopedTryLockType lock(mutex);

        if (lock.isLocked())
            info = newInfo;
    }

    juce::AudioPlayHead::CurrentPositionInfo get() const noexcept {
        const juce::SpinLock::ScopedLockType lock(mutex);
        return info;
    }

private:
    juce::SpinLock mutex;
    juce::AudioPlayHead::CurrentPositionInfo info{};
};
