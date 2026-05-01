@echo off
setlocal EnableDelayedExpansion


:: ======================================================= Setup ======================================================= 

echo ================ Build_Dependancy_Debug_x64 ================

set MSBUILD_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
set MS_NUGET_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"


set SSGAL_MODULE_NAME=SSGAL
set SSGAL_MODULE_PATH="..\!SSGAL_MODULE_NAME!\!SSGAL_MODULE_NAME!.sln"


set "PHYSX_BUILD_OPTION_DEBUG=/m /nologo /verbosity:quiet /clp:ErrorsOnly;NoSummary /p:Configuration="Debug""
set "PHYSX_BUILD_OPTION_CHECKED=/m /nologo /verbosity:quiet /clp:ErrorsOnly;NoSummary /p:Configuration="checked""

set PHYSX_BULID_TOOL_PATH="..\PhysX\physx"
set PHYSX_BUILD_SLN_PATH="..\PhysX\physx\compiler\vc17win64-cpu-only\PhysXSDK.sln"

set PHYSX_BUILD_RESULT_PATH_DEBUG=..\PhysX\physx\bin\win.x86_64.vc143.md\debug
set PHYSX_BUILD_RESULT_PATH_CHECKED=..\PhysX\physx\bin\win.x86_64.vc143.md\checked


set PHYSX_NEEDED_MODULE_NAMES[0]=LowLevel_static_64
set PHYSX_NEEDED_MODULE_NAMES[1]=LowLevelAABB_static_64
set PHYSX_NEEDED_MODULE_NAMES[2]=LowLevelDynamics_static_64
set PHYSX_NEEDED_MODULE_NAMES[3]=PhysX_64
set PHYSX_NEEDED_MODULE_NAMES[4]=PhysXCharacterKinematic_static_64
set PHYSX_NEEDED_MODULE_NAMES[5]=PhysXCommon_64
set PHYSX_NEEDED_MODULE_NAMES[6]=PhysXCooking_64
set PHYSX_NEEDED_MODULE_NAMES[7]=PhysXExtensions_static_64
set PHYSX_NEEDED_MODULE_NAMES[8]=PhysXFoundation_64
set PHYSX_NEEDED_MODULE_NAMES[9]=PhysXPvdSDK_static_64
set PHYSX_NEEDED_MODULE_NAMES[10]=PhysXTask_static_64
set PHYSX_NEEDED_MODULE_NAMES[11]=PVDRuntime_64
set PHYSX_NEEDED_MODULE_NAMES[12]=SceneQuery_static_64
set PHYSX_NEEDED_MODULE_NAMES[13]=SimulationController_static_64
set PHYSX_NEEDED_MODULE_CNT=13

for /L %%i in (0,1,%PHYSX_NEEDED_MODULE_CNT%) do (
    set PHYSX_MODULE_PREFIX_DEBUG[%%i]=!PHYSX_BUILD_RESULT_PATH_DEBUG!\!PHYSX_NEEDED_MODULE_NAMES[%%i]!
    set PHYSX_MODULE_PREFIX_CHECKED[%%i]=!PHYSX_BUILD_RESULT_PATH_CHECKED!\!PHYSX_NEEDED_MODULE_NAMES[%%i]!
)


set SSENGINE_PHYSX_BUILD_RESULT_DEBUG=..\_BuildResults\PhysX_Debug_x64
set SSENGINE_PHYSX_BUILD_RESULT_RELEASE=..\_BuildResults\PhysX_Release_x64


set "IMGUI_BUILD_OPTION_DEBUG=/m /nologo /verbosity:quiet /clp:ErrorsOnly;NoSummary /p:Configuration="Debug""
set "IMGUI_BUILD_OPTION_RELEASE=/m /nologo /verbosity:quiet /clp:ErrorsOnly;NoSummary /p:Configuration="Release""
set IMGUI_BUILD_SLN_PATH="..\SSImgui\SSImgui.sln"



set FBX_SDK_LIB_FOLDER_DEBUG=..\ExternLibs\FBXSDK\lib\x64\Debug
set FBX_SDK_LIB_FOLDER_RELEASE=..\ExternLibs\FBXSDK\lib\x64\Release
set FBX_SDK_DLL_COPY_TARGET_DEBUG=..\_BuildResults\Debug\DLL
set FBX_SDK_DLL_COPY_TARGET_RELEASE=..\_BuildResults\Release\DLL


:: ======================================================= Run ======================================================= 


echo.
echo.
echo     ================ Restore_DX12_Nuget_Package ================
echo.
%MSBUILD_PATH% %SSGAL_MODULE_PATH% -t:Restore -p:RestorePackagesConfig=true
echo     ================ ~Restore_DX12_Nuget_Package ================


echo.
echo.
echo     ================ PhysX_Git_Submodule_Update ================
echo.
git submodule update --init --recursive
echo     ================ ~PhysX_Git_Submodule_Update ================


echo.
echo.
echo     ================ PhysX_Generate_Project ================
echo.
call %PHYSX_BULID_TOOL_PATH%/generate_projects.bat vc17win64-cpu-only
echo     ================ ~PhysX_Generate_Project ================


echo.
echo.
echo     ================ PhysX_Run_Build ================
echo.
echo     Building with Debug option... 
%MSBUILD_PATH% %PHYSX_BUILD_SLN_PATH% %PHYSX_BUILD_OPTION_DEBUG%
echo     Building with Checked option... 
%MSBUILD_PATH% %PHYSX_BUILD_SLN_PATH% %PHYSX_BUILD_OPTION_CHECKED%
echo     ================ ~PhysX_Run_Build ================


echo.
echo.
echo     ================ PhysX_Copy_BuildResult ================
echo.

if %ERRORLEVEL% neq 0 (
     echo         @@@@@@@ PHYSX BUILD FAILED !!! Errno: %ERRORLEVEL% @@@@@@@
)

echo.
echo.
echo     Copy PhysX Debug Bins... 
echo.
if not exist "%SSENGINE_PHYSX_BUILD_RESULT_DEBUG%" mkdir "%SSENGINE_PHYSX_BUILD_RESULT_DEBUG%"
for /L %%i in (0,1,%PHYSX_NEEDED_MODULE_CNT%) do (

    copy /y "!PHYSX_MODULE_PREFIX_DEBUG[%%i]!.*" "%SSENGINE_PHYSX_BUILD_RESULT_DEBUG%\"
)

echo.
echo.
echo     Copy PhysX Release Bins... 
echo.
if not exist "%SSENGINE_PHYSX_BUILD_RESULT_RELEASE%" mkdir "%SSENGINE_PHYSX_BUILD_RESULT_RELEASE%"
for /L %%i in (0,1,%PHYSX_NEEDED_MODULE_CNT%) do (

    copy /y "!PHYSX_MODULE_PREFIX_CHECKED[%%i]!.*" "%SSENGINE_PHYSX_BUILD_RESULT_RELEASE%/"
)


echo     ================ ~PhysX_Copy_BuildResult ================



echo.
echo.
echo     ================ ImGui_Build ================
echo.

echo     Building with Debug option... 
%MSBUILD_PATH% %IMGUI_BUILD_SLN_PATH% %IMGUI_BUILD_OPTION_DEBUG%
echo     Building with Checked option... 
%MSBUILD_PATH% %IMGUI_BUILD_SLN_PATH% %IMGUI_BUILD_OPTION_RELEASE%

echo     ================ ~ImGui_Build ================



echo.
echo.
echo     ================ FBX_SDK_Copy_DLL ================
echo.

if not exist "%FBX_SDK_DLL_COPY_TARGET_DEBUG%" mkdir "%FBX_SDK_DLL_COPY_TARGET_DEBUG%"
if not exist "%FBX_SDK_DLL_COPY_TARGET_RELEASE%" mkdir "%FBX_SDK_DLL_COPY_TARGET_RELEASE%"



echo     ================ ~FBX_SDK_Copy_DLL ================

echo ================ ~Build_Dependancy_Debug_x64 ================


echo.
echo.
pause

endlocal