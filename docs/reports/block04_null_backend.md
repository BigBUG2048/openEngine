# Block 4 보고서: Null Backend

**일자:** 2026-06-29  
**상태:** 완료

## 목표

GPU 없이 common API 전체를 검증할 수 있는 stub backend를 구현한다.

## 구현 내용

### NullBackend (`targets/null/`)

`IBackend` 인터페이스 전체 구현. 실제 GPU 메모리/드라이버 호출 없음.

| 리소스 | Null 동작 |
|--------|-----------|
| Buffer | desc 저장, CpuToGpu hint 시 staging vector 유지 |
| Texture | desc만 저장 |
| Pipeline | slot 존재 여부만 추적 |
| CommandBuffer | recorded command string log |
| Fence | signaled 플래그 |
| Semaphore | 존재 여부만 추적 |
| Swapchain | 2-frame fake image, rotate frameIndex |

### backend_factory.cpp

```cpp
createBackend(BackendType::Null) → NullBackend
Vulkan / Metal / DX12 → Unsupported (미구현)
```

### Command recording log

Null backend는 각 cmd* 호출을 문자열로 기록 (예: `"Draw(count=3,instances=1)"`).  
테스트/디버그 시 API 호출 순서 introspection 가능.

## 설계 의도

- CI/headless 환경에서 GPU 없이 빌드·테스트
- real backend 추가 전 API shape 확정
- common validation + backend delegate 경로 검증

## 산출물

```
targets/null/null_backend.h
targets/null/null_backend.cpp
targets/common/backend_factory.cpp
```

## 검증

- `BackendType::Null`로 device 생성 성공
- submit 시 `submitCount_` 증가, fence signal 처리
- updateBuffer가 staging buffer에 데이터 기록

## 다음 Block

Buffer/Texture/Pipeline public API + CMake + 통합 테스트
