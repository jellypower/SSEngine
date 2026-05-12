# SSEngine

![image.png](https://github.com/jellypower/zzPublicImageDataBase/blob/main/Portfolio/SSEngine/SSEngine_Play_Capture.png)

- Unity 와 Unreal이 가지고 있는 각자의 강점을 융합하여 **나만의 게임엔진**을 만들어내고자 하는 프로젝트입니다.
- 단순히 기술적 공부를 위한 개발이 아닌 실제 제품을 만들어 낼 수 있을만큼 구조화된 엔진을 만들어내는것을 목표로 하는 프로젝트입니다.

# 프로젝트 정보

## 엔진 구조

![SSEngine_Simple_Architecture.png](https://github.com/jellypower/zzPublicImageDataBase/blob/main/Portfolio/SSEngine/SSEngine_Simple_Architecture.png)

- **`SSEngineDefault.dll`**: 엔진 전역에서 **공통**적으로 사용가능한 기본 타입, 컨테이너, 수학함수 등을 포함하는 모듈
- **`SSGameModule.dll`**: 실제 **콘텐츠**가 제작되는 모듈. 게임의 기본 기능들을 활용해 월드를 어떻게 구성하고 캐릭터를 어떻게 조작할지에 대한 룰을 정의합니다.
- **`SSContentsBase.dll`**: 콘텐츠단에서 쉽게 사용할 수 있게 **다양한 렌더, 물리 기능들을 추상화**하여 올려주는 모듈. (ex. StaticMeshComponent, AnimationComponent …)
- **`SSRenderer.dll`**: 렌더 패스를 구성하거나, 렌더링에 필요한 오브젝트(라이트, 메시)등을 추상화해 ContentsBase에서 사용할 수 있게 구현된 기능을 모아두는 **렌더 기능 모듈**.
- **`SSGAL.dll`**: **그래픽스 API종속적인 기능과 리소스를 추상화**하여 렌더러에 노출하는 모듈. (DirectX 12)
- **`SSCollision.dll`**: **충돌**, 오브젝트움직임, 물리, 오버랩과 같은 기능을 포함하는 모듈. (PhysX 기반)
- **`SObject.dll`**: 컨텐츠 레벨의 오브젝트를 식별하기 위한 **고유 ID**를 발행하고 고유 ID를 활용한 델리게이트, 라이프사이클 관리에 도움을 주는 기능을 포함하는 모듈.
- **`SSAssetDBManager.dll`**: SQLite를 활용해 프로젝트의 **에셋 목록을 관리**하는 모듈.
- **`SSFBXImporter.dll`**: FBXSDK를 활용해 모델을 **엔진 내부 포맷으로 변환**하는 모듈.
- **`SSEditor.exe`**: 실제 Windows메시지 루프가 있는 **게임 엔트리**용 exe 프로젝트. 에디터 및 ImGui 기능 포함.
- **`SSGame.exe`**: 에디터 기능을 제외한 **게임 구동 전용** exe 프로젝트.

## 빌드 및 실행방법

### 종속성 로드

1. 본 엔진은 모델을 로딩하기 위해 `FBXSDK` 를 다운받아야 합니다.
    - 현재 호환되는 버전은 `2020.3.4` 입니다.
    - 다운받은 SDK의 `lib`와 `include` 폴더를 루트폴더에 `SSEngine/ExternLibs/FBXSDK` 폴더 안에 위치시킵니다. (만약 ExternLibs 폴더가 없다면 폴더를 만들어주세요)
    - FBXSDK 링크
        - [FBX SDK 2020.3.4 다운로드](https://www.autodesk.com/content/dam/autodesk/www/adn/fbx/2020-3-4/fbx202034_fbxsdk_vs2022_win.exe)
        - [오토데스크 공식 FBX SDK 다운로드 페이지](https://aps.autodesk.com/developer/overview/fbx-sdk)
2. 프로젝트 root 에 위치한 _BuildBatch 폴더에 있는 `Build_Dependancy_x64.bat` 를 실행합니다.
    - **주의:** `PhysX` 를 빌드하기 위해선 `cmake`다운로드와 환경변수 등록이 필요합니다. **(PATH)**


### 프로젝트 빌드

- 프로젝트 root 에 위치한 _BuildBatch 폴더에 있는 다음의 배치파일들을 실행해 빌드합니다.
    - `SSEngineBuild_Debug_x64.bat`: 디버그 옵션으로 빌드합니다
    - `SSEngineBuild_Debug_x64_Rebuild.bat`: 디버그 옵션으로 전체 프로젝트를 다시 빌드합니다
    - `SSEngineBuild_Release_x64.bat`: 릴리즈 옵션으로 빌드합니다
    - `SSEngineBuild_Release_x64_Rebuild.bat`: 릴리즈 옵션으로 전체 프로젝트를 다시 빌드합니다

### 설정 및 실행

1. 위 배치파일들을 통해 빌드 옵션에 따른 바이너리가 만들어졌으면 실행할 exe의 작업 디렉토리(기본 설정은 `SSEngine\SSEditor\SSEditor`) 에 리소스를 위치시킵니다.
    - 만약 이미 `Resource` 폴더가 있다면 전부 삭제하고 다시 붙여넣어 주세요.
2. 리소스 파일들은 GitHub의 Release섹션에 `Resource.zip` 에서 다운받을 수 있습니다.
3. 해당 파일을 압축해제하고 작업 디렉토리에 리소스를 위치시킵니다.
    - *주의: exe를 직접 실행할 경우 Working Directory가 달라지기에 exe파일이 위치한 폴더에 Resource를 배치해야 합니다.*
4. _BuildBatch 폴더에 있는 `Copy_Built_DLLs.bat` 를 실행합니다.
    - 해당 배치는 외부 라이브러리의 빌드된 DLL(PhysX, FBXSDK ...)을 실제 실행할 exe폴더에 카피합니다.
    - 해당 배치파일은 Debug빌드와 Release빌드 할 때 각 각 실행해줘야 정상 실행이 가능합니다.
5. 이제 `SSEditor.sln` 파일을 열어 `f5`로 프로젝트를 실행하거나 빌드된 exe를 더블클릭하여 실행할 수 있습니다.


# **기본 컨트롤**

- 시점조작: 마우스 이동
- 움직이기: W/A/S/D
- 시점고정 및 조준: 마우스 우클릭
- 에디터 모드 토글: P

# 관련 링크
- 엔진 제작 일지: https://velog.io/@jellypower/series/%EB%82%98%EB%8A%94-%EB%A7%8C%EB%93%A0%EB%8B%A4-%EA%B2%8C%EC%9E%84%EC%97%94%EC%A7%84
- 데모영상 모음:
    - 애니메이션 블렌드 스페이스: https://youtu.be/J_K8yV4KCV8?si=NNhoajY9qWv9HO9p
    - 가속도 기반 캐릭터 무브먼트: https://youtu.be/KHo7lmSJnLM?si=jeiCSwU_f6rGcJwt
    - Convex한 오브젝트간의 충돌판정: https://youtu.be/iz16O6yGidU?si=sZ4iAbdajRUEDG6G


# 문제 해결 안내

- 종속성 구성은 사용자마다 각기 다른 환경적인 요인으로 인해 정상적으로 작동하지 않을 수 있습니다. 설정 과정에서 문제가 발생할 경우, **dongcheold147@gmail.com**으로 문의해 주시기 바랍니다.


## 바로가기
[<img src="https://github.com/jellypower/zzPublicImageDataBase/blob/main/Icon/Youtube_logo.png" width="64"/>](https://www.youtube.com/watch?v=J_K8yV4KCV8&list=PLn_nqBk81UiCGGYGs5YFQIz098fdHk2NS&index=3)
[<img src="https://github.com/jellypower/zzPublicImageDataBase/blob/main/Icon/velog_icon.png" width="64"/>](https://velog.io/@jellypower/series/%EB%82%98%EB%8A%94-%EB%A7%8C%EB%93%A0%EB%8B%A4-%EA%B2%8C%EC%9E%84%EC%97%94%EC%A7%84)

---

<br/>
<br/>
<br/>


# SSEngine

![image.png](https://github.com/jellypower/zzPublicImageDataBase/blob/main/Portfolio/SSEngine/SSEngine_Play_Capture.png)

- This project aims to develop a proprietary game engine by merging the unique strengths of both Unity and Unreal Engine.
- Rather than developing solely for technical study, the objective is to build a highly structured, production-ready engine capable of powering real-world commercial products.

# Project Info

## Engine Architecture

![SSEngine_Simple_Architecture.png](https://github.com/jellypower/zzPublicImageDataBase/blob/main/Portfolio/SSEngine/SSEngine_Simple_Architecture.png)

- **`SSEngineDefault.dll`**: A core module containing fundamental types, containers, and mathematical functions used globally throughout the engine.
- **`SSGameModule.dll`**: The gameplay logic module where actual content is authored. It defines world composition and character control rules using the engine's base features.
- **`SSContentsBase.dll`**: Provides high-level abstractions of rendering and physics features (e.g., `StaticMeshComponent`, `AnimationComponent`) for easy access within the gameplay layer.
- **`SSRenderer.dll`**: A dedicated rendering module that manages render pass composition and provides abstractions for renderable objects (lights, meshes) used by `SSContentsBase`.
- **`SSGAL.dll`**: Abstracts Graphics API-specific functions and resources, exposing a unified interface to the Renderer. (DirectX 12)
- **`SSCollision.dll`**: Handles collision detection, object movement, physics, and overlap event functionality. (PhysX-based)
- **`SObject.dll`**: Manages unique ID issuance for content-level objects and provides utilities for ID-based delegates and lifecycle management.
- **`SSAssetDBManager.dll`**: Manages the project asset list using SQLite.
- **`SSFBXImporter.dll`**: Converts models into the engine's internal format using FBXSDK.
- **`SSEditor.exe`**: The main entry point executable containing the Windows message loop. Editor features and ImGui are implemented directly inside this project.
- **`SSGame.exe`**: A shipping entry point executable with all editor features stripped out, containing only game runtime functionality.

## Build and Execution

### Dependency Loading

1. This engine requires the `FBX SDK` for model loading.
    - The currently compatible version is **`2020.3.4`**.
    - Place the `lib` and `include` folders from the downloaded SDK into the following directory: `SSEngine/ExternLibs/FBXSDK` (If the `ExternLibs` folder does not exist, please create it manually.)
    - **FBX SDK Links:**
        - [Download FBX SDK 2020.3.4 (Direct)](https://www.autodesk.com/content/dam/autodesk/www/adn/fbx/2020-3-4/fbx202034_fbxsdk_vs2022_win.exe)
        - [Official Autodesk FBX SDK Download Page](https://aps.autodesk.com/developer/overview/fbx-sdk)
2. Run `Build_Dependancy_x64.bat` located in the `_BuildBatch` folder at the project root.
    - **Note:** To build `PhysX`, you must have `CMake` installed and registered in your system environment variables **(PATH)**.


### Building the Project

- Navigate to the `_BuildBatch` folder in the project root and run the following batch files to build:
    - `SSEngineBuild_Debug_x64.bat`: Builds the project with Debug configuration.
    - `SSEngineBuild_Debug_x64_Rebuild.bat`: Performs a clean rebuild with Debug configuration.
    - `SSEngineBuild_Release_x64.bat`: Builds the project with Release configuration.
    - `SSEngineBuild_Release_x64_Rebuild.bat`: Performs a clean rebuild with Release configuration.

### Setup and Execution

1. Once the binaries are generated, place the necessary resources into the working directory of the executable (Default: `SSEngine\SSEditor\SSEditor`).
    - If the `Resource` folder already exists, please delete it entirely and then paste the new one.
2. Resource files can be downloaded from **Resource.zip** in the GitHub **Releases** section.
3. Extract the zip file and ensure the resources are placed correctly in the working directory.
   - *Note: If launching via the `.exe` file, the resources must be located in the same folder as the executable due to the change in working directory.*
4. Run Copy_Built_DLLs.bat located in the _BuildBatch folder.
    - This batch file copies the built DLLs of external libraries (PhysX, FBXSDK, etc.) into the folder where the actual executable (exe) is located.
    - This batch file must be run for both Debug and Release builds respectively to ensure the application runs correctly.
5. You can then run the project via **SSEditor.sln** (Press `F5`) or by launching the built `.exe` directly.


# Controls

- **Camera Look**: Mouse Movement
- **Movement**: W / A / S / D
- **Aim / Lock Camera**: Right Mouse Click (Hold)
- **Toggle Editor Mode**: P

# Related Links
- Engine Development Logs (Devlog): https://velog.io/@jellypower/series/%EB%82%98%EB%8A%94-%EB%A7%8C%EB%93%A0%EB%8B%A4-%EA%B2%8C%EC%9E%84%EC%97%94%EC%A7%84
- Demo Videos:
    - Animation Blend Spaces: https://youtu.be/J_K8yV4KCV8?si=NNhoajY9qWv9HO9p
    - Acceleration-based Character Movement: https://youtu.be/KHo7lmSJnLM?si=jeiCSwU_f6rGcJwt
    - Collision Detection between Convex Objects: https://youtu.be/iz16O6yGidU?si=sZ4iAbdajRUEDG6G

# Note on Troubleshooting

Dependency configuration may fail due to various environmental factors. If you encounter any issues, please contact **[dongcheold147@gmail.com](mailto:dongcheold147@gmail.com)** for support.

## Shortcut
[<img src="https://github.com/jellypower/zzPublicImageDataBase/blob/main/Icon/Youtube_logo.png" width="64"/>](https://www.youtube.com/watch?v=J_K8yV4KCV8&list=PLn_nqBk81UiCGGYGs5YFQIz098fdHk2NS&index=3)
[<img src="https://github.com/jellypower/zzPublicImageDataBase/blob/main/Icon/velog_icon.png" width="64"/>](https://velog.io/@jellypower/series/%EB%82%98%EB%8A%94-%EB%A7%8C%EB%93%A0%EB%8B%A4-%EA%B2%8C%EC%9E%84%EC%97%94%EC%A7%84)

---
