#include "osrm/errorcodes.hpp"

#include <array>
#include <boost/assert.hpp>
#include <string>

namespace osrm
{

constexpr const std::array<const char *, 9> codes = {{
    "No Error",                                       // NoError
    "Fingerpring did not match the expected value",   // InvalidFingerprint
    "File is incompatible with this version of OSRM", // IncompatibleFileVersion
    "File is missing",                                // MissingFile
    "Problem opening file",                           // FileOpenError
    "Problem reading from file",                      // FileReadError
    "Problem writing to file",                        // FileWriteError
    "I/O error occurred",                             // FileIOError
    "Unexpected end of file"                          // UnexpectedEndOfFile
}};

const char *ErrorDescriptions[] = {
    "No Error",                                       // NoError
    "Fingerpring did not match the expected value",   // InvalidFingerprint
    "File is incompatible with this version of OSRM", // IncompatibleFileVersion
    "File is missing",                                // MissingFile
    "Problem opening file",                           // FileOpenError
    "Problem reading from file",                      // FileReadError
    "Problem writing to file",                        // FileWriteError
    "I/O error occurred",                             // FileIOError
    "Unexpected end of file",                         // UnexpectedEndOfFile
};

// Check to see that there is a description for each error
static_assert(sizeof(ErrorDescriptions) / sizeof(*(ErrorDescriptions)) == ErrorCode::__ENDMARKER__,
              "ErrorCode list and ErrorDescription lists are different sizes");

static_assert(codes.size() == ErrorCode::__ENDMARKER__,
              "ErrorCode list and ErrorDescription lists are different sizes");

const std::string GetErrorDescription(const ErrorCode code)
{
    BOOST_ASSERT(code < ErrorCode::__ENDMARKER__);
    return codes[code];
}
}