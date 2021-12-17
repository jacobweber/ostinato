#include <catch2/catch.hpp>

#include "PluginProcessor.h"

TEST_CASE("Plugin instance name") {
    PluginProcessor testPlugin;
    CHECK_THAT(testPlugin.getName().toStdString(), Catch::Equals("Ostinato"));
}
