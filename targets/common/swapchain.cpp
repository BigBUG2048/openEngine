#include "targets/common/swapchain.h"

#include "device_context.h"
#include "targets/common/error.h"
#include "validation.h"

namespace oe {

Result<SwapchainHandle> createSwapchain(DeviceHandle device, const SwapchainDesc& desc) {
    auto* ctx = detail::DeviceRegistry::instance().get(device);
    if (!ctx) {
        return Result<SwapchainHandle>::failure(getLastError());
    }
    if (!detail::validateSwapchainDesc(desc)) {
        return Result<SwapchainHandle>::failure(getLastError());
    }

    const SwapchainHandle handle = ctx->swapchains.allocate();
    auto result = ctx->backend->createSwapchain(handle.index, desc);
    if (!result) {
        ctx->swapchains.free(handle);
        return Result<SwapchainHandle>::failure(result.error);
    }

    return Result<SwapchainHandle>::success(handle);
}

void destroySwapchain(SwapchainHandle swapchain) {
    auto* ctx = detail::DeviceRegistry::instance().findBySwapchain(swapchain);
    if (!ctx) {
        return;
    }
    ctx->backend->destroySwapchain(swapchain.index);
    ctx->swapchains.free(swapchain);
}

Result<TextureHandle> acquireNextImage(SwapchainHandle swapchain, SemaphoreHandle signal) {
    auto* ctx = detail::DeviceRegistry::instance().findBySwapchain(swapchain);
    if (!ctx) {
        return Result<TextureHandle>::failure(getLastError());
    }

    auto acquireResult = ctx->backend->acquireNextImage(swapchain.index, signal.index);
    if (!acquireResult) {
        return Result<TextureHandle>::failure(acquireResult.error);
    }

    const TextureHandle texture{acquireResult.value, 1};
    return Result<TextureHandle>::success(texture);
}

Result<void> present(SwapchainHandle swapchain, QueueHandle queue, SemaphoreHandle wait) {
    auto* ctx = detail::DeviceRegistry::instance().findBySwapchain(swapchain);
    if (!ctx) {
        return Result<void>::failure(getLastError());
    }

    return ctx->backend->present(swapchain.index, queue.index, wait.index);
}

} // namespace oe
