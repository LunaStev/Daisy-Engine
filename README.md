# Daisy Engine

THE END 시리즈 전용 게임엔진 - 모듈형 C++ 게임엔진

## 개요

Daisy Engine은 THE END 시리즈를 위해 설계된 모듈형 게임엔진입니다. 언리얼 엔진과 같은 복잡한 엮임 구조 대신, 각 모듈이 독립적으로 작동할 수 있도록 설계되었습니다.

## 주요 특징

- **모듈형 아키텍처**: 각 모듈이 독립적으로 작동
- **무한 월드**: 관측 가능한 우주 크기 비례 스케일
- **제로 로딩**: 예측적 스트리밍과 LOD 시스템
- **고급 AI**: 자율적인 NPC 경제·사회·전투 시스템
- **멀티플레이어**: Single Shard 구조로 대규모 접속 지원
- **Vulkan 기반 렌더링**: 고성능 그래픽

## 엔진 모듈 구조

```
Daisy Engine
├── Core                    # 엔진 코어 및 모듈 관리
├── DaisyPhysics           # 물리 엔진 (중력, 충돌, 유체역학)
├── DaisyRender            # 렌더링 엔진 (Vulkan, 절차 생성, LOD)
├── DaisySound             # 오디오 엔진 (3D 사운드, Doppler, 음성채팅)
├── DaisyAI                # AI 엔진 (경제, 사회, 전투, 학습)
├── DaisyNet               # 네트워크 엔진 (Single Shard, 모드 지원)
├── WorldStreamer          # 월드 스트리밍 (무한 월드, 예측 로딩)
└── ScriptSystem           # 스크립트 시스템 (게임 로직, 모드)
```

## 빌드 요구사항

- **컴파일러**: C++20 지원 (MSVC 2022, GCC 11+, Clang 13+)
- **빌드 시스템**: CMake 3.20+
- **그래픽 API**: Vulkan SDK 1.2+
- **플랫폼**: Windows, DaisyOS

## 빌드 방법

### Windows (Visual Studio)

```bash
# 저장소 클론
git clone <repository-url>
cd "Daisy Engine"

# CMake 빌드 폴더 생성
mkdir build
cd build

# Visual Studio 프로젝트 생성
cmake .. -G "Visual Studio 17 2022"

# 빌드
cmake --build . --config Release
```

### Linux / DaisyOS

```bash
# 저장소 클론
git clone <repository-url>
cd "Daisy Engine"

# 의존성 설치 (Ubuntu/Debian)
sudo apt-get install build-essential cmake libvulkan-dev

# 빌드
mkdir build && cd build
cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 빌드 결과물

빌드가 성공하면 다음 실행파일들이 생성됩니다:

- `build/Examples/BasicApp/DaisyEngineExample` - 기본 엔진 예제
- `build/Editor/DaisyEditor/DaisyEditor` - Daisy Editor (게임 에디터)

### 실행 방법

```bash
# 기본 예제 실행
./build/Examples/BasicApp/DaisyEngineExample

# 에디터 실행
./build/Editor/DaisyEditor/DaisyEditor
```

## 사용 예제

### 기본 엔진 초기화

```cpp
#include "DaisyEngine.h"

int main() {
    // 엔진 초기화
    if (!DAISY_ENGINE.Initialize()) {
        return -1;
    }
    
    auto* engine = DAISY_ENGINE.GetModule<Engine>();
    
    // 모듈 등록
    auto* physics = engine->RegisterModule<DaisyPhysics>();
    auto* renderer = engine->RegisterModule<DaisyRender>();
    auto* sound = engine->RegisterModule<DaisySound>();
    auto* ai = engine->RegisterModule<DaisyAI>();
    
    // 메인 루프
    while (engine->IsRunning()) {
        engine->Update();
    }
    
    DAISY_ENGINE.Shutdown();
    return 0;
}
```

### 절차적 콘텐츠 생성

```cpp
// 행성 생성
Vector3 earthPos(0, 0, 0);
renderer->GenerateProceduralPlanet(earthPos, 6371000); // 지구

// 도시 생성
renderer->GenerateProceduralCity(Vector3(0, 6371000, 0), 1000000);

// 우주선 생성
renderer->GenerateProceduralShip(Vector3(400000, 0, 0), 100);
```

### AI 에이전트 생성

```cpp
// AI 에이전트 생성
uint32_t citizenId = ai->CreateAIAgent("Citizen_001", Vector3(0, 6371000, 0));

// 행동 패턴 설정
ai->SetAgentBehavior(citizenId, AIBehaviorType::Economic);
ai->SetAgentPersonality(citizenId, 0.3f, 0.8f, 0.9f); // 낮은 공격성, 높은 지능, 높은 협력

// 목표 추가
ai->AddAgentGoal(citizenId, "find_resources");
ai->AddAgentGoal(citizenId, "build_shelter");
```

### 물리 시뮬레이션

```cpp
// 중력장 추가
physics->AddGravityWell(Vector3(0, 0, 0), 5.972e24f, 1e8f, true); // 지구

// 강체 생성
uint32_t spacecraftId = physics->CreateRigidBody(Vector3(7000000, 0, 0), 10000);

// 추력 적용
physics->ApplyForce(spacecraftId, Vector3(0, 1000, 0));
```

## 모듈별 상세 기능

### DaisyPhysics
- 은하/행성 궤도 시뮬레이션
- 대기권 및 유체역학
- LOD 기반 충돌 최적화
- 중력장 및 다중체 물리

### DaisyRender
- Vulkan 기반 고성능 렌더링
- 절차적 행성/도시/함선 생성
- 무한 월드 스케일링
- 실시간 LOD 시스템

### DaisyAI
- 경제 AI (무역, 생산, 소비)
- 사회 AI (건설, 협력, 반란)
- 전투 AI (지상전, 우주전)
- 학습 가능한 동적 행동 패턴

### DaisyNet
- Single Shard 구조
- 다중 클라이언트 동시 접속
- 마인크래프트식 모드/플러그인 지원
- 커뮤니티 서버 지원

## 개발자 가이드

### 새 모듈 추가

1. `Engine/Modules/` 하위에 새 폴더 생성
2. `Module` 클래스를 상속하는 새 클래스 구현
3. CMakeLists.txt에 모듈 추가
4. 메인 CMakeLists.txt에서 서브디렉터리 추가

### 디버깅

Debug 빌드에서 메모리 추적이 자동으로 활성화됩니다:

```cpp
#ifdef DAISY_DEBUG
    // 메모리 할당 추적
    MyClass* obj = DAISY_NEW(MyClass, param1, param2);
    
    // 메모리 해제
    DAISY_DELETE(obj);
    
    // 메모리 리포트 출력
    DAISY_LOG.PrintMemoryReport();
#endif
```

## 성능 최적화

### LOD 시스템
```cpp
renderer->EnableLOD(true);
renderer->SetLODDistances(100.0f, 500.0f, 2000.0f);
```

### 월드 스트리밍
```cpp
StreamingSettings settings;
settings.loadRadius = 1000000.0f;        // 로딩 반경
settings.predictionRadius = 500000.0f;   // 예측 로딩 반경
settings.enablePredictiveStreaming = true;
worldStreamer->SetStreamingSettings(settings);
```

## Daisy Editor

Daisy Engine에는 통합 에디터가 포함되어 있어 게임 개발을 위한 완전한 환경을 제공합니다.

### 에디터 기능

- **씬 계층구조**: 엔티티 관리 및 부모-자식 관계 설정
- **인스펙터**: 선택된 오브젝트의 속성 편집
- **에셋 브라우저**: 프로젝트 파일 관리 및 임포트
- **뷰포트**: 3D 씬 미리보기 및 카메라 조작
- **콘솔**: 실시간 로그 및 디버깅 정보
- **메뉴바**: 프로젝트 관리 및 에디터 설정

### 에디터 조작법

- **카메라 이동**: 마우스 + 키보드 (WASD)
- **오브젝트 선택**: 뷰포트 또는 씬 계층구조에서 클릭
- **속성 편집**: 인스펙터에서 Transform 등 편집
- **에셋 관리**: 에셋 브라우저에서 드래그 앤 드롭

### 지원 에셋 형식

- **씬**: `.dscene`
- **메시**: `.obj`, `.fbx`, `.gltf`, `.glb`
- **텍스처**: `.png`, `.jpg`, `.tga`, `.bmp`, `.dds`
- **사운드**: `.wav`, `.mp3`, `.ogg`
- **스크립트**: `.ds` (Daisy Script), `.lua`
- **셰이더**: `.vert`, `.frag`, `.hlsl`, `.glsl`

## 성능 특징

- **제로 로딩**: 예측적 스트리밍으로 끊김 없는 월드 탐험
- **무한 스케일**: 은하 크기까지 확장 가능한 월드
- **모듈형 아키텍처**: 필요한 기능만 로드하여 메모리 효율성
- **대규모 AI**: 수천 개의 NPC가 동시에 활동
- **고성능 렌더링**: Vulkan 기반 최적화된 그래픽

## 라이센스

[라이센스 정보]

## 기여

[기여 가이드라인]

## 지원

이슈나 질문이 있으시면 [이슈 트래커]에서 제기해주세요.