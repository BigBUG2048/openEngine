#include "targets/common/error.h"

#include <string>

namespace oe {

namespace {

thread_local Error g_lastError = Error::None;
thread_local std::string g_lastErrorMessage;

} // namespace

const char* errorToString(Error error) {
    switch (error) {
    case Error::None: return "None";
    case Error::InvalidHandle: return "InvalidHandle";
    case Error::InvalidArgument: return "InvalidArgument";
    case Error::OutOfMemory: return "OutOfMemory";
    case Error::NotFound: return "NotFound";
    case Error::AlreadyExists: return "AlreadyExists";
    case Error::InvalidState: return "InvalidState";
    case Error::BackendError: return "BackendError";
    case Error::Unsupported: return "Unsupported";
    default: return "Unknown";
    }
}

void setLastError(Error error, const char* message) {
    g_lastError = error;
    g_lastErrorMessage = message ? message : errorToString(error);
}

Error getLastError() { return g_lastError; }

const char* getLastErrorMessage() { return g_lastErrorMessage.c_str(); }

} // namespace oe
