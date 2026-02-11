#pragma once

enum ESystemPathDialogueOption
{
	SPD_NONE = 0,

	SPD_CREATEPATH = 1 << 1
};


HRESULT OpenSystemPathDialogue(SS::StringW& OutFilePath, ESystemPathDialogueOption Option = SPD_NONE);
