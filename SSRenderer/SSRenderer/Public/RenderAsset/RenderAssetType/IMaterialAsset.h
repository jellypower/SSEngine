#pragma once
#include "SSRenderer/Public/RenderAsset/RenderAssetType/IAssetBase.h"

struct MtlDataBase;
class GALMaterialAssetWrapperBase;




class IMaterialAsset : public IAssetBase
{
public:
	static const EAssetType ThisAssetType = EAssetType::Material;

protected:
	MtlDataBase* _MtlData = nullptr;

	// TODO: 매우 중요!
	// TODO: 매우 중요!
	// TODO: 매우 중요!
	// GALMaterial이랑 해당 GALMaterial이 사용하는 Texture의 라이프 사이클을 정해놔야 한다.
	// 다른 에셋들은 괜찮은데 메테리얼의 경우 RealTime에 수정되는 경우의 수가 있다.
	// 고려해야 할 점은
	// 1. GPU작업이 진행되는 중에 시스템에서 ConstantBuffer나 텍스처의 DescHandle이 갑자기 수정되는 경우 결과가 예상치 못하다.
	// 2. GPU작업이 진행되고 있는 중에 레퍼런싱 하고있는 텍스처를 변경할 경우 이전 텍스처의 레퍼런스 카운트가 내려가서
	//		GPU메모리가 삭제되고 이걸 렌더링하려고 GPU가 접근하다가 뻑이 날 수 있다.
	GALMaterialAssetWrapperBase* _GALMaterialAsset = nullptr;

public:
	const MtlDataBase* GetMtlData() const { return _MtlData; }
	GALMaterialAssetWrapperBase* GetGALMaterialAsset() const { return _GALMaterialAsset; }

	virtual void ReleaseGALData() = 0;
};
