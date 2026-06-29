#include "backend_interface.h"
#include "null_backend.h"

#include "targets/common/error.h"

#include <memory>

namespace oe {
namespace detail {

std::unique_ptr<IBackend> createBackend(BackendType type) {
    switch (type) {
    case BackendType::Null:
        return std::make_unique<NullBackend>();
    case BackendType::Vulkan:
    case BackendType::Metal:
    case BackendType::DX12:
        setLastError(Error::Unsupported, "Backend not implemented yet");
        return nullptr;
    default:
        setLastError(Error::InvalidArgument, "Unknown backend type");
        return nullptr;
    }
}

} // namespace detail
} // namespace oe
