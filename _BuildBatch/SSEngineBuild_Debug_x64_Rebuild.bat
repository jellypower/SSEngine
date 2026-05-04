@echo off
setlocal EnableDelayedExpansion

echo SSEngineBuild_Debug_x64_Rebuild

set MSBUILD_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
set "BUILD_OPTION=/m /nologo /verbosity:minimal /clp:Summary;ErrorsOnly /p:Configuration="Debug" /t:Rebuild"



set MODULE_NAMES[0]=ss-sqllite
set MODULE_NAMES[1]=SSEngineDefault
set MODULE_NAMES[2]=SObject
set MODULE_NAMES[3]=SSGAL
set MODULE_NAMES[4]=SSRenderer
set MODULE_NAMES[5]=SSCollision
set MODULE_NAMES[6]=SSFBXImporter
set MODULE_NAMES[7]=SSAssetDBManager
set MODULE_NAMES[8]=SSContentsBase
set MODULE_NAMES[9]=SSGameModule
set MODULE_NAMES[10]=SSEditor
set MODULE_CNT=10

for /L %%i in (0,1,%MODULE_CNT%) do (
    set MODULE_PATH[%%i]="../!MODULE_NAMES[%%i]!/!MODULE_NAMES[%%i]!.sln"
)


echo ======== ALL BUILD ========
echo.

for /L %%i in (0,1,%MODULE_CNT%) do (
    echo     ==== BUILD MODULE: !MODULE_NAMES[%%i]! ====
    %MSBUILD_PATH% !MODULE_PATH[%%i]! %BUILD_OPTION%
    echo     ==== ~BUILD MODULE: !MODULE_NAMES[%%i]! ====
    echo.
    echo.
)

echo.
echo ======== ~ALL BUILD ========



pause

endlocal