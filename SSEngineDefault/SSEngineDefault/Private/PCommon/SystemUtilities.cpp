#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/SystemUtilities.h"
#include <filesystem>

bool ConvertToWorkingDirPath(SS::StringW& InOutPath)
{
	if (InOutPath.GetStrLen() <=0)
	{
		return false;
	}

	SS::StringW WorkingDir;
	bool bResult = GetCurWorkingDirPath(WorkingDir);
	if (bResult == false)
	{
		SS_ASSERT(false);
		return false;
	}

	int32 WorkingDirLen = WorkingDir.GetStrLen();
	if (wcsncmp(WorkingDir.C_Str(), InOutPath.C_Str(), WorkingDirLen) != 0)
	{
		return false;
	}

	const utf16* Repath = (InOutPath.C_Str() + WorkingDirLen + 1);
	InOutPath = Repath;

	return true;
}

bool GetCurWorkingDirPath(SS::StringW& OutPath)
{
	std::filesystem::path currentPath = std::filesystem::current_path();
	OutPath = currentPath.c_str();

	const int32 WorkingDirLen = OutPath.GetStrLen();
	if (WorkingDirLen <= 0)
	{
		SS_ASSERT(false);
		return false;
	}

	OutPath.Replace(L"\\", L"/");
	return true;
}
