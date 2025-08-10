#include "CorvusTestSuit.hpp"

#include <catch2/catch_session.hpp>

int TestMain(const int ArgumentCount, char* Arguments[])
{
    bool bShouldUseXML = true;
    for (int Index = 0; Index < ArgumentCount; ++Index)
    {
        if (strcmp(Arguments[Index], "--no-xml") == 0)
        {
            bShouldUseXML = false;
        }
    }

    Catch::Session Session;
    Catch::ConfigData Config = Session.configData();
    Config.verbosity = Catch::Verbosity::High;
    Config.reporterSpecifications.push_back(Catch::ReporterSpec("Console", {}, Catch::ColourMode::PlatformDefault, {}));
    if (bShouldUseXML)
    {
        Config.reporterSpecifications.push_back(Catch::ReporterSpec("XML", Catch::Optional<std::string>("TestResults.xml"), Catch::ColourMode::PlatformDefault, {}));
    }
    Session.useConfigData(Config);

    const int Result = Session.run();
    return Result;
}
