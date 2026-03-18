#pragma once
#include "SSContainer/SSString/SSStringW.h"
#include "SSEngineDefault/ModuleExportKeyword.h"


SSENGINEDEFAULT_MODULE bool ConvertToWorkingDirPath(SS::StringW& InOutPath);
SSENGINEDEFAULT_MODULE bool GetCurWorkingDirPath(SS::StringW& InOutPath);


SSENGINEDEFAULT_MODULE void SetCursorHide(bool bHide);
SSENGINEDEFAULT_MODULE void FixCursorCenter(bool bFix);