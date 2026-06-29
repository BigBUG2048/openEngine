#pragma once

#include "targets/common/error.h"
#include "targets/common/types.h"

namespace oe {

Result<FenceHandle> createFence(DeviceHandle device);
Result<SemaphoreHandle> createSemaphore(DeviceHandle device);
void destroyFence(DeviceHandle device, FenceHandle fence);
void destroySemaphore(DeviceHandle device, SemaphoreHandle semaphore);

Result<void> submit(QueueHandle queue, const SubmitDesc& desc);
Result<void> waitFence(FenceHandle fence, uint64_t timeoutNs = UINT64_MAX);
Result<void> resetFence(FenceHandle fence);

} // namespace oe
