#pragma once

#include "backend_interface.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace oe {
namespace detail {

class NullBackend final : public IBackend {
public:
    Result<void> initialize(const DeviceDesc& desc) override;
    void shutdown() override;

    Result<void> createBuffer(uint32_t slot, const BufferDesc& desc) override;
    void destroyBuffer(uint32_t slot) override;
    Result<void> updateBuffer(uint32_t slot, const void* data, size_t size, size_t offset) override;

    Result<void> createTexture(uint32_t slot, const TextureDesc& desc) override;
    void destroyTexture(uint32_t slot) override;

    Result<void> createGraphicsPipeline(uint32_t slot, const GraphicsPipelineDesc& desc) override;
    Result<void> createComputePipeline(uint32_t slot, const ComputePipelineDesc& desc) override;
    void destroyPipeline(uint32_t slot) override;

    Result<void> beginCommandBuffer(uint32_t slot, QueueType queue) override;
    Result<void> endCommandBuffer(uint32_t slot) override;

    void cmdBeginRenderPass(uint32_t slot, const RenderPassDesc& desc) override;
    void cmdEndRenderPass(uint32_t slot) override;
    void cmdBeginComputePass(uint32_t slot) override;
    void cmdEndComputePass(uint32_t slot) override;

    void cmdBindPipeline(uint32_t slot, uint32_t pipelineSlot) override;
    void cmdBindVertexBuffers(uint32_t slot, uint32_t firstBinding, const uint32_t* bufferSlots,
                              const uint64_t* offsets, uint32_t count) override;
    void cmdBindIndexBuffer(uint32_t slot, uint32_t bufferSlot, IndexType indexType,
                            uint64_t offset) override;
    void cmdDraw(uint32_t slot, const DrawDesc& desc) override;
    void cmdDrawIndexed(uint32_t slot, const DrawIndexedDesc& desc) override;
    void cmdDispatch(uint32_t slot, const DispatchDesc& desc) override;
    void cmdCopyBuffer(uint32_t slot, uint32_t srcSlot, uint32_t dstSlot, uint64_t size,
                       uint64_t srcOffset, uint64_t dstOffset) override;
    void cmdBarrier(uint32_t slot, const BarrierDesc& desc) override;

    Result<void> createFence(uint32_t slot) override;
    Result<void> createSemaphore(uint32_t slot) override;
    void destroyFence(uint32_t slot) override;
    void destroySemaphore(uint32_t slot) override;
    Result<void> submit(uint32_t queueSlot, const SubmitDesc& desc) override;
    Result<void> waitFence(uint32_t slot, uint64_t timeoutNs) override;
    Result<void> resetFence(uint32_t slot) override;

    Result<void> createSwapchain(uint32_t slot, const SwapchainDesc& desc) override;
    void destroySwapchain(uint32_t slot) override;
    Result<uint32_t> acquireNextImage(uint32_t slot, uint32_t signalSemaphoreSlot) override;
    Result<void> present(uint32_t slot, uint32_t queueSlot, uint32_t waitSemaphoreSlot) override;

    // Test introspection
    [[nodiscard]] size_t recordedCommandCount(uint32_t cmdSlot) const;
    [[nodiscard]] size_t submitCount() const { return submitCount_; }

private:
    struct BufferState {
        BufferDesc desc{};
        std::vector<uint8_t> staging;
    };

    struct TextureState {
        TextureDesc desc{};
    };

    struct PipelineState {
        bool isCompute = false;
    };

    struct CmdState {
        bool open = false;
        bool inRenderPass = false;
        bool inComputePass = false;
        std::vector<std::string> commands;
    };

    struct FenceState {
        bool signaled = false;
    };

    struct SwapchainState {
        SwapchainDesc desc{};
        uint32_t frameIndex = 0;
        std::vector<TextureState> images;
    };

    std::unordered_map<uint32_t, BufferState> buffers_;
    std::unordered_map<uint32_t, TextureState> textures_;
    std::unordered_map<uint32_t, PipelineState> pipelines_;
    std::unordered_map<uint32_t, CmdState> commandBuffers_;
    std::unordered_map<uint32_t, FenceState> fences_;
    std::unordered_map<uint32_t, bool> semaphores_;
    std::unordered_map<uint32_t, SwapchainState> swapchains_;

    size_t submitCount_ = 0;
};

} // namespace detail
} // namespace oe
