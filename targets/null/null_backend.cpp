#include "null_backend.h"

#include "targets/common/error.h"

#include <cstring>
#include <sstream>

namespace oe {
namespace detail {

Result<void> NullBackend::initialize(const DeviceDesc& desc) {
    (void)desc;
    return Result<void>::success();
}

void NullBackend::shutdown() {
    buffers_.clear();
    textures_.clear();
    pipelines_.clear();
    commandBuffers_.clear();
    fences_.clear();
    semaphores_.clear();
    swapchains_.clear();
    submitCount_ = 0;
}

Result<void> NullBackend::createBuffer(uint32_t slot, const BufferDesc& desc) {
    BufferState state;
    state.desc = desc;
    if (desc.memoryHint != MemoryHint::GpuOnly) {
        state.staging.resize(static_cast<size_t>(desc.size));
    }
    buffers_[slot] = std::move(state);
    return Result<void>::success();
}

void NullBackend::destroyBuffer(uint32_t slot) { buffers_.erase(slot); }

Result<void> NullBackend::updateBuffer(uint32_t slot, const void* data, size_t size, size_t offset) {
    auto it = buffers_.find(slot);
    if (it == buffers_.end()) {
        setLastError(Error::InvalidHandle, "NullBackend buffer not found");
        return Result<void>::failure(Error::InvalidHandle);
    }
    if (offset + size > it->second.staging.size() && it->second.staging.empty()) {
        setLastError(Error::InvalidArgument, "Cannot update GPU-only buffer without staging");
        return Result<void>::failure(Error::InvalidArgument);
    }
    if (it->second.staging.empty()) {
        it->second.staging.resize(static_cast<size_t>(it->second.desc.size));
    }
    if (offset + size > it->second.staging.size()) {
        setLastError(Error::InvalidArgument, "updateBuffer out of range");
        return Result<void>::failure(Error::InvalidArgument);
    }
    std::memcpy(it->second.staging.data() + offset, data, size);
    return Result<void>::success();
}

Result<void> NullBackend::createTexture(uint32_t slot, const TextureDesc& desc) {
    textures_[slot] = TextureState{desc};
    return Result<void>::success();
}

void NullBackend::destroyTexture(uint32_t slot) { textures_.erase(slot); }

Result<void> NullBackend::createGraphicsPipeline(uint32_t slot, const GraphicsPipelineDesc& desc) {
    (void)desc;
    pipelines_[slot] = PipelineState{false};
    return Result<void>::success();
}

Result<void> NullBackend::createComputePipeline(uint32_t slot, const ComputePipelineDesc& desc) {
    (void)desc;
    pipelines_[slot] = PipelineState{true};
    return Result<void>::success();
}

void NullBackend::destroyPipeline(uint32_t slot) { pipelines_.erase(slot); }

Result<void> NullBackend::beginCommandBuffer(uint32_t slot, QueueType queue) {
    (void)queue;
    commandBuffers_[slot] = CmdState{};
    commandBuffers_[slot].open = true;
    return Result<void>::success();
}

Result<void> NullBackend::endCommandBuffer(uint32_t slot) {
    auto it = commandBuffers_.find(slot);
    if (it == commandBuffers_.end() || !it->second.open) {
        setLastError(Error::InvalidState, "Command buffer not open");
        return Result<void>::failure(Error::InvalidState);
    }
    it->second.open = false;
    return Result<void>::success();
}

void NullBackend::cmdBeginRenderPass(uint32_t slot, const RenderPassDesc& desc) {
    auto& cmd = commandBuffers_[slot];
    cmd.inRenderPass = true;
    std::ostringstream oss;
    oss << "BeginRenderPass(color=" << desc.colorAttachments.size()
        << ",depth=" << desc.hasDepthAttachment << ")";
    cmd.commands.push_back(oss.str());
}

void NullBackend::cmdEndRenderPass(uint32_t slot) {
    commandBuffers_[slot].inRenderPass = false;
    commandBuffers_[slot].commands.push_back("EndRenderPass");
}

void NullBackend::cmdBeginComputePass(uint32_t slot) {
    commandBuffers_[slot].inComputePass = true;
    commandBuffers_[slot].commands.push_back("BeginComputePass");
}

void NullBackend::cmdEndComputePass(uint32_t slot) {
    commandBuffers_[slot].inComputePass = false;
    commandBuffers_[slot].commands.push_back("EndComputePass");
}

void NullBackend::cmdBindPipeline(uint32_t slot, uint32_t pipelineSlot) {
    std::ostringstream oss;
    oss << "BindPipeline(" << pipelineSlot << ")";
    commandBuffers_[slot].commands.push_back(oss.str());
}

void NullBackend::cmdBindVertexBuffers(uint32_t slot, uint32_t firstBinding,
                                       const uint32_t* bufferSlots, const uint64_t* offsets,
                                       uint32_t count) {
    std::ostringstream oss;
    oss << "BindVertexBuffers(first=" << firstBinding << ",count=" << count << ")";
    (void)bufferSlots;
    (void)offsets;
    commandBuffers_[slot].commands.push_back(oss.str());
}

void NullBackend::cmdBindIndexBuffer(uint32_t slot, uint32_t bufferSlot, IndexType indexType,
                                     uint64_t offset) {
    std::ostringstream oss;
    oss << "BindIndexBuffer(" << bufferSlot << ",type=" << static_cast<int>(indexType)
        << ",offset=" << offset << ")";
    commandBuffers_[slot].commands.push_back(oss.str());
}

void NullBackend::cmdDraw(uint32_t slot, const DrawDesc& desc) {
    std::ostringstream oss;
    oss << "Draw(count=" << desc.vertexCount << ",instances=" << desc.instanceCount << ")";
    commandBuffers_[slot].commands.push_back(oss.str());
}

void NullBackend::cmdDrawIndexed(uint32_t slot, const DrawIndexedDesc& desc) {
    std::ostringstream oss;
    oss << "DrawIndexed(count=" << desc.indexCount << ",instances=" << desc.instanceCount << ")";
    commandBuffers_[slot].commands.push_back(oss.str());
}

void NullBackend::cmdDispatch(uint32_t slot, const DispatchDesc& desc) {
    std::ostringstream oss;
    oss << "Dispatch(" << desc.groupCountX << "," << desc.groupCountY << "," << desc.groupCountZ << ")";
    commandBuffers_[slot].commands.push_back(oss.str());
}

void NullBackend::cmdCopyBuffer(uint32_t slot, uint32_t srcSlot, uint32_t dstSlot, uint64_t size,
                                uint64_t srcOffset, uint64_t dstOffset) {
    std::ostringstream oss;
    oss << "CopyBuffer(src=" << srcSlot << ",dst=" << dstSlot << ",size=" << size
        << ",srcOff=" << srcOffset << ",dstOff=" << dstOffset << ")";
    commandBuffers_[slot].commands.push_back(oss.str());
}

void NullBackend::cmdBarrier(uint32_t slot, const BarrierDesc& desc) {
    (void)desc;
    commandBuffers_[slot].commands.push_back("Barrier");
}

Result<void> NullBackend::createFence(uint32_t slot) {
    fences_[slot] = FenceState{};
    return Result<void>::success();
}

Result<void> NullBackend::createSemaphore(uint32_t slot) {
    semaphores_[slot] = false;
    return Result<void>::success();
}

void NullBackend::destroyFence(uint32_t slot) { fences_.erase(slot); }
void NullBackend::destroySemaphore(uint32_t slot) { semaphores_.erase(slot); }

Result<void> NullBackend::submit(uint32_t queueSlot, const SubmitDesc& desc) {
    (void)queueSlot;
    ++submitCount_;
    if (desc.signalFence.isValid()) {
        auto it = fences_.find(desc.signalFence.index);
        if (it != fences_.end()) {
            it->second.signaled = true;
        }
    }
    (void)desc.commandBuffers;
    return Result<void>::success();
}

Result<void> NullBackend::waitFence(uint32_t slot, uint64_t timeoutNs) {
    (void)timeoutNs;
    auto it = fences_.find(slot);
    if (it == fences_.end()) {
        setLastError(Error::InvalidHandle, "Fence not found");
        return Result<void>::failure(Error::InvalidHandle);
    }
    it->second.signaled = false;
    return Result<void>::success();
}

Result<void> NullBackend::resetFence(uint32_t slot) {
    auto it = fences_.find(slot);
    if (it == fences_.end()) {
        setLastError(Error::InvalidHandle, "Fence not found");
        return Result<void>::failure(Error::InvalidHandle);
    }
    it->second.signaled = false;
    return Result<void>::success();
}

Result<void> NullBackend::createSwapchain(uint32_t slot, const SwapchainDesc& desc) {
    SwapchainState state;
    state.desc = desc;
    state.images.resize(2);
    for (auto& image : state.images) {
        image.desc.width = desc.width;
        image.desc.height = desc.height;
        image.desc.format = desc.preferredFormat;
        image.desc.usage = TextureUsage::RenderTarget;
    }
    swapchains_[slot] = std::move(state);
    return Result<void>::success();
}

void NullBackend::destroySwapchain(uint32_t slot) { swapchains_.erase(slot); }

Result<uint32_t> NullBackend::acquireNextImage(uint32_t slot, uint32_t signalSemaphoreSlot) {
    (void)signalSemaphoreSlot;
    auto it = swapchains_.find(slot);
    if (it == swapchains_.end()) {
        setLastError(Error::InvalidHandle, "Swapchain not found");
        return Result<uint32_t>::failure(Error::InvalidHandle);
    }
    const uint32_t imageIndex = it->second.frameIndex % static_cast<uint32_t>(it->second.images.size());
    it->second.frameIndex++;
    return Result<uint32_t>::success(imageIndex);
}

Result<void> NullBackend::present(uint32_t slot, uint32_t queueSlot, uint32_t waitSemaphoreSlot) {
    (void)slot;
    (void)queueSlot;
    (void)waitSemaphoreSlot;
    return Result<void>::success();
}

size_t NullBackend::recordedCommandCount(uint32_t cmdSlot) const {
    auto it = commandBuffers_.find(cmdSlot);
    if (it == commandBuffers_.end()) {
        return 0;
    }
    return it->second.commands.size();
}

} // namespace detail
} // namespace oe
