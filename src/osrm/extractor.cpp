#include "osrm/extractor.hpp"
#include "extractor/extractor.hpp"
#include "extractor/extractor_config.hpp"
#include "extractor/scripting_environment_lua.hpp"
#include "osrm/errorcodes.hpp"

namespace osrm
{

// Pimpl-like facade

ErrorCode extract(const extractor::ExtractorConfig &config)
{
    try
    {
        extractor::Sol2ScriptingEnvironment scripting_environment(
            config.profile_path.string().c_str());
        extractor::Extractor(config).run(scripting_environment);
    }
    catch (const util::internal_exception &e)
    {
        util::Log(logERROR) << e.what();
        return e.code();
    }
    return NoError;
}

} // ns osrm
