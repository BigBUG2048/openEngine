#include "targets/common/pipeline.h"

#include "debug_layer.h"
#include "device_context.h"
#include "targets/common/error.h"
#include "validation.h"

namespace oe {

Result<PipelineHandle> createGraphicsPipeline(DeviceHandle device, const GraphicsPipelineDesc& desc) {
    auto* ctx = detail::DeviceRegistry::instance().get(device);
    if (!ctx) {
        return Result<PipelineHandle>::failure(getLastError());
    }
    if (!detail::validateGraphicsPipelineDesc(desc)) {
        return Result<PipelineHandle>::failure(getLastError());
    }

    const PipelineHandle handle = ctx->pipelines.allocate();
    auto result = ctx->backend->createGraphicsPipeline(handle.index, desc);
    if (!result) {
        ctx->pipelines.free(handle);
        return Result<PipelineHandle>::failure(result.error);
    }

    if (ctx->desc.enableDebugNaming && desc.debugName) {
        detail::debugNameResource("Pipeline", handle.index, desc.debugName);
    }

    return Result<PipelineHandle>::success(handle);
}

Result<PipelineHandle> createComputePipeline(DeviceHandle device, const ComputePipelineDesc& desc) {
    auto* ctx = detail::DeviceRegistry::instance().get(device);
    if (!ctx) {
        return Result<PipelineHandle>::failure(getLastError());
    }
    if (!detail::validateComputePipelineDesc(desc)) {
        return Result<PipelineHandle>::failure(getLastError());
    }

    const PipelineHandle handle = ctx->pipelines.allocate();
    auto result = ctx->backend->createComputePipeline(handle.index, desc);
    if (!result) {
        ctx->pipelines.free(handle);
        return Result<PipelineHandle>::failure(result.error);
    }

    if (ctx->desc.enableDebugNaming && desc.debugName) {
        detail::debugNameResource("Pipeline", handle.index, desc.debugName);
    }

    return Result<PipelineHandle>::success(handle);
}

void destroyPipeline(DeviceHandle device, PipelineHandle pipeline) {
    auto* ctx = detail::DeviceRegistry::instance().get(device);
    if (!ctx || !ctx->pipelines.isValid(pipeline)) {
        return;
    }
    ctx->backend->destroyPipeline(pipeline.index);
    ctx->pipelines.free(pipeline);
}

} // namespace oe
