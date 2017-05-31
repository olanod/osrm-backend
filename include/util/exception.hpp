/*

Copyright (c) 2016, Project OSRM contributors
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list
of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef OSRM_EXCEPTION_HPP
#define OSRM_EXCEPTION_HPP

#include <exception>
#include <iostream>
#include <string>
#include <utility>

#include "osrm/errorcodes.hpp"
#include <boost/format.hpp>

namespace osrm
{
namespace util
{

class exception : public std::exception
{
  public:
    explicit exception(const char *message) : message(message) {}
    explicit exception(std::string message) : message(std::move(message)) {}
    explicit exception(boost::format message) : message(message.str()) {}
    const char *what() const noexcept override { return message.c_str(); }

  private:
    // This function exists to 'anchor' the class, and stop the compiler from
    // copying vtable and RTTI info into every object file that includes
    // this header. (Caught by -Wweak-vtables under Clang.)
    virtual void anchor() const;
    const std::string message;
};

/**
 * Indicates a class of error that occurred that was caused by some kind of
 * external input (common examples are out of disk space, file permission errors,
 * user supplied bad data, etc).
 */
class runtime_error : public std::runtime_error
{
    using Base = std::runtime_error;

  protected:
    explicit runtime_error(const char *message,
                           const std::string &sourceref,
                           const std::string &code_description)
        : Base(message), sourceref(sourceref)
    {
        messagebuf = code_description + ": " + Base::what() + " (at " + sourceref + ")";
    }
    explicit runtime_error(std::string message,
                           const std::string &sourceref,
                           const std::string &code_description)
        : runtime_error(message.c_str(), sourceref, code_description)
    {
    }

  public:
    const char *what() const noexcept override { return messagebuf.c_str(); }
    const char *where() const noexcept { return sourceref.c_str(); }
    virtual ErrorCode GetCode() const = 0;
    virtual std::string GetCodeDescription() const = 0;

  private:
    // This function exists to 'anchor' the class, and stop the compiler from
    // copying vtable and RTTI info into every object file that includes
    // this header. (Caught by -Wweak-vtables under Clang.)
    virtual void anchor() const;
    const std::string sourceref; // Source location where the exception was raised
    std::string messagebuf;
};

// TODO: the classes below  are dying for templatization, but I don't know of a good pattern to
// instantiate a template with string literals that isn't hella ugly.

/**
 * Thrown when a file is read that doesn't have a valid fingerprint and one
 * was expected
 */
class InvalidFingerprintException : public runtime_error
{
    using Base = runtime_error;
    using Base::Base;

  public:
    InvalidFingerprintException(const std::string &message, const std::string &sourceref)
        : Base(message, sourceref, GetCodeDescription())
    {
    }
    ErrorCode GetCode() const override { return ErrorCode::IncompatibleFileVersion; }
    std::string GetCodeDescription() const override
    {
        return "Fingerprint did not match the expected value";
    }
};

/**
 * Thrown when we try to read a file that contains a valid fingerprint, but the file
 * was generated with a version of OSRM that is incompatible
 */
class IncompatileFileVersionException : public runtime_error
{
    using Base = runtime_error;
    using Base::Base;

  public:
    IncompatileFileVersionException(const std::string &message, const std::string &sourceref)
        : Base(message, sourceref, GetCodeDescription())
    {
    }
    ErrorCode GetCode() const override { return ErrorCode::IncompatibleFileVersion; }
    std::string GetCodeDescription() const override
    {
        return "File is incompatible with this version of OSRM";
    }
};

/**
 * Thrown when a file we expected to find is not there
 */
class MissingFileException : public runtime_error
{
    using Base = runtime_error;
    using Base::Base;

  public:
    MissingFileException(const std::string &message, const std::string &sourceref)
        : Base(message, sourceref, GetCodeDescription())
    {
    }
    ErrorCode GetCode() const override { return ErrorCode::MissingFile; }
    std::string GetCodeDescription() const override { return "File is missing"; }
};

/**
 * Thrown when there is any kind of problem reading from a file.  This includes
 * failure to open a file (i.e. because of file permissions)
 */
class FileReadException : public runtime_error
{
    using Base = runtime_error;
    using Base::Base;

  public:
    FileReadException(const std::string &message, const std::string &sourceref)
        : Base(message, sourceref, GetCodeDescription())
    {
    }
    ErrorCode GetCode() const override { return ErrorCode::FileReadError; }
    std::string GetCodeDescription() const override { return "Problem reading from file"; }
};

/**
 * Thrown when there is any kind of problem writing to a file.  This includes failure
 * to create a file (i.e. because of space or permissions).
 */
class FileWriteException : public runtime_error
{
    using Base = runtime_error;
    using Base::Base;

  public:
    FileWriteException(const std::string &message, const std::string &sourceref)
        : Base(message, sourceref, GetCodeDescription())
    {
    }
    ErrorCode GetCode() const override { return ErrorCode::FileWriteError; }
    std::string GetCodeDescription() const override { return "Problem writing to file"; }
};

/**
 * Occurs when there is some other kind of I/O problem - error accessing the filesystem,
 * reading metadata, etc
 */
class IOException : public runtime_error
{
    using Base = runtime_error;
    using Base::Base;

  public:
    IOException(const std::string &message, const std::string &sourceref)
        : Base(message, sourceref, GetCodeDescription())
    {
    }
    ErrorCode GetCode() const override { return ErrorCode::FileIOError; }
    std::string GetCodeDescription() const override { return "I/O problem"; }
};

/**
 * Thrown when we unexpectedly reach the end of file.  Usually indicates
 * that a file was truncated outside OSRM somehow, or the header has
 * become corrupt and the object count is incorrect.
 */
class UnexpectedEndOfFileException : public runtime_error
{
    using Base = runtime_error;
    using Base::Base;

  public:
    UnexpectedEndOfFileException(const std::string &message, const std::string &sourceref)
        : Base(message, sourceref, GetCodeDescription())
    {
    }
    ErrorCode GetCode() const override { return ErrorCode::UnexpectedEndOfFile; }
    std::string GetCodeDescription() const override { return "Unexpected end of file"; }
};

/**
 * Thrown when someone tries to load a dataset in CoreCH mode but the dataset
 * is not a CoreCH dataset.
 */
class IncompatibleDatasetException : public runtime_error
{
    using Base = runtime_error;
    using Base::Base;

  public:
    IncompatibleDatasetException(const std::string &message, const std::string &sourceref)
        : Base(message, sourceref, GetCodeDescription())
    {
    }
    ErrorCode GetCode() const override { return ErrorCode::IncompatibleDataset; }
    std::string GetCodeDescription() const override
    {
        return "The dataset you are trying to load is not compatible with the routing algorithm "
               "you want to use.";
    }
};

/**
 * Thrown when the algorithm name supplied by the user doesn't match a known type
 */
class UnknownAlgorithmException : public runtime_error
{
    using Base = runtime_error;
    using Base::Base;

  public:
    UnknownAlgorithmException(const std::string &message, const std::string &sourceref)
        : Base(message, sourceref, GetCodeDescription())
    {
    }
    ErrorCode GetCode() const override { return ErrorCode::UnknownAlgorithm; }
    std::string GetCodeDescription() const override { return "Unrecognized algorithm"; }
};
}
}

#endif /* OSRM_EXCEPTION_HPP */
