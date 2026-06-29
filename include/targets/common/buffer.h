#pragma once

#include "targets/common/error.h"
#include "targets/common/types.h"

namespace oe {

Result<BufferHandle> createBuffer(DeviceHandle device, const BufferDesc& desc);
void destroyBuffer(DeviceHandle device, BufferHandle buffer);
Result<void> updateBuffer(DeviceHandle device, BufferHandle buffer, const void* data, size_t size,
                          size_t offset = 0);

} // namespace oe
