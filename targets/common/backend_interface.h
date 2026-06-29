#pragma once

#include "targets/common/error.h"
#include "targets/common/types.h"

namespace oe {
namespace detail {

class IBackend {
public:
    virtual ~IBackend() = default;

    virtual Result<void> initialize(const DeviceDesc& desc) = 0;
    virtual void shutdown() = 0;

    virtual Result<void> createBuffer(uint32_t slot, const BufferDesc& desc) = 0;
    virtual void destroyBuffer(uint32_t slot) = 0;
    virtual Result<void> updateBuffer(uint32_t slot, const void* data, size_t size, size_t offset) = 0;

    virtual Result<void> createTexture(uint32_t slot, const TextureDesc& desc) = 0;
    virtual void destroyTexture(uint32_t slot) = 0;

    virtual Result<void> createGraphicsPipeline(uint32_t slot, const GraphicsPipelineDesc& desc) = 0;
    virtual Result<void> createComputePipeline(uint32_t slot, const ComputePipelineDesc& desc) = 0;
    virtual void destroyPipeline(uint32_t slot) = 0;

    virtual Result<void> beginCommandBuffer(uint32_t slot, QueueType queue) = 0;
    virtual Result<void> endCommandBuffer(uint32_t slot) = 0;

    virtual void cmdBeginRenderPass(uint32_t slot, const RenderPassDesc& desc) = 0;
    virtual void cmdEndRenderPass(uint32_t slot) = 0;
    virtual void cmdBeginComputePass(uint32_t slot) = 0;
    virtual void cmdEndComputePass(uint32_t slot) = 0;

    virtual void cmdBindPipeline(uint32_t slot, uint32_t pipelineSlot) = 0;
    virtual void cmdBindVertexBuffers(uint32_t slot, uint32_t firstBinding, const uint32_t* bufferSlots,
                                      const uint64_t* offsets, uint32_t count) = 0;
    virtual void cmdBindIndexBuffer(uint32_t slot, uint32_t bufferSlot, IndexType indexType,
                                    uint64_t offset) = 0;
    virtual void cmdDraw(uint32_t slot, const DrawDesc& desc) = 0;
    virtual void cmdDrawIndexed(uint32_t slot, const DrawIndexedDesc& desc) = 0;
    virtual void cmdDispatch(uint32_t slot, const DispatchDesc& desc) = 0;
    virtual void cmdCopyBuffer(uint32_t slot, uint32_t srcSlot, uint32_t dstSlot, uint64_t size,
                               uint64_t srcOffset, uint64_t dstOffset) = 0;
    virtual void cmdBarrier(uint32_t slot, const BarrierDesc& desc) = 0;

    virtual Result<void> createFence(uint32_t slot) = 0;
    virtual Result<void> createSemaphore(uint32_t slot) = 0;
    virtual void destroyFence(uint32_t slot) = 0;
    virtual void destroySemaphore(uint32_t slot) = 0;
    virtual Result<void> submit(uint32_t queueSlot, const SubmitDesc& desc) = 0;
    virtual Result<void> waitFence(uint32_t slot, uint64_t timeoutNs) = 0;
    virtual Result<void> resetFence(uint32_t slot) = 0;

    virtual Result<void> createSwapchain(uint32_t slot, const SwapchainDesc& desc) = 0;
    virtual void destroySwapchain(uint32_t slot) = 0;
    virtual Result<uint32_t> acquireNextImage(uint32_t slot, uint32_t signalSemaphoreSlot) = 0;
    virtual Result<void> present(uint32_t slot, uint32_t queueSlot, uint32_t waitSemaphoreSlot) = 0;
};

std::unique_ptr<IBackend> createBackend(BackendType type);

} // namespace detail
} // namespace oe
