#include "SSFBXImporter/Public/ModuleEntry/SSFBXImporterFactory.h"

#include "SSFBXImporter/Private/SSFBXImporter.h"

ISSFBXImporter* CreateSSFBXImporter()
{
	return DBG_NEW SSFBXImporter();
}
