#include "device_context.h"

#include "targets/common/error.h"
#include "validation.h"

namespace oe {
namespace detail {

DeviceRegistry& DeviceRegistry::instance() {
    static DeviceRegistry registry;
    return registry;
}

Result<DeviceHandle> DeviceRegistry::create(const DeviceDesc& desc) {
    if (!validateDeviceDesc(desc)) {
        return Result<DeviceHandle>::failure(getLastError());
    }

    auto ctx = std::make_unique<DeviceContext>();
    ctx->desc = desc;
    ctx->backend = createBackend(desc.backend);
    if (!ctx->backend) {
        setLastError(Error::Unsupported, "Backend not available");
        return Result<DeviceHandle>::failure(Error::Unsupported);
    }

    auto initResult = ctx->backend->initialize(desc);
    if (!initResult) {
        return Result<DeviceHandle>::failure(initResult.error);
    }

    // Default queues: one per type
    for (uint32_t i = 0; i < 3; ++i) {
        ctx->queues.allocate();
    }

    const DeviceHandle handle = devices_.allocate();
    ctx->handle = handle;

    if (handle.index >= contexts_.size()) {
        contexts_.resize(handle.index + 1);
    }
    contexts_[handle.index] = std::move(ctx);

    return Result<DeviceHandle>::success(handle);
}

void DeviceRegistry::destroy(DeviceHandle handle) {
    if (!devices_.isValid(handle)) {
        return;
    }

    if (handle.index < contexts_.size() && contexts_[handle.index]) {
        contexts_[handle.index]->backend->shutdown();
        contexts_[handle.index].reset();
    }

    devices_.free(handle);
}

DeviceContext* DeviceRegistry::get(DeviceHandle handle) {
    if (!devices_.isValid(handle)) {
        setLastError(Error::InvalidHandle, "Invalid device handle");
        return nullptr;
    }
    if (handle.index >= contexts_.size() || !contexts_[handle.index]) {
        setLastError(Error::InvalidHandle, "Device context not found");
        return nullptr;
    }
    return contexts_[handle.index].get();
}

const DeviceContext* DeviceRegistry::get(DeviceHandle handle) const {
    return const_cast<DeviceRegistry*>(this)->get(handle);
}

DeviceContext* DeviceRegistry::findByCommandBuffer(CommandBufferHandle cmd) {
    if (!cmd.isValid()) {
        setLastError(Error::InvalidHandle, "Invalid command buffer handle");
        return nullptr;
    }

    for (auto& ctxPtr : contexts_) {
        if (!ctxPtr) {
            continue;
        }
        if (ctxPtr->commandBuffers.isValid(cmd)) {
            return ctxPtr.get();
        }
    }

    setLastError(Error::InvalidHandle, "Command buffer not found");
    return nullptr;
}

DeviceContext* DeviceRegistry::findByQueue(QueueHandle queue) {
    if (!queue.isValid()) {
        setLastError(Error::InvalidHandle, "Invalid queue handle");
        return nullptr;
    }

    for (auto& ctxPtr : contexts_) {
        if (!ctxPtr) {
            continue;
        }
        if (ctxPtr->queues.isValid(queue)) {
            return ctxPtr.get();
        }
    }

    setLastError(Error::InvalidHandle, "Queue not found");
    return nullptr;
}

DeviceContext* DeviceRegistry::findByFence(FenceHandle fence) {
    if (!fence.isValid()) {
        setLastError(Error::InvalidHandle, "Invalid fence handle");
        return nullptr;
    }
    for (auto& ctxPtr : contexts_) {
        if (ctxPtr && ctxPtr->fences.isValid(fence)) {
            return ctxPtr.get();
        }
    }
    setLastError(Error::InvalidHandle, "Fence not found");
    return nullptr;
}

DeviceContext* DeviceRegistry::findBySemaphore(SemaphoreHandle semaphore) {
    if (!semaphore.isValid()) {
        setLastError(Error::InvalidHandle, "Invalid semaphore handle");
        return nullptr;
    }
    for (auto& ctxPtr : contexts_) {
        if (ctxPtr && ctxPtr->semaphores.isValid(semaphore)) {
            return ctxPtr.get();
        }
    }
    setLastError(Error::InvalidHandle, "Semaphore not found");
    return nullptr;
}

DeviceContext* DeviceRegistry::findBySwapchain(SwapchainHandle swapchain) {
    if (!swapchain.isValid()) {
        setLastError(Error::InvalidHandle, "Invalid swapchain handle");
        return nullptr;
    }
    for (auto& ctxPtr : contexts_) {
        if (ctxPtr && ctxPtr->swapchains.isValid(swapchain)) {
            return ctxPtr.get();
        }
    }
    setLastError(Error::InvalidHandle, "Swapchain not found");
    return nullptr;
}

} // namespace detail
} // namespace oe
