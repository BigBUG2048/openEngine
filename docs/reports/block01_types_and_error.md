# Block 1 보고서: types.h + error.h

**일자:** 2026-06-29  
**상태:** 완료

## 목표

공통 3D API의 기반 타입, opaque handle, descriptor 구조체, 통일된 에러 모델을 정의한다.

## 구현 내용

### error.h / error.cpp

- `Error` enum: `InvalidHandle`, `InvalidArgument`, `InvalidState`, `Unsupported` 등
- `Result<T>` 템플릿: `ok()`, `success()`, `failure()` 팩토리
- `Result<void>` 특수화
- thread-local `getLastError()` / `setLastError()` / `getLastErrorMessage()`

### types.h

| 카테고리 | 항목 |
|----------|------|
| Handle | `DeviceHandle`, `BufferHandle`, `TextureHandle`, `PipelineHandle`, `CommandBufferHandle`, `FenceHandle`, `SemaphoreHandle`, `SwapchainHandle` |
| Enum | `BackendType`, `QueueType`, `BufferUsage`, `TextureFormat`, `ShaderStage`, `LoadOp`, `StoreOp` 등 |
| Desc | `DeviceDesc`, `BufferDesc`, `TextureDesc`, `GraphicsPipelineDesc`, `RenderPassDesc`, `SubmitDesc`, `SwapchainDesc` |

Handle은 `index + generation` 패턴으로 use-after-free 탐지를 지원한다.

### handle_pool.h (internal)

- `HandlePool<HandleT>`: 슬롯 할당/해제, generation 증가, `isValid()` 검사

## 설계 결정

- **에러 모델:** `Result<T>` + thread-local last error (backend별 native error 매핑 준비)
- **Handle:** raw pointer 대신 opaque handle — backend lookup table과 연동
- **Desc:** Vulkan/Metal/DX12 공통 denominator만 포함 (subpass 미포함, 단일 render pass)

## 산출물

```
include/targets/common/error.h
include/targets/common/types.h
targets/common/error.cpp
targets/common/handle_pool.h
```

## 검증

- 컴파일 성공 (후속 block과 함께 `oe_common` 라이브러리로 빌드)
- Block 5 테스트에서 invalid buffer desc가 `InvalidArgument`로 거부됨 확인

## 다음 Block

Device create/destroy, queue 조회, `DeviceRegistry` + `DeviceContext` 도입
