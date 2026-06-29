#include "targets/common/command_buffer.h"

#include "device_context.h"
#include "targets/common/error.h"
#include "validation.h"

namespace oe {

Result<CommandBufferHandle> beginCommandBuffer(DeviceHandle device, QueueType queue) {
    auto* ctx = detail::DeviceRegistry::instance().get(device);
    if (!ctx) {
        return Result<CommandBufferHandle>::failure(getLastError());
    }

    const CommandBufferHandle handle = ctx->commandBuffers.allocate();
    auto& state = ctx->cmdState(handle.index);
    state = detail::CommandBufferState{};
    state.recording = true;
    state.queue = queue;
    state.deviceIndex = device.index;

    auto result = ctx->backend->beginCommandBuffer(handle.index, queue);
    if (!result) {
        ctx->commandBuffers.free(handle);
        return Result<CommandBufferHandle>::failure(result.error);
    }

    return Result<CommandBufferHandle>::success(handle);
}

Result<void> endCommandBuffer(CommandBufferHandle cmd) {
    auto* ctx = detail::DeviceRegistry::instance().findByCommandBuffer(cmd);
    if (!ctx) {
        return Result<void>::failure(getLastError());
    }

    auto& state = ctx->cmdState(cmd.index);
    if (!state.recording) {
        setLastError(Error::InvalidState, "Command buffer is not recording");
        return Result<void>::failure(Error::InvalidState);
    }
    if (state.inRenderPass || state.inComputePass) {
        setLastError(Error::InvalidState, "Command buffer still inside a pass");
        return Result<void>::failure(Error::InvalidState);
    }

    auto result = ctx->backend->endCommandBuffer(cmd.index);
    if (!result) {
        return result;
    }

    state.recording = false;
    return Result<void>::success();
}

void cmdBeginRenderPass(CommandBufferHandle cmd, const RenderPassDesc& desc) {
    auto* ctx = detail::DeviceRegistry::instance().findByCommandBuffer(cmd);
    if (!ctx || !detail::validateRenderPassDesc(desc)) {
        return;
    }

    auto& state = ctx->cmdState(cmd.index);
    if (!state.recording || state.inRenderPass || state.inComputePass) {
        setLastError(Error::InvalidState, "Invalid state for cmdBeginRenderPass");
        return;
    }

    ctx->backend->cmdBeginRenderPass(cmd.index, desc);
    state.inRenderPass = true;
}

void cmdEndRenderPass(CommandBufferHandle cmd) {
    auto* ctx = detail::DeviceRegistry::instance().findByCommandBuffer(cmd);
    if (!ctx) {
        return;
    }

    auto& state = ctx->cmdState(cmd.index);
    if (!state.inRenderPass) {
        setLastError(Error::InvalidState, "Not inside render pass");
        return;
    }

    ctx->backend->cmdEndRenderPass(cmd.index);
    state.inRenderPass = false;
}

void cmdBeginComputePass(CommandBufferHandle cmd) {
    auto* ctx = detail::DeviceRegistry::instance().findByCommandBuffer(cmd);
    if (!ctx) {
        return;
    }

    auto& state = ctx->cmdState(cmd.index);
    if (!state.recording || state.inRenderPass || state.inComputePass) {
        setLastError(Error::InvalidState, "Invalid state for cmdBeginComputePass");
        return;
    }

    ctx->backend->cmdBeginComputePass(cmd.index);
    state.inComputePass = true;
}

void cmdEndComputePass(CommandBufferHandle cmd) {
    auto* ctx = detail::DeviceRegistry::instance().findByCommandBuffer(cmd);
    if (!ctx) {
        return;
    }

    auto& state = ctx->cmdState(cmd.index);
    if (!state.inComputePass) {
        setLastError(Error::InvalidState, "Not inside compute pass");
        return;
    }

    ctx->backend->cmdEndComputePass(cmd.index);
    state.inComputePass = false;
}

void cmdBindPipeline(CommandBufferHandle cmd, PipelineHandle pipeline) {
    auto* ctx = detail::DeviceRegistry::instance().findByCommandBuffer(cmd);
    if (!ctx || !ctx->pipelines.isValid(pipeline)) {
        if (ctx) {
            setLastError(Error::InvalidHandle, "Invalid pipeline handle");
        }
        return;
    }

    auto& state = ctx->cmdState(cmd.index);
    if (!state.recording) {
        setLastError(Error::InvalidState, "Command buffer is not recording");
        return;
    }

    ctx->backend->cmdBindPipeline(cmd.index, pipeline.index);
    state.boundPipeline = pipeline;
}

void cmdBindVertexBuffers(CommandBufferHandle cmd, uint32_t firstBinding, const BufferHandle* buffers,
                          const uint64_t* offsets, uint32_t count) {
    auto* ctx = detail::DeviceRegistry::instance().findByCommandBuffer(cmd);
    if (!ctx || buffers == nullptr || offsets == nullptr || count == 0) {
        setLastError(Error::InvalidArgument, "Invalid vertex buffer bind arguments");
        return;
    }

    std::vector<uint32_t> slots(count);
    for (uint32_t i = 0; i < count; ++i) {
        if (!ctx->buffers.isValid(buffers[i])) {
            setLastError(Error::InvalidHandle, "Invalid vertex buffer handle");
            return;
        }
        slots[i] = buffers[i].index;
    }

    ctx->backend->cmdBindVertexBuffers(cmd.index, firstBinding, slots.data(), offsets, count);
}

void cmdBindIndexBuffer(CommandBufferHandle cmd, BufferHandle buffer, IndexType indexType,
                        uint64_t offset) {
    auto* ctx = detail::DeviceRegistry::instance().findByCommandBuffer(cmd);
    if (!ctx || !ctx->buffers.isValid(buffer)) {
        setLastError(Error::InvalidHandle, "Invalid index buffer handle");
        return;
    }

    ctx->backend->cmdBindIndexBuffer(cmd.index, buffer.index, indexType, offset);
}

void cmdDraw(CommandBufferHandle cmd, const DrawDesc& desc) {
    auto* ctx = detail::DeviceRegistry::instance().findByCommandBuffer(cmd);
    if (!ctx) {
        return;
    }

    auto& state = ctx->cmdState(cmd.index);
    if (!state.recording || !state.inRenderPass) {
        setLastError(Error::InvalidState, "Draw requires active render pass");
        return;
    }

    ctx->backend->cmdDraw(cmd.index, desc);
}

void cmdDrawIndexed(CommandBufferHandle cmd, const DrawIndexedDesc& desc) {
    auto* ctx = detail::DeviceRegistry::instance().findByCommandBuffer(cmd);
    if (!ctx) {
        return;
    }

    auto& state = ctx->cmdState(cmd.index);
    if (!state.recording || !state.inRenderPass) {
        setLastError(Error::InvalidState, "DrawIndexed requires active render pass");
        return;
    }

    ctx->backend->cmdDrawIndexed(cmd.index, desc);
}

void cmdDispatch(CommandBufferHandle cmd, const DispatchDesc& desc) {
    auto* ctx = detail::DeviceRegistry::instance().findByCommandBuffer(cmd);
    if (!ctx) {
        return;
    }

    auto& state = ctx->cmdState(cmd.index);
    if (!state.recording || !state.inComputePass) {
        setLastError(Error::InvalidState, "Dispatch requires active compute pass");
        return;
    }

    ctx->backend->cmdDispatch(cmd.index, desc);
}

void cmdCopyBuffer(CommandBufferHandle cmd, BufferHandle src, BufferHandle dst, uint64_t size,
                   uint64_t srcOffset, uint64_t dstOffset) {
    auto* ctx = detail::DeviceRegistry::instance().findByCommandBuffer(cmd);
    if (!ctx || !ctx->buffers.isValid(src) || !ctx->buffers.isValid(dst)) {
        setLastError(Error::InvalidHandle, "Invalid buffer handle for copy");
        return;
    }

    ctx->backend->cmdCopyBuffer(cmd.index, src.index, dst.index, size, srcOffset, dstOffset);
}

void cmdBarrier(CommandBufferHandle cmd, const BarrierDesc& desc) {
    auto* ctx = detail::DeviceRegistry::instance().findByCommandBuffer(cmd);
    if (!ctx) {
        return;
    }

    ctx->backend->cmdBarrier(cmd.index, desc);
}

} // namespace oe
