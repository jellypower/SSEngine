#include "pch.h"

#include "OpenFilePathDialogue.h"



HRESULT OpenSystemPathDialogue(SS::StringW& OutFilePath, ESystemPathDialogueOption Option)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog* pFileOpen;

		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			FILEOPENDIALOGOPTIONS OriginalOption;
			pFileOpen->GetOptions(&OriginalOption);


			if (Option & SPD_CREATEPATH)
			{
				OriginalOption = OriginalOption & (~FOS_FILEMUSTEXIST);
			}



			pFileOpen->SetOptions(OriginalOption);
			hr = pFileOpen->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						OutFilePath = pszFilePath;
						OutFilePath.Replace(L"\\", L"/");
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
	return hr;
}