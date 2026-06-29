# Common 3D API 설계

openEngine의 하위 3D API 공통 레이어(`include/targets/common`, `targets/common`) 설계 문서입니다.  
Vulkan, Metal, DX12 등 실제 GPU 백엔드는 이후 단계에서 구현하며, common 레이어는 **포팅 가능한 계약(contract)** 을 정의합니다.

---

## 목표

- 엔진/애플리케이션 코드가 GPU 백엔드에 직접 의존하지 않도록 한다.
- Vulkan / Metal / DX12가 모두 표현 가능한 **최소 공통 denominator** API를 제공한다.
- common 레이어에서 검증, 상태 추적, 디버그 기능을 일관되게 제공한다.
- 백엔드 교체, hot reload, validation layer 추가가 용이하도록 한다.

---

## 레이어 구조

```
Application / Engine
        ↓
  targets/common        ← 포팅 가능한 3D API (헤더 + 공통 구현)
        ↓
  targets/vulkan
  targets/metal
  targets/dx12          ← 이후 추가
        ↓
  OS / Driver
```

| 경로 | 역할 |
|------|------|
| `include/targets/common/` | 공개 API — 타입, 핸들, enum, 함수 선언 |
| `targets/common/` | 공통 구현 — 검증, 상태 추적, 리소스 생명주기, 디버그 레이어 |
| `targets/<backend>/` | 실제 GPU 호출만 담당 |

**핵심 원칙: common에 GPU 종속 코드를 넣지 않는다.**

---

## 책임 분리

### common에 두는 것

| 영역 | 예시 |
|------|------|
| 핸들/ID | `BufferHandle`, `TextureHandle` (opaque) |
| 리소스 설명 | `BufferDesc`, `TextureDesc`, `PipelineDesc` |
| 상태/동기화 개념 | `QueueType`, `Fence`, `Semaphore` (추상) |
| 커맨드 기록 | `CommandBuffer` 인터페이스 (record만, submit은 backend) |
| 생명주기 | create/destroy, ref-count 또는 pool |
| 디버그 | validation, naming, leak check |

### backend에만 두는 것

- 실제 device / swapchain 생성
- memory allocation (VMA, Metal heap 등)
- shader 컴파일 / 리플렉션 (SPIR-V, MSL, DXIL)
- pipeline state object 생성
- queue submit, present

---

## 디렉터리 구조 (초안)

```
include/targets/common/
  types.h              # 핸들, enum, 공통 타입
  device.h
  buffer.h
  texture.h
  pipeline.h
  command_buffer.h
  pass.h
  sync.h
  shader.h
  swapchain.h
  error.h

targets/common/
  device.cpp           # validation, handle table
  resource_pool.cpp
  command_buffer.cpp
  validation.cpp
  debug_layer.cpp

targets/null/          # GPU 없이 API 검증용 (권장)
targets/vulkan/        # 이후
targets/metal/         # 이후
targets/dx12/          # 이후
```

---

## 핵심 설계 결정

### 1. 최소 공통 denominator

Vulkan / Metal / DX12가 모두 표현 가능한 descriptor만 common에 둔다.

- Render pass는 고정 vs dynamic 여부가 API마다 다르므로, common API는 **attachment + load/store + clear** 수준으로 추상화한다.
- Metal의 tile-based deferred rendering, DX12의 render pass tier 등 백엔드별 차이는 backend 내부에서 흡수한다.

### 2. 핸들 vs raw pointer

- common은 **opaque handle** + backend 내부 lookup table을 사용한다.
- backend 교체, hot reload, validation에 유리하다.
- handle은 generation counter를 포함해 use-after-free를 조기에 탐지할 수 있다.

```cpp
// 예시 (개념)
struct BufferHandle { uint32_t index; uint32_t generation; };
```

### 3. 에러 모델

backend마다 예외 / HRESULT / VkResult 형태가 다르므로 common에서 통일한다.

- `Result<T, Error>` 또는 `bool + getLastError()` 중 하나를 선택해 일관되게 사용한다.
- `Error` enum은 common에 정의하고, backend는 native error를 common error로 매핑한다.

### 4. Shader 경로 분리

| 레이어 | 책임 |
|--------|------|
| common | `ShaderModuleDesc { entry, stage, bytecode_or_source }` |
| backend | SPIR-V → MSL / DXIL 변환, reflection, PSO 바인딩 |

엔진은 shader를 common descriptor로 전달하고, 백엔드가 플랫폼별 컴파일/캐시를 담당한다.

### 5. Swapchain

- common에 `Swapchain` 인터페이스만 둔다.
- surface / window 연동은 platform + backend가 처리한다.
- present mode, format 선택 등은 backend extension 또는 capability query로 노출한다.

---

## Common API Surface (초안)

### Device

```cpp
DeviceHandle createDevice(const DeviceDesc& desc);
void destroyDevice(DeviceHandle device);

QueueHandle getQueue(DeviceHandle device, QueueType type, uint32_t index);
```

- `DeviceDesc`: validation layer 활성화, debug naming, preferred backend hint 등
- `QueueType`: Graphics, Compute, Copy (Transfer)

### Buffer

```cpp
BufferHandle createBuffer(DeviceHandle device, const BufferDesc& desc);
void destroyBuffer(DeviceHandle device, BufferHandle buffer);
void updateBuffer(DeviceHandle device, BufferHandle buffer, const void* data, size_t size, size_t offset);
```

- `BufferDesc`: size, usage (Vertex, Index, Uniform, Storage, Staging), memory hint (CPU/GPU visible)
- map/unmap은 staging buffer 패턴 또는 optional backend capability로 제공

### Texture

```cpp
TextureHandle createTexture(DeviceHandle device, const TextureDesc& desc);
void destroyTexture(DeviceHandle device, TextureHandle texture);
```

- `TextureDesc`: width, height, depth, mipLevels, format, usage (Sampled, RenderTarget, DepthStencil, Storage)
- format enum은 common에 정의, backend가 native format으로 변환

### Pipeline

```cpp
PipelineHandle createGraphicsPipeline(DeviceHandle device, const GraphicsPipelineDesc& desc);
PipelineHandle createComputePipeline(DeviceHandle device, const ComputePipelineDesc& desc);
void destroyPipeline(DeviceHandle device, PipelineHandle pipeline);
```

- `GraphicsPipelineDesc`: shader modules, vertex layout, rasterizer, depth/stencil, blend state, render pass compatibility
- render pass compatibility는 attachment format/layout 수준의 추상 descriptor

### Pass (Render / Compute)

```cpp
void cmdBeginRenderPass(CommandBufferHandle cmd, const RenderPassDesc& desc);
void cmdEndRenderPass(CommandBufferHandle cmd);

void cmdBeginComputePass(CommandBufferHandle cmd);
void cmdEndComputePass(CommandBufferHandle cmd);
```

- `RenderPassDesc`: color attachments, depth attachment, load/store ops, clear values
- subpass는 초기에는 미지원 또는 단일 subpass로 단순화

### CommandBuffer

```cpp
CommandBufferHandle beginCommandBuffer(DeviceHandle device, QueueType queue);
void endCommandBuffer(CommandBufferHandle cmd);

void cmdBindPipeline(CommandBufferHandle cmd, PipelineHandle pipeline);
void cmdBindVertexBuffers(CommandBufferHandle cmd, ...);
void cmdBindIndexBuffer(CommandBufferHandle cmd, ...);
void cmdBindDescriptorSets(CommandBufferHandle cmd, ...);
void cmdDraw(CommandBufferHandle cmd, ...);
void cmdDrawIndexed(CommandBufferHandle cmd, ...);
void cmdDispatch(CommandBufferHandle cmd, ...);

void cmdCopyBuffer(CommandBufferHandle cmd, ...);
void cmdCopyTexture(CommandBufferHandle cmd, ...);
void cmdBarrier(CommandBufferHandle cmd, const BarrierDesc& desc);
```

- record는 common validation + backend delegate
- submit은 backend 전담

### Submit & Sync

```cpp
FenceHandle createFence(DeviceHandle device);
SemaphoreHandle createSemaphore(DeviceHandle device);

void submit(QueueHandle queue, const SubmitDesc& desc);
void waitFence(FenceHandle fence, uint64_t timeout);
void resetFence(FenceHandle fence);
```

- `SubmitDesc`: command buffers, wait semaphores, signal semaphores, signal fence

### Swapchain

```cpp
SwapchainHandle createSwapchain(DeviceHandle device, const SwapchainDesc& desc);
void destroySwapchain(SwapchainHandle swapchain);
TextureHandle acquireNextImage(SwapchainHandle swapchain, SemaphoreHandle signal);
void present(SwapchainHandle swapchain, QueueHandle queue, SemaphoreHandle wait);
```

---

## Null Backend (`targets/null`)

GPU 없이 common API를 검증하기 위한 stub backend.

| 목적 | 설명 |
|------|------|
| API shape 검증 | create/destroy, handle table, validation layer 동작 확인 |
| CI / headless | GPU 없는 환경에서 빌드·테스트 |
| 설계 조기 확정 | real backend 전에 desc/API 틀어짐 방지 |

Null backend는 실제 GPU 메모리 할당 없이 handle과 상태만 추적한다.

---

## 구현 순서

1. **`types.h`** — handle, enum, desc 구조체 정의
2. **`Device`** — create/destroy, queue 조회
3. **`CommandBuffer`** — record/submit 인터페이스
4. **`targets/null`** — GPU 없이 common API 검증
5. **Buffer / Texture / Pipeline** — 리소스 생성 및 bind/draw 경로
6. **첫 real backend** — 타겟 OS 기준 Vulkan 또는 Metal
7. **두 번째 backend** — desc/API 포팅성 재검증

---

## 백엔드 우선순위 (참고)

| 기준 | Vulkan-first | Metal-first |
|------|--------------|-------------|
| 크로스플랫폼 | Windows + Linux + Android | macOS / iOS |
| API 노출도 | explicit, validation layer 풍부 | Apple 생태계 최적 |
| 학습 곡선 | 높음 | 중간 |

- **데스크톱/크로스플랫폼** 목표 → Vulkan-first 권장
- **Apple 전용** 목표 → Metal-first 권장
- Null backend를 먼저 두면 backend 선택과 무관하게 common API를 굳힐 수 있다.

---

## 비목표 (초기 단계)

- Ray tracing, mesh shader 등 tier-2 기능
- Multi-GPU, VR 전용 API
- Shader 컴파일러 자체 구현 (backend 또는 외부 도구 위임)
- Windowing / input (platform 레이어 책임)

---

## 다음 단계

- [x] `types.h` handle/desc 초안 작성
- [x] `error.h` 에러 모델 확정
- [x] `targets/null` 스켈레톤 구현
- [ ] 첫 real backend 선택 (Vulkan / Metal)
- [x] CMake 또는 빌드 시스템에 `targets/common`, `targets/null` 추가

구현 진행 보고서: [`docs/reports/README.md`](reports/README.md)
