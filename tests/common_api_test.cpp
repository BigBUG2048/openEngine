#include "targets/common/api.h"

#include <cassert>
#include <cstring>
#include <iostream>

static const char kDummyShader[] = "void main() {}";

static bool testDeviceLifecycle() {
    oe::DeviceDesc desc{};
    desc.backend = oe::BackendType::Null;
    desc.applicationName = "openEngine-test";

    auto deviceResult = oe::createDevice(desc);
    if (!deviceResult) {
        std::cerr << "createDevice failed: " << oe::getLastErrorMessage() << '\n';
        return false;
    }

    oe::destroyDevice(deviceResult.value);
    return true;
}

static bool testBufferAndDrawPath() {
    oe::DeviceDesc desc{};
    desc.backend = oe::BackendType::Null;
    desc.applicationName = "openEngine-test";

    auto deviceResult = oe::createDevice(desc);
    if (!deviceResult) {
        return false;
    }
    const oe::DeviceHandle device = deviceResult.value;

    oe::BufferDesc vbDesc{};
    vbDesc.size = 1024;
    vbDesc.usage = oe::BufferUsage::Vertex;
    vbDesc.memoryHint = oe::MemoryHint::CpuToGpu;
    auto vbResult = oe::createBuffer(device, vbDesc);
    if (!vbResult) {
        std::cerr << "createBuffer failed\n";
        return false;
    }

    float vertices[] = {0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f};
    if (!oe::updateBuffer(device, vbResult.value, vertices, sizeof(vertices))) {
        std::cerr << "updateBuffer failed\n";
        return false;
    }

    oe::TextureDesc texDesc{};
    texDesc.width = 800;
    texDesc.height = 600;
    texDesc.format = oe::TextureFormat::R8G8B8A8_UNORM;
    texDesc.usage = oe::TextureUsage::RenderTarget;
    auto texResult = oe::createTexture(device, texDesc);
    if (!texResult) {
        std::cerr << "createTexture failed\n";
        return false;
    }

    oe::GraphicsPipelineDesc pipeDesc{};
    pipeDesc.vertexShader.source = kDummyShader;
    pipeDesc.fragmentShader.source = kDummyShader;
    auto pipeResult = oe::createGraphicsPipeline(device, pipeDesc);
    if (!pipeResult) {
        std::cerr << "createGraphicsPipeline failed\n";
        return false;
    }

    auto queueResult = oe::getQueue(device, oe::QueueType::Graphics);
    if (!queueResult) {
        return false;
    }

    auto cmdResult = oe::beginCommandBuffer(device, oe::QueueType::Graphics);
    if (!cmdResult) {
        return false;
    }
    const oe::CommandBufferHandle cmd = cmdResult.value;

    oe::RenderPassDesc passDesc{};
    oe::ColorAttachmentDesc color{};
    color.texture = texResult.value;
    color.loadOp = oe::LoadOp::Clear;
    passDesc.colorAttachments.push_back(color);

    oe::cmdBeginRenderPass(cmd, passDesc);
    oe::cmdBindPipeline(cmd, pipeResult.value);

    const oe::BufferHandle buffers[] = {vbResult.value};
    const uint64_t offsets[] = {0};
    oe::cmdBindVertexBuffers(cmd, 0, buffers, offsets, 1);

    oe::DrawDesc drawDesc{};
    drawDesc.vertexCount = 3;
    oe::cmdDraw(cmd, drawDesc);
    oe::cmdEndRenderPass(cmd);

    if (!oe::endCommandBuffer(cmd)) {
        std::cerr << "endCommandBuffer failed\n";
        return false;
    }

    auto fenceResult = oe::createFence(device);
    if (!fenceResult) {
        return false;
    }

    oe::SubmitDesc submitDesc{};
    submitDesc.commandBuffers.push_back(cmd);
    submitDesc.signalFence = fenceResult.value;
    if (!oe::submit(queueResult.value, submitDesc)) {
        std::cerr << "submit failed\n";
        return false;
    }

    oe::destroyPipeline(device, pipeResult.value);
    oe::destroyTexture(device, texResult.value);
    oe::destroyBuffer(device, vbResult.value);
    oe::destroyFence(device, fenceResult.value);
    oe::destroyDevice(device);
    return true;
}

static bool testValidationRejectsInvalidBuffer() {
    oe::DeviceDesc desc{};
    desc.backend = oe::BackendType::Null;
    desc.applicationName = "openEngine-test";

    auto deviceResult = oe::createDevice(desc);
    if (!deviceResult) {
        return false;
    }

    oe::BufferDesc badDesc{};
    badDesc.size = 0;
    badDesc.usage = oe::BufferUsage::Vertex;
    auto result = oe::createBuffer(deviceResult.value, badDesc);
    if (result.ok()) {
        std::cerr << "Expected invalid buffer to fail validation\n";
        return false;
    }

    oe::destroyDevice(deviceResult.value);
    return true;
}

int main() {
    const bool ok = testDeviceLifecycle() && testBufferAndDrawPath() && testValidationRejectsInvalidBuffer();
    if (!ok) {
        std::cerr << "common_api_tests FAILED\n";
        return 1;
    }
    std::cout << "common_api_tests PASSED\n";
    return 0;
}
