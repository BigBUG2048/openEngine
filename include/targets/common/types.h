#pragma once

#include <cstdint>
#include <vector>

namespace oe {

// --- Handles (opaque, generation-tracked) ---

struct HandleBase {
    uint32_t index = kInvalidIndex;
    uint32_t generation = 0;

    static constexpr uint32_t kInvalidIndex = UINT32_MAX;

    [[nodiscard]] bool isValid() const { return index != kInvalidIndex; }
    [[nodiscard]] bool operator==(const HandleBase& other) const {
        return index == other.index && generation == other.generation;
    }
    [[nodiscard]] bool operator!=(const HandleBase& other) const { return !(*this == other); }
};

struct DeviceHandle : HandleBase {};
struct QueueHandle : HandleBase {};
struct BufferHandle : HandleBase {};
struct TextureHandle : HandleBase {};
struct PipelineHandle : HandleBase {};
struct CommandBufferHandle : HandleBase {};
struct FenceHandle : HandleBase {};
struct SemaphoreHandle : HandleBase {};
struct SwapchainHandle : HandleBase {};
struct ShaderModuleHandle : HandleBase {};

// --- Enums ---

enum class BackendType : uint8_t {
    Null,
    Vulkan,
    Metal,
    DX12,
};

enum class QueueType : uint8_t {
    Graphics,
    Compute,
    Copy,
};

enum class BufferUsage : uint32_t {
    None = 0,
    Vertex = 1 << 0,
    Index = 1 << 1,
    Uniform = 1 << 2,
    Storage = 1 << 3,
    Staging = 1 << 4,
};

inline BufferUsage operator|(BufferUsage a, BufferUsage b) {
    return static_cast<BufferUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

enum class MemoryHint : uint8_t {
    GpuOnly,
    CpuToGpu,
    GpuToCpu,
};

enum class TextureFormat : uint16_t {
    Unknown,
    R8G8B8A8_UNORM,
    B8G8R8A8_UNORM,
    R16G16B16A16_FLOAT,
    D24_UNORM_S8_UINT,
    D32_FLOAT,
};

enum class TextureUsage : uint32_t {
    None = 0,
    Sampled = 1 << 0,
    RenderTarget = 1 << 1,
    DepthStencil = 1 << 2,
    Storage = 1 << 3,
};

inline TextureUsage operator|(TextureUsage a, TextureUsage b) {
    return static_cast<TextureUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

enum class ShaderStage : uint8_t {
    Vertex,
    Fragment,
    Compute,
};

enum class LoadOp : uint8_t {
    Load,
    Clear,
    DontCare,
};

enum class StoreOp : uint8_t {
    Store,
    DontCare,
};

enum class IndexType : uint8_t {
    Uint16,
    Uint32,
};

enum class PrimitiveTopology : uint8_t {
    TriangleList,
    TriangleStrip,
    LineList,
};

enum class CompareOp : uint8_t {
    Never,
    Less,
    Equal,
    LessOrEqual,
    Greater,
    NotEqual,
    GreaterOrEqual,
    Always,
};

enum class BlendFactor : uint8_t {
    Zero,
    One,
    SrcAlpha,
    OneMinusSrcAlpha,
};

// --- Descriptors ---

struct DeviceDesc {
    BackendType backend = BackendType::Null;
    bool enableValidation = true;
    bool enableDebugNaming = false;
    const char* applicationName = "openEngine";
};

struct BufferDesc {
    uint64_t size = 0;
    BufferUsage usage = BufferUsage::None;
    MemoryHint memoryHint = MemoryHint::GpuOnly;
    const char* debugName = nullptr;
};

struct TextureDesc {
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t depth = 1;
    uint32_t mipLevels = 1;
    TextureFormat format = TextureFormat::Unknown;
    TextureUsage usage = TextureUsage::None;
    const char* debugName = nullptr;
};

struct ShaderModuleDesc {
    ShaderStage stage = ShaderStage::Vertex;
    const char* entryPoint = "main";
    const void* bytecode = nullptr;
    size_t bytecodeSize = 0;
    const char* source = nullptr;
    const char* debugName = nullptr;
};

struct VertexAttribute {
    uint32_t location = 0;
    TextureFormat format = TextureFormat::R8G8B8A8_UNORM;
    uint32_t offset = 0;
};

struct VertexBinding {
    uint32_t binding = 0;
    uint32_t stride = 0;
};

struct VertexLayout {
    std::vector<VertexBinding> bindings;
    std::vector<VertexAttribute> attributes;
};

struct RasterizerState {
    bool cullBackFaces = true;
    bool frontCounterClockwise = false;
};

struct DepthStencilState {
    bool depthTest = true;
    bool depthWrite = true;
    CompareOp depthCompare = CompareOp::Less;
};

struct BlendAttachmentState {
    bool blendEnable = false;
    BlendFactor srcFactor = BlendFactor::SrcAlpha;
    BlendFactor dstFactor = BlendFactor::OneMinusSrcAlpha;
};

struct GraphicsPipelineDesc {
    ShaderModuleDesc vertexShader{};
    ShaderModuleDesc fragmentShader{};
    VertexLayout vertexLayout{};
    RasterizerState rasterizer{};
    DepthStencilState depthStencil{};
    BlendAttachmentState blend{};
    PrimitiveTopology topology = PrimitiveTopology::TriangleList;
    TextureFormat colorAttachmentFormat = TextureFormat::R8G8B8A8_UNORM;
    TextureFormat depthAttachmentFormat = TextureFormat::D32_FLOAT;
    const char* debugName = nullptr;
};

struct ComputePipelineDesc {
    ShaderModuleDesc computeShader{};
    const char* debugName = nullptr;
};

struct ColorAttachmentDesc {
    TextureHandle texture{};
    LoadOp loadOp = LoadOp::Clear;
    StoreOp storeOp = StoreOp::Store;
    float clearColor[4] = {0.f, 0.f, 0.f, 1.f};
};

struct DepthAttachmentDesc {
    TextureHandle texture{};
    LoadOp loadOp = LoadOp::Clear;
    StoreOp storeOp = StoreOp::Store;
    float clearDepth = 1.f;
    uint32_t clearStencil = 0;
};

struct RenderPassDesc {
    std::vector<ColorAttachmentDesc> colorAttachments;
    DepthAttachmentDesc depthAttachment{};
    bool hasDepthAttachment = false;
};

struct BarrierDesc {
    // Simplified barrier for initial implementation
    bool bufferBarrier = false;
    bool textureBarrier = false;
};

struct DrawDesc {
    uint32_t vertexCount = 0;
    uint32_t instanceCount = 1;
    uint32_t firstVertex = 0;
    uint32_t firstInstance = 0;
};

struct DrawIndexedDesc {
    uint32_t indexCount = 0;
    uint32_t instanceCount = 1;
    uint32_t firstIndex = 0;
    int32_t vertexOffset = 0;
    uint32_t firstInstance = 0;
};

struct DispatchDesc {
    uint32_t groupCountX = 1;
    uint32_t groupCountY = 1;
    uint32_t groupCountZ = 1;
};

struct SubmitDesc {
    std::vector<CommandBufferHandle> commandBuffers;
    std::vector<SemaphoreHandle> waitSemaphores;
    std::vector<SemaphoreHandle> signalSemaphores;
    FenceHandle signalFence{};
};

struct SwapchainDesc {
    void* nativeWindow = nullptr;
    uint32_t width = 0;
    uint32_t height = 0;
    TextureFormat preferredFormat = TextureFormat::B8G8R8A8_UNORM;
    bool vsync = true;
};

} // namespace oe
