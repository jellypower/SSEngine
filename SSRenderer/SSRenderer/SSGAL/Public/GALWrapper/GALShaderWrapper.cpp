#include "GALShaderWrapper.h"

GALShaderWrapper::GALShaderWrapper(SS::SHasherW InShaderName, const utf16* InShaderPath, const char* entryPoint,
	EShaderType InShaderType)
{
	_ShaderName = InShaderName;
	_ShaderPath = InShaderPath;

	_entryPointName = entryPoint;
	_shaderType = InShaderType;
}
