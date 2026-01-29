#define SSENGINEDEFAULT_MODULE_EXPORT
#include "SSEngineDefault/Public/TestCodes/TestFunctions.h"

#include "TestFunction_Internal.h"
#include "TestSerialize_Internal.h"

void TEST_ExecuteTestFunctions()
{
	ContainerTest_PooledLinkedList();
	ContainerTest_HashMap();
	CustomHeapAllocatorTest();
	SHasherPoolTest();
	SHasherTest();
	StringTest();
	DirectXMathTest();
	StringSerializerTest();
	HasherSerializerTest();
}
