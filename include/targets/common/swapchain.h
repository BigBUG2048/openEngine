#pragma once

#include "targets/common/error.h"
#include "targets/common/types.h"

namespace oe {

Result<SwapchainHandle> createSwapchain(DeviceHandle device, const SwapchainDesc& desc);
void destroySwapchain(SwapchainHandle swapchain);
Result<TextureHandle> acquireNextImage(SwapchainHandle swapchain, SemaphoreHandle signal);
Result<void> present(SwapchainHandle swapchain, QueueHandle queue, SemaphoreHandle wait);

} // namespace oe
