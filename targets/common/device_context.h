#pragma once

#include "targets/common/types.h"
#include "backend_interface.h"
#include "handle_pool.h"

#include <memory>
#include <vector>

namespace oe {
namespace detail {

struct CommandBufferState {
    bool recording = false;
    QueueType queue = QueueType::Graphics;
    bool inRenderPass = false;
    bool inComputePass = false;
    PipelineHandle boundPipeline{};
    uint32_t deviceIndex = HandleBase::kInvalidIndex;
};

struct DeviceContext {
    DeviceHandle handle{};
    DeviceDesc desc{};
    std::unique_ptr<IBackend> backend;

    HandlePool<QueueHandle> queues;
    HandlePool<BufferHandle> buffers;
    HandlePool<TextureHandle> textures;
    HandlePool<PipelineHandle> pipelines;
    HandlePool<CommandBufferHandle> commandBuffers;
    HandlePool<FenceHandle> fences;
    HandlePool<SemaphoreHandle> semaphores;
    HandlePool<SwapchainHandle> swapchains;

    std::vector<CommandBufferState> commandBufferStates;

    CommandBufferState& cmdState(uint32_t index) {
        if (index >= commandBufferStates.size()) {
            commandBufferStates.resize(index + 1);
        }
        return commandBufferStates[index];
    }
};

class DeviceRegistry {
public:
    static DeviceRegistry& instance();

    Result<DeviceHandle> create(const DeviceDesc& desc);
    void destroy(DeviceHandle handle);

    DeviceContext* get(DeviceHandle handle);
    const DeviceContext* get(DeviceHandle handle) const;
    DeviceContext* findByCommandBuffer(CommandBufferHandle cmd);
    DeviceContext* findByQueue(QueueHandle queue);
    DeviceContext* findByFence(FenceHandle fence);
    DeviceContext* findBySemaphore(SemaphoreHandle semaphore);
    DeviceContext* findBySwapchain(SwapchainHandle swapchain);

private:
    HandlePool<DeviceHandle> devices_;
    std::vector<std::unique_ptr<DeviceContext>> contexts_;
};

} // namespace detail
} // namespace oe
