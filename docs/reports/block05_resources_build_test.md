# Block 5 보고서: Resources + Build + Test

**일자:** 2026-06-29  
**상태:** 완료

## 목표

Buffer/Texture/Pipeline 리소스 API를 common 레이어에 연결하고, CMake 빌드와 통합 테스트로 전체 stack을 검증한다.

## 구현 내용

### Resource API

| API | common layer | backend delegate |
|-----|--------------|------------------|
| `createBuffer` / `destroyBuffer` / `updateBuffer` | desc validation, handle pool | slot 기반 buffer state |
| `createTexture` / `destroyTexture` | desc validation | slot 기반 texture state |
| `createGraphicsPipeline` / `createComputePipeline` / `destroyPipeline` | shader presence validation | slot 기반 pipeline state |

### Umbrella header

`include/targets/common/api.h` — 모든 public header 일괄 include

### CMake

- `oe_common` static library: common + null backend
- `common_api_tests` executable
- CTest 등록

### 통합 테스트 (`tests/common_api_test.cpp`)

| 테스트 | 검증 항목 |
|--------|-----------|
| `testDeviceLifecycle` | create/destroy |
| `testBufferAndDrawPath` | buffer → texture → pipeline → cmd record → submit |
| `testValidationRejectsInvalidBuffer` | size=0 buffer 거부 |

## 빌드 결과

```
cmake -B build -S .
cmake --build build
./build/common_api_tests
→ common_api_tests PASSED
```

## 파일 트리 (신규)

```
include/targets/common/
  api.h, error.h, types.h, device.h, buffer.h, texture.h,
  pipeline.h, command_buffer.h, sync.h, swapchain.h

targets/common/
  error.cpp, validation.cpp, debug_layer.cpp,
  device_context.cpp, device.cpp, buffer.cpp, texture.cpp,
  pipeline.cpp, command_buffer.cpp, sync.cpp, swapchain.cpp,
  backend_factory.cpp, handle_pool.h, backend_interface.h, ...

targets/null/
  null_backend.h, null_backend.cpp

tests/common_api_test.cpp
CMakeLists.txt
```

## 미구현 / 후속 작업 (Block 6+)

- [ ] `targets/vulkan` — 첫 real backend
- [ ] `targets/metal` / `targets/dx12`
- [ ] descriptor set / bindless (현재 `cmdBindDescriptorSets` 미포함)
- [ ] shader module 별도 handle (`ShaderModuleHandle` 타입만 존재)
- [ ] map/unmap buffer API
- [ ] leak check / debug naming backend 연동

## 결론

설계 문서 Block 1~5 범위 구현 완료. Null backend 기준 end-to-end render path(buffer → texture → pipeline → draw → submit)가 동작하며, validation layer가 invalid input을 차단한다.
