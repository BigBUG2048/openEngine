#pragma once

#include "targets/common/error.h"
#include "targets/common/types.h"

namespace oe {

struct DeviceHandle;
struct QueueHandle;
struct BufferHandle;
struct TextureHandle;
struct PipelineHandle;
struct CommandBufferHandle;
struct FenceHandle;
struct SemaphoreHandle;
struct SwapchainHandle;

Result<DeviceHandle> createDevice(const DeviceDesc& desc);
void destroyDevice(DeviceHandle device);

Result<QueueHandle> getQueue(DeviceHandle device, QueueType type, uint32_t index = 0);

} // namespace oe
