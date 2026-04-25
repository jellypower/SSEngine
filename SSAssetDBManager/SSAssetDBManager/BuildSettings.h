#pragma once


#pragma comment(lib, "ss-sqllite.lib")


#ifdef _DEBUG

#pragma comment(lib, "SSEngineDefault_Debug_x64.lib")
#pragma comment(lib, "SSRenderer_Debug_x64.lib")
#pragma comment(lib, "SObject_Debug_x64.lib")

#else

#pragma comment(lib, "SSEngineDefault_Release_x64.lib")
#pragma comment(lib, "SSRenderer_Release_x64.lib")
#pragma comment(lib, "SObject_Release_x64.lib")

#endif





