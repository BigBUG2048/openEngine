#include "targets/common/texture.h"

#include "debug_layer.h"
#include "device_context.h"
#include "targets/common/error.h"
#include "validation.h"

namespace oe {

Result<TextureHandle> createTexture(DeviceHandle device, const TextureDesc& desc) {
    auto* ctx = detail::DeviceRegistry::instance().get(device);
    if (!ctx) {
        return Result<TextureHandle>::failure(getLastError());
    }
    if (!detail::validateTextureDesc(desc)) {
        return Result<TextureHandle>::failure(getLastError());
    }

    const TextureHandle handle = ctx->textures.allocate();
    auto result = ctx->backend->createTexture(handle.index, desc);
    if (!result) {
        ctx->textures.free(handle);
        return Result<TextureHandle>::failure(result.error);
    }

    if (ctx->desc.enableDebugNaming && desc.debugName) {
        detail::debugNameResource("Texture", handle.index, desc.debugName);
    }

    return Result<TextureHandle>::success(handle);
}

void destroyTexture(DeviceHandle device, TextureHandle texture) {
    auto* ctx = detail::DeviceRegistry::instance().get(device);
    if (!ctx || !ctx->textures.isValid(texture)) {
        return;
    }
    ctx->backend->destroyTexture(texture.index);
    ctx->textures.free(texture);
}

} // namespace oe
