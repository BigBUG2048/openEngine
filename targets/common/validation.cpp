#include "validation.h"

#include "targets/common/error.h"

namespace oe {
namespace detail {

bool validateDeviceDesc(const DeviceDesc& desc) {
    if (desc.applicationName == nullptr || desc.applicationName[0] == '\0') {
        setLastError(Error::InvalidArgument, "DeviceDesc.applicationName is required");
        return false;
    }
    return true;
}

bool validateBufferDesc(const BufferDesc& desc) {
    if (desc.size == 0) {
        setLastError(Error::InvalidArgument, "BufferDesc.size must be > 0");
        return false;
    }
    if (desc.usage == BufferUsage::None) {
        setLastError(Error::InvalidArgument, "BufferDesc.usage must be set");
        return false;
    }
    return true;
}

bool validateTextureDesc(const TextureDesc& desc) {
    if (desc.width == 0 || desc.height == 0) {
        setLastError(Error::InvalidArgument, "TextureDesc dimensions must be > 0");
        return false;
    }
    if (desc.format == TextureFormat::Unknown) {
        setLastError(Error::InvalidArgument, "TextureDesc.format must be set");
        return false;
    }
    if (desc.usage == TextureUsage::None) {
        setLastError(Error::InvalidArgument, "TextureDesc.usage must be set");
        return false;
    }
    return true;
}

bool validateGraphicsPipelineDesc(const GraphicsPipelineDesc& desc) {
    if (desc.vertexShader.bytecode == nullptr && desc.vertexShader.source == nullptr) {
        setLastError(Error::InvalidArgument, "GraphicsPipelineDesc requires vertex shader");
        return false;
    }
    if (desc.fragmentShader.bytecode == nullptr && desc.fragmentShader.source == nullptr) {
        setLastError(Error::InvalidArgument, "GraphicsPipelineDesc requires fragment shader");
        return false;
    }
    return true;
}

bool validateComputePipelineDesc(const ComputePipelineDesc& desc) {
    if (desc.computeShader.bytecode == nullptr && desc.computeShader.source == nullptr) {
        setLastError(Error::InvalidArgument, "ComputePipelineDesc requires compute shader");
        return false;
    }
    return true;
}

bool validateRenderPassDesc(const RenderPassDesc& desc) {
    if (desc.colorAttachments.empty() && !desc.hasDepthAttachment) {
        setLastError(Error::InvalidArgument, "RenderPassDesc requires at least one attachment");
        return false;
    }
    return true;
}

bool validateSubmitDesc(const SubmitDesc& desc) {
    if (desc.commandBuffers.empty()) {
        setLastError(Error::InvalidArgument, "SubmitDesc requires at least one command buffer");
        return false;
    }
    return true;
}

bool validateSwapchainDesc(const SwapchainDesc& desc) {
    if (desc.width == 0 || desc.height == 0) {
        setLastError(Error::InvalidArgument, "SwapchainDesc dimensions must be > 0");
        return false;
    }
    return true;
}

} // namespace detail
} // namespace oe
