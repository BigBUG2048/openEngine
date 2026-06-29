#pragma once

#include "targets/common/error.h"
#include "targets/common/types.h"

namespace oe {

Result<CommandBufferHandle> beginCommandBuffer(DeviceHandle device, QueueType queue);
Result<void> endCommandBuffer(CommandBufferHandle cmd);

void cmdBeginRenderPass(CommandBufferHandle cmd, const RenderPassDesc& desc);
void cmdEndRenderPass(CommandBufferHandle cmd);
void cmdBeginComputePass(CommandBufferHandle cmd);
void cmdEndComputePass(CommandBufferHandle cmd);

void cmdBindPipeline(CommandBufferHandle cmd, PipelineHandle pipeline);
void cmdBindVertexBuffers(CommandBufferHandle cmd, uint32_t firstBinding,
                          const BufferHandle* buffers, const uint64_t* offsets, uint32_t count);
void cmdBindIndexBuffer(CommandBufferHandle cmd, BufferHandle buffer, IndexType indexType,
                        uint64_t offset = 0);
void cmdDraw(CommandBufferHandle cmd, const DrawDesc& desc);
void cmdDrawIndexed(CommandBufferHandle cmd, const DrawIndexedDesc& desc);
void cmdDispatch(CommandBufferHandle cmd, const DispatchDesc& desc);
void cmdCopyBuffer(CommandBufferHandle cmd, BufferHandle src, BufferHandle dst, uint64_t size,
                   uint64_t srcOffset = 0, uint64_t dstOffset = 0);
void cmdBarrier(CommandBufferHandle cmd, const BarrierDesc& desc);

} // namespace oe
