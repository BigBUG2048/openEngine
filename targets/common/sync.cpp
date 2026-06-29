#include "targets/common/sync.h"

#include "device_context.h"
#include "targets/common/error.h"
#include "validation.h"

namespace oe {

Result<FenceHandle> createFence(DeviceHandle device) {
    auto* ctx = detail::DeviceRegistry::instance().get(device);
    if (!ctx) {
        return Result<FenceHandle>::failure(getLastError());
    }

    const FenceHandle handle = ctx->fences.allocate();
    auto result = ctx->backend->createFence(handle.index);
    if (!result) {
        ctx->fences.free(handle);
        return Result<FenceHandle>::failure(result.error);
    }

    return Result<FenceHandle>::success(handle);
}

Result<SemaphoreHandle> createSemaphore(DeviceHandle device) {
    auto* ctx = detail::DeviceRegistry::instance().get(device);
    if (!ctx) {
        return Result<SemaphoreHandle>::failure(getLastError());
    }

    const SemaphoreHandle handle = ctx->semaphores.allocate();
    auto result = ctx->backend->createSemaphore(handle.index);
    if (!result) {
        ctx->semaphores.free(handle);
        return Result<SemaphoreHandle>::failure(result.error);
    }

    return Result<SemaphoreHandle>::success(handle);
}

void destroyFence(DeviceHandle device, FenceHandle fence) {
    auto* ctx = detail::DeviceRegistry::instance().get(device);
    if (!ctx || !ctx->fences.isValid(fence)) {
        return;
    }
    ctx->backend->destroyFence(fence.index);
    ctx->fences.free(fence);
}

void destroySemaphore(DeviceHandle device, SemaphoreHandle semaphore) {
    auto* ctx = detail::DeviceRegistry::instance().get(device);
    if (!ctx || !ctx->semaphores.isValid(semaphore)) {
        return;
    }
    ctx->backend->destroySemaphore(semaphore.index);
    ctx->semaphores.free(semaphore);
}

Result<void> submit(QueueHandle queue, const SubmitDesc& desc) {
    auto* ctx = detail::DeviceRegistry::instance().findByQueue(queue);
    if (!ctx) {
        return Result<void>::failure(getLastError());
    }
    if (!detail::validateSubmitDesc(desc)) {
        return Result<void>::failure(getLastError());
    }

    for (const auto& cmd : desc.commandBuffers) {
        if (!ctx->commandBuffers.isValid(cmd)) {
            setLastError(Error::InvalidHandle, "Submit contains invalid command buffer");
            return Result<void>::failure(Error::InvalidHandle);
        }
    }

    return ctx->backend->submit(queue.index, desc);
}

Result<void> waitFence(FenceHandle fence, uint64_t timeoutNs) {
    auto* ctx = detail::DeviceRegistry::instance().findByFence(fence);
    if (!ctx) {
        return Result<void>::failure(getLastError());
    }
    return ctx->backend->waitFence(fence.index, timeoutNs);
}

Result<void> resetFence(FenceHandle fence) {
    auto* ctx = detail::DeviceRegistry::instance().findByFence(fence);
    if (!ctx) {
        return Result<void>::failure(getLastError());
    }
    return ctx->backend->resetFence(fence.index);
}

} // namespace oe
