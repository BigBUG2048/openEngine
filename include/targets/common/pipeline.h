#pragma once

#include "targets/common/error.h"
#include "targets/common/types.h"

namespace oe {

Result<PipelineHandle> createGraphicsPipeline(DeviceHandle device, const GraphicsPipelineDesc& desc);
Result<PipelineHandle> createComputePipeline(DeviceHandle device, const ComputePipelineDesc& desc);
void destroyPipeline(DeviceHandle device, PipelineHandle pipeline);

} // namespace oe
