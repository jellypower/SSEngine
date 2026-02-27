#pragma once
#include "SRenderComponentBase.h"

class IMaterialAsset;
class IMeshAsset;
class IModelAsset;

class SSCONTENTBASE_MODULE SMeshRenderComponentBase : public SRenderComponentBase
{
protected:
	IModelAsset* _CachedOirignalModelAsset = nullptr;
	IMeshAsset* _CachedMeshAsset = nullptr;
	SS::PooledList<IMaterialAsset*, SS::InlineAllocator<8>> _CachedMtlAssets;

protected:
	void SnycMeshRIWithAssetBindingIfExists();


public:
	SS::SHasherW GetMeshAssetName() const;
	SS::SHasherW GetMtlAssetName(int32 MtlIdx) const;

public:
	void SetModelAsset(SS::SHasherW ModelAssetName);
	void SetMeshAsset(SS::SHasherW MeshAssetName);
	void SetMaterialAsset(SS::SHasherW MtlAssetName, int32 MtlIdx);

	// Model이 인스턴스로 만들어지면 해당 렌더인스턴스는 더 이상 모델 자체에 바인딩 되지 않는다.
	// 해당 모델이 가지고 있는 메시와 메테리얼 정보만 스크랩해가고 원본 모델에 대한 정보는 잊어버린다.
	// 그래서 해당 모델 에셋의 메테리얼이 변경된다면 이는 해당 렌더 인스턴스에 적용되지 않는 사항이다.
	// 만약 모델 에셋의 메테리얼이 변경되고 해당 사항이 반영됐으면 한다면 ApplyModelAssetChange함수를 사용해서
	// 최신 모델 정보로 렌더 인스턴스를 업데이트 해줘야 한다.
	void ApplyModelAssetChange();
};

