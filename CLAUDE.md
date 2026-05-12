# SSEngine — Claude Instructions

For engine architecture, module structure, and data flows, see [Architecture.md](Architecture.md).

---

# Development Environment
- IDE: Visual Studio 2022
- Language: C++ (C++17)
- Platform: Windows x64
- Graphics API: DirectX 12 (managed via NuGet)
- Build System: MSBuild (each module has its own independent .sln)


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
- Each module uses a Precompiled Header (`pch.h`) for common includes — suggest creating 'pch.h' if there is no pch

## Braces
- Always use braces for control flow bodies, even single-line:
```cpp
// correct
if (condition)
{
    DoSomething();
}

// forbidden
if (condition)
    DoSomething();
```
