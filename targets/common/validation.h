#pragma once

#include "targets/common/types.h"

namespace oe {
namespace detail {

bool validateDeviceDesc(const DeviceDesc& desc);
bool validateBufferDesc(const BufferDesc& desc);
bool validateTextureDesc(const TextureDesc& desc);
bool validateGraphicsPipelineDesc(const GraphicsPipelineDesc& desc);
bool validateComputePipelineDesc(const ComputePipelineDesc& desc);
bool validateRenderPassDesc(const RenderPassDesc& desc);
bool validateSubmitDesc(const SubmitDesc& desc);
bool validateSwapchainDesc(const SwapchainDesc& desc);

} // namespace detail
} // namespace oe
