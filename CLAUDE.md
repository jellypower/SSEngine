# Project Goals
- Project name: SSEngine
- The goal is to build a personal game engine by combining the strengths of both Unity and Unreal Engine
- The aim is not just technical study, but a structured engine capable of shipping a real product
- Targeting cross-platform compatibility beyond Windows in the long term

# Development Environment
- IDE: Visual Studio 2022
- Language: C++ (C++17)
- Platform: Windows x64
- Graphics API: DirectX 12 (managed via NuGet)
- Build System: MSBuild (each module has its own independent .sln)


# Engine Architecture

## Engine Modules
/SSEngineDefault          -> Contains primitive types, containers, and math functions shared across the entire engine
/SSContentsBase           -> Abstracts various rendering and physics features into a content-friendly interface
/SSRenderer               -> Manages render passes, render object instances, and the renderable context of the current World. Exposes interfaces consumed by ContentsBase
/SSGAL                    -> Abstracts graphics API-specific resources and functionality, exposing them to the Renderer
/SSCollision              -> Handles collision detection, object movement, physics simulation, and overlap queries
/SObject                  -> Issues unique IDs for content-level objects and provides delegate and lifecycle management built on top of those IDs. Roughly analogous to UObject in Unreal Engine
/SSAssetDBManager         -> Manages the project asset list using SQLite. Primary purpose is hiding SQLite internals from the rest of the engine
/SSFBXImporter            -> Converts models to the engine's internal format using FBXSDK. Serialization lives in SSRenderer; this module exists mainly to encapsulate FBXSDK internals
/SSGameModule             -> The module where actual game content is authored, using features exposed by EngineDefault, ContentsBase, and other engine layers

## Third-Party Modules
- Listed in .claudeignore since they are not modified directly
/ExternLibs/FBXSDK        -> FBXSDK for model importing. Currently using version 2020.3.4
/PhysX                    -> NVIDIA PhysX for physics and collision. Currently using CPU-only mode (no CUDA)
/ss-sqllite               -> Custom-built SQLite for asset and content data management. No plans to modify directly
/SSImgui                  -> Custom-built Dear ImGui for editor UI. No plans to modify directly


## Executables
/SSEditor                 -> Windows message loop entry point for the game. Editor features and ImGui are implemented directly inside this project
/SSGame                   -> Windows message loop entry point for shipping. All editor features are stripped out; only game runtime functionality is included


# Build Instructions

## First-Time Setup (run once after cloning)
Run `_BuildBatch/Build_Dependancy_x64.bat`
- Updates PhysX git submodule and builds it (Debug / Checked configurations)
- Builds ImGui (Debug / Release)
- Copies FBXSDK DLL/LIB into `_BuildResults`
- Restores DirectX 12 NuGet packages for SSGAL

## Regular Build
Run `_BuildBatch/SSEngineBuild_Debug_x64.bat` or `SSEngineBuild_Release_x64.bat`

### Module Build Order (by dependency)
ss-sqllite → SSEngineDefault → SObject → SSGAL → SSRenderer → SSCollision → SSFBXImporter → SSAssetDBManager → SSContentsBase → SSGameModule → SSEditor

## Post-Build
Run `_BuildBatch/Copy_Built_DLLs.bat` — copies built DLLs to the executable output path


# Coding Conventions

## Naming
- Member variables: `_` prefix + PascalCase (e.g. `_Foundation`, `_WorldName`)
- Interface classes: `I` prefix (e.g. `ICollInstanceBase`, `ICollDevice`)
- Enum types: `E` prefix (e.g. `ECollShapeType`)
- Engine containers/types: `SS::` namespace (e.g. `SS::HashMap`, `SS::SHasherW`)
- Concrete classes: no prefix (e.g. `CollisionWorld`, `CollDevice`)

## Types
- Use engine-defined integer types: `int32`, `uint32`, `int64`, `uint64`, `int16`, `uint16`, `int8`, `uint8` (defined in SSNativeKeywords.h)
- Use `utf16` (wchar_t) and `utf8` (char) for character types
- Use `FORCEINLINE` macro to force inlining

## Structure
- Headers split into Public/Private folders. Public headers are exposed to other modules; Private headers are internal implementation
- Interfaces go in Public; concrete implementations go in Private
- Each module uses a Precompiled Header (`pch.h`) for common includes
