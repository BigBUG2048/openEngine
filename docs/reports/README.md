# Common 3D API 구현 보고서

설계 문서 [`common_api_design.md`](../common_api_design.md) 기준으로 단계별 구현 진행 상황입니다.

| Block | 내용 | 상태 | 보고서 |
|-------|------|------|--------|
| 1 | types.h + error.h | 완료 | [block01](block01_types_and_error.md) |
| 2 | Device API + common 구현 | 완료 | [block02](block02_device.md) |
| 3 | CommandBuffer record/submit | 완료 | [block03](block03_command_buffer.md) |
| 4 | Null backend | 완료 | [block04](block04_null_backend.md) |
| 5 | Buffer/Texture/Pipeline + CMake + 테스트 | 완료 | [block05](block05_resources_build_test.md) |
| 6 | Real backend (Vulkan/Metal/DX12) | 미착수 | — |

## 빌드 및 테스트

```bash
cmake -B build -S .
cmake --build build
ctest --test-dir build
./build/common_api_tests
```

## 생성된 주요 경로

```
include/targets/common/   # 공개 API 헤더
targets/common/           # common 구현 + validation
targets/null/             # Null backend
tests/common_api_test.cpp # 통합 테스트
CMakeLists.txt
```
