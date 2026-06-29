# Block 2 보고서: Device API + common 구현

**일자:** 2026-06-29  
**상태:** 완료

## 목표

Device 생명주기, queue 조회, backend 위임 구조, validation 레이어의 진입점을 구현한다.

## 구현 내용

### 공개 API (`device.h`)

```cpp
Result<DeviceHandle> createDevice(const DeviceDesc& desc);
void destroyDevice(DeviceHandle device);
Result<QueueHandle> getQueue(DeviceHandle device, QueueType type, uint32_t index = 0);
```

### DeviceRegistry / DeviceContext (internal)

- **DeviceRegistry:** singleton, device handle pool + context vector
- **DeviceContext:** device별 backend instance, 모든 resource handle pool 보유
- device 생성 시 `createBackend(desc.backend)` 호출 → Null backend 연결
- 기본 queue 3개(Graphics/Compute/Copy) 사전 할당

### validation.cpp

- `validateDeviceDesc`: applicationName 필수
- `validateBufferDesc`, `validateTextureDesc`, `validateGraphicsPipelineDesc` 등 (Block 5에서 활용)

### backend_interface.h

- `IBackend` 가상 인터페이스: 모든 GPU 작업의 backend delegate 계약
- common 레이어는 validation 후 slot index로 backend에 위임

## 아키텍처

```
createDevice()
    → validateDeviceDesc()
    → createBackend(BackendType)
    → backend->initialize()
    → DeviceRegistry에 DeviceContext 등록
    → DeviceHandle 반환
```

## 산출물

```
include/targets/common/device.h
targets/common/device.cpp
targets/common/device_context.h
targets/common/device_context.cpp
targets/common/backend_interface.h
targets/common/validation.h
targets/common/validation.cpp
targets/common/debug_layer.h
targets/common/debug_layer.cpp
```

## 검증

- `testDeviceLifecycle`: create → destroy 정상 동작
- invalid device handle 접근 시 `InvalidHandle` 반환

## 다음 Block

CommandBuffer record/submit 인터페이스 및 pass 상태 추적
