#pragma once

#include "targets/common/error.h"
#include "targets/common/types.h"

namespace oe {

Result<TextureHandle> createTexture(DeviceHandle device, const TextureDesc& desc);
void destroyTexture(DeviceHandle device, TextureHandle texture);

} // namespace oe
