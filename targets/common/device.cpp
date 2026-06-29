#include "targets/common/device.h"

#include "device_context.h"
#include "targets/common/error.h"

namespace oe {

Result<DeviceHandle> createDevice(const DeviceDesc& desc) {
    return detail::DeviceRegistry::instance().create(desc);
}

void destroyDevice(DeviceHandle device) {
    detail::DeviceRegistry::instance().destroy(device);
}

Result<QueueHandle> getQueue(DeviceHandle device, QueueType type, uint32_t index) {
    auto* ctx = detail::DeviceRegistry::instance().get(device);
    if (!ctx) {
        return Result<QueueHandle>::failure(getLastError());
    }

    const uint32_t queueIndex = static_cast<uint32_t>(type);
    if (index != 0) {
        setLastError(Error::InvalidArgument, "Only queue index 0 is supported initially");
        return Result<QueueHandle>::failure(Error::InvalidArgument);
    }

    QueueHandle handle{queueIndex, 1};
    if (!ctx->queues.isValid(handle)) {
        setLastError(Error::NotFound, "Queue not found");
        return Result<QueueHandle>::failure(Error::NotFound);
    }

    return Result<QueueHandle>::success(handle);
}

} // namespace oe
