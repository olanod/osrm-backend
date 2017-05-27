#ifndef ERRORCODES_HPP
#define ERRORCODES_HPP

#include <string>

namespace osrm
{

/**
 * Various error codes that can be returned by OSRM internal functions.
 * Note: often, these translate into return codes from `int main()` functions.
 *       Thus, do not change the order - if adding new codes, append them to the
 *       end (but leave _ENDMARKER_ in the last position), so the code values do
 *       not change for users that are checking for certain values.
 */
enum ErrorCode
{
    NoError = 0u,
    InvalidFingerprint,
    IncompatibleFileVersion,
    MissingFile,
    FileOpenError,
    FileReadError,
    FileWriteError,
    FileIOError,
    UnexpectedEndOfFile,
    __ENDMARKER__ // Leave this value at the end
};

const std::string GetErrorDescription(const ErrorCode code);
}

#endif // ERRORCODES_HPP