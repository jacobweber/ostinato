#define CATCH_CONFIG_RUNNER

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include <juce_audio_utils/juce_audio_utils.h>

int main(int argc, char *argv[]) {
    auto gui = juce::ScopedJuceInitialiser_GUI{};
    return Catch::Session().run(argc, argv);
}
