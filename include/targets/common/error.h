#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace oe {

enum class Error : uint32_t {
    None = 0,
    InvalidHandle,
    InvalidArgument,
    OutOfMemory,
    NotFound,
    AlreadyExists,
    InvalidState,
    BackendError,
    Unsupported,
};

const char* errorToString(Error error);

void setLastError(Error error, const char* message = nullptr);
Error getLastError();
const char* getLastErrorMessage();

template<typename T>
struct Result {
    T value{};
    Error error = Error::None;

    [[nodiscard]] bool ok() const { return error == Error::None; }
    [[nodiscard]] explicit operator bool() const { return ok(); }

    static Result success(T val) {
        Result r;
        r.value = val;
        return r;
    }

    static Result failure(Error err) {
        Result r;
        r.error = err;
        return r;
    }
};

template<>
struct Result<void> {
    Error error = Error::None;

    [[nodiscard]] bool ok() const { return error == Error::None; }
    [[nodiscard]] explicit operator bool() const { return ok(); }

    static Result success() { return {}; }

    static Result failure(Error err) {
        Result r;
        r.error = err;
        return r;
    }
};

} // namespace oe
