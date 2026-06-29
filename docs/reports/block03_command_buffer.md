# Block 3 보고서: CommandBuffer + Sync + Swapchain

**일자:** 2026-06-29  
**상태:** 완료

## 목표

커맨드 기록 API, submit/sync 경로, swapchain 인터페이스를 common 레이어에 구현한다.

## 구현 내용

### CommandBuffer API

| 함수 | 역할 |
|------|------|
| `beginCommandBuffer` / `endCommandBuffer` | recording 상태 관리 |
| `cmdBeginRenderPass` / `cmdEndRenderPass` | render pass 경계 |
| `cmdBeginComputePass` / `cmdEndComputePass` | compute pass 경계 |
| `cmdBindPipeline`, `cmdBindVertexBuffers`, `cmdBindIndexBuffer` | 바인딩 |
| `cmdDraw`, `cmdDrawIndexed`, `cmdDispatch` | draw/dispatch |
| `cmdCopyBuffer`, `cmdBarrier` | copy/barrier |

### 상태 검증 (common layer)

- recording 중이 아니면 draw/dispatch 거부
- render pass 밖에서 draw 거부
- compute pass 밖에서 dispatch 거부
- pass 열린 채 `endCommandBuffer` 거부

### Sync API

- `createFence` / `createSemaphore` / `destroy*`
- `submit(QueueHandle, SubmitDesc)` — command buffer 유효성 검사 후 backend 위임
- `waitFence` / `resetFence`

### Swapchain API

- `createSwapchain` / `destroySwapchain`
- `acquireNextImage` / `present`

### Registry lookup

command buffer, queue, fence, semaphore, swapchain은 device handle 없이 사용되므로 `DeviceRegistry`에 역조회 메서드 추가:

- `findByCommandBuffer`
- `findByQueue`
- `findByFence` / `findBySemaphore` / `findBySwapchain`

## 산출물

```
include/targets/common/command_buffer.h
include/targets/common/sync.h
include/targets/common/swapchain.h
targets/common/command_buffer.cpp
targets/common/sync.cpp
targets/common/swapchain.cpp
```

## 검증

- `testBufferAndDrawPath`: begin → render pass → bind → draw → end → submit 전체 경로 통과
- invalid state에서 common validation이 backend 호출 전에 차단

## 다음 Block

Null backend 구현으로 GPU 없이 end-to-end 검증
