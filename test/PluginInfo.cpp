#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "PluginProcessor.h"

TEST_CASE("Plugin instance name") {
    PluginProcessor testPlugin;
    CHECK_THAT(testPlugin.getName().toStdString(), Catch::Matchers::Equals("Ostinato"));
}
