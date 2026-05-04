# Project 목표
- 프로젝트 이름: SSEngine
- Unity 와 Unreal이 가지고 있는 각자의 강점을 융합하여 나만의 게임엔진을 만들어내는 것이 목표
- 기술적 공부를 위한 개발이 아닌 실제 제품을 만들어 낼 수 있을만큼 구조화된 엔진을 만들어내는 것


# 개발 환경
- IDE: Visual Studio 2022
- 언어: C++ (C++17)
- 플랫폼: Windows x64
- 그래픽스 API: DirectX 12 (NuGet 패키지로 관리)
- 빌드 시스템: MSBuild (각 모듈이 독립적인 .sln을 가짐)


# 엔진 구조

## 자체 모듈 목록
/SSEngineDefault          -> 엔진 전역에서 공통적으로 사용가능한 기본 타입, 컨테이너, 수학함수 등을 포함하는 모듈
/SSContentsBase           -> 콘텐츠단에서 쉽게 사용할 수 있게 다양한 렌더, 물리 기능들을 추상화하여 올려주는 모듈
/SSRenderer               -> 렌더 패스를 구성하거나, 렌더링에 필요한 오브젝트를 대변하는 인스턴스를 구성하거나, 현재 World에서 렌더할 콘텍스트를 관리하는 등의 목적으로 사용됨. 일반적으로 ContentsBase에서 활용 가능하게 구현된 인터페이스를 노출하는 모듈
/SSGAL                    -> 그래픽스 API종속적인 기능과 리소스를 추상화하여 렌더러에 노출하는 모듈
/SSCollision              -> 충돌, 오브젝트움직임, 물리, 오버랩과 같은 기능을 포함하는 모듈.
/SObject                  -> 컨텐츠 레벨의 오브젝트를 식별하기 위한 고유 ID를 발행하고 고유 ID를 활용한 델리게이트, 라이프사이클 관리에 도움을 주는 기능을 포함하는 모듈. 일반적으로 언리얼의 UObject 쪽과 대응.
/SSAssetDBManager         -> sqlite를 활용한 프로젝트의 에셋 리스트를 관리하는 모듈. 에셋 매니지먼트를 위한 sqlite기능을 외부에 숨기기 위한 것이 주 목적.
/SSFBXImporter            -> FBXSDK를 활용해 모델을 자체 포맷으로 변환하는것이 주 목적인 모듈. 파일화는 등의 기능은 SSRenderer 에 직접적으로 담겨있음. FBXSDK기능을 외부에 숨기기 위한 것이 주 목적.
/SSGameModule             -> 실제 콘텐츠가 제작되는 모듈. EngineDefault, ContentsBase 등 콘텐츠단에서 활용 가능한 기능들을 활용해 실제 게임 로직을 구현하는 곳.

## 외부 모듈 목록
- 외부 모듈은 직접 수정할 일 없게 .claudeignore에 작성해놨음
/ExternLibs/FBXSDK        -> 모델 임포팅을 위한 FBXSDK. 현재 사용하는 버전은 2020.3.4
/PhysX                    -> 물리, 충돌에 사용하는 NVidia의 PhysX 프로젝트. 우선은 CUDA관련 기능은 제외하고 CPU측 기능만 활용하는중.
/ss-sqllite               -> 에셋이나 콘텐츠 데이터 일괄관리를 위한 sqlite 프로젝트의 자체 빌드 버전. 자체로 수정할 계획은 아직 없음.
/SSImgui                  -> 에디터 기능 작성을 위한 ImGUI 자체 빌드 버전. 자체로 수정할 계획은 아직 없음.


## exe 목록
/SSEditor                 -> 실제 Windows메시지 루프가 있는 게임 엔트리용 exe 프로젝트. 에디터 기능과 ImGUI기능들의 경우 해당 exe 프로젝트에서 곧바로 구현되고 사용함
/SSGame                   -> 실제 Windows메세지 루프가 있는 게임 엔트리용 exe 프로젝트. Editor의 기능은 전부 제외돼있고 실제 게임 구동만을 위한 기능만 사용


# 빌드 방법

## 최초 세팅 (클론 후 1회)
`_BuildBatch/Build_Dependancy_x64.bat` 실행
- PhysX git submodule update 및 빌드 (Debug / Checked 구성)
- ImGui 빌드 (Debug / Release)
- FBXSDK DLL/LIB을 `_BuildResults`로 복사
- SSGAL의 DirectX 12 NuGet 패키지 복원

## 일반 빌드
`_BuildBatch/SSEngineBuild_Debug_x64.bat` 또는 `SSEngineBuild_Release_x64.bat` 실행

### 모듈 빌드 순서 (의존성 순)
ss-sqllite → SSEngineDefault → SObject → SSGAL → SSRenderer → SSCollision → SSFBXImporter → SSAssetDBManager → SSContentsBase → SSGameModule → SSEditor

## 빌드 후
`_BuildBatch/Copy_Built_DLLs.bat` 실행 - 빌드된 DLL들을 실행 경로로 복사


# 코딩 컨벤션

## 네이밍
- 멤버 변수: `_` 접두사 + PascalCase (예: `_Foundation`, `_WorldName`)
- 인터페이스 클래스: `I` 접두사 (예: `ICollInstanceBase`, `ICollDevice`)
- 열거형 타입: `E` 접두사 (예: `ECollShapeType`)
- 엔진 자체 컨테이너/타입: `SS::` 네임스페이스 (예: `SS::HashMap`, `SS::SHasherW`)
- 일반 구현 클래스: 접두사 없음 (예: `CollisionWorld`, `CollDevice`)

## 타입
- `int32`, `uint32`, `int64`, `uint64`, `int16`, `uint16`, `int8`, `uint8` 사용 (SSNativeKeywords.h 정의)
- `utf16` (wchar_t), `utf8` (char) 사용
- `FORCEINLINE` 매크로로 인라인 강제

## 구조
- Public/Private 폴더로 헤더 분리. Public은 외부 모듈에 노출, Private는 내부 구현
- 인터페이스는 Public에, 구현체는 Private에 위치
- PCH(Precompiled Header) 사용: 각 모듈의 `pch.h`에 공통 헤더 포함
