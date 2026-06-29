#include "targets/common/buffer.h"

#include "debug_layer.h"
#include "device_context.h"
#include "targets/common/error.h"
#include "validation.h"

namespace oe {

Result<BufferHandle> createBuffer(DeviceHandle device, const BufferDesc& desc) {
    auto* ctx = detail::DeviceRegistry::instance().get(device);
    if (!ctx) {
        return Result<BufferHandle>::failure(getLastError());
    }
    if (!detail::validateBufferDesc(desc)) {
        return Result<BufferHandle>::failure(getLastError());
    }

    const BufferHandle handle = ctx->buffers.allocate();
    auto result = ctx->backend->createBuffer(handle.index, desc);
    if (!result) {
        ctx->buffers.free(handle);
        return Result<BufferHandle>::failure(result.error);
    }

    if (ctx->desc.enableDebugNaming && desc.debugName) {
        detail::debugNameResource("Buffer", handle.index, desc.debugName);
    }

    return Result<BufferHandle>::success(handle);
}

void destroyBuffer(DeviceHandle device, BufferHandle buffer) {
    auto* ctx = detail::DeviceRegistry::instance().get(device);
    if (!ctx || !ctx->buffers.isValid(buffer)) {
        return;
    }
    ctx->backend->destroyBuffer(buffer.index);
    ctx->buffers.free(buffer);
}

Result<void> updateBuffer(DeviceHandle device, BufferHandle buffer, const void* data, size_t size,
                          size_t offset) {
    auto* ctx = detail::DeviceRegistry::instance().get(device);
    if (!ctx) {
        return Result<void>::failure(getLastError());
    }
    if (!ctx->buffers.isValid(buffer)) {
        setLastError(Error::InvalidHandle, "Invalid buffer handle");
        return Result<void>::failure(Error::InvalidHandle);
    }
    if (data == nullptr || size == 0) {
        setLastError(Error::InvalidArgument, "updateBuffer requires data and size > 0");
        return Result<void>::failure(Error::InvalidArgument);
    }

    return ctx->backend->updateBuffer(buffer.index, data, size, offset);
}

} // namespace oe
