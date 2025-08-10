#include "CorvusTestSuit.hpp"

#include <catch2/catch_session.hpp>

int TestMain(int ArgumentCount, char* Arguments[])
{
    Catch::Session Session;
    Catch::ConfigData Config = Session.configData();
    Config.verbosity = Catch::Verbosity::High;
    Config.reporterSpecifications.push_back(Catch::ReporterSpec("Console", {}, Catch::ColourMode::PlatformDefault, {}));
    Config.reporterSpecifications.push_back(Catch::ReporterSpec("XML", Catch::Optional<std::string>("TestResults.xml"), Catch::ColourMode::PlatformDefault, {}));
    Session.useConfigData(Config);
    
    int Result = Session.run();
    return Result;
}
