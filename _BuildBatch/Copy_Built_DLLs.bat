@echo off
setlocal EnableDelayedExpansion


:: ======================================================= Setup ======================================================= 

echo ================ Build_Dependancy_Debug_x64 ================


set SSENGINE_PHYSX_BUILD_RESULT_DEBUG=..\_BuildResults\PhysX_Debug_x64
set SSENGINE_PHYSX_BUILD_RESULT_RELEASE=..\_BuildResults\PhysX_Release_x64

set SSENGINE_FBXSDK_BUILD_RESULT_PREFIX_DEBUG=..\_BuildResults\Debug
set SSENGINE_FBXSDK_BUILD_RESULT_PREFIX_RELEASE=..\_BuildResults\Release


set DLL_COPY_TARGET_DEBUG=..\SSEditor\x64\Debug\
set DLL_COPY_TARGET_RELEASE=..\SSEditor\x64\Release\

:: ======================================================= Run ======================================================= 


echo.
echo.
echo     ================ Copy_PhysX_Built_Dlls ================
echo.

copy /y "%SSENGINE_PHYSX_BUILD_RESULT_DEBUG%\*.dll" "%DLL_COPY_TARGET_DEBUG%"
copy /y "%SSENGINE_PHYSX_BUILD_RESULT_DEBUG%\*.pdb" "%DLL_COPY_TARGET_DEBUG%"
copy /y "%SSENGINE_PHYSX_BUILD_RESULT_RELEASE%\*.dll" "%DLL_COPY_TARGET_RELEASE%"
copy /y "%SSENGINE_PHYSX_BUILD_RESULT_RELEASE%\*.pdb" "%DLL_COPY_TARGET_RELEASE%"

echo     ================ ~Copy_PhysX_Built_Dlls ================


echo.
echo.
echo     ================ Copy_FBXSD_Built_Dlls ================
echo.

copy /y "%SSENGINE_FBXSDK_BUILD_RESULT_PREFIX_DEBUG%\DLL\libfbxsdk.dll" "%DLL_COPY_TARGET_DEBUG%"
copy /y "%SSENGINE_FBXSDK_BUILD_RESULT_PREFIX_RELEASE%\DLL\libfbxsdk.dll" "%DLL_COPY_TARGET_RELEASE%"

echo     ================ ~Copy_FBXSD_Built_Dlls ================
echo.


echo ================ ~Build_Dependancy_Debug_x64 ================


echo.
echo.
pause

endlocal