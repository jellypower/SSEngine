#include "pch.h"
#include "SSFBXImporterDEBUGUtils.h"

void PrintFbxNodeInfo(FbxNode* node)
{
	::FbxMesh* fbxMesh = node->GetMesh();
	if (fbxMesh == nullptr)
	{
		return;
	}

	{

		SS_LOG("mesh name: %s\n", node->GetName());
		SS_LOG("\tnode ID: %llu, mesh ID: %llu, uv Cnt: %d, material count: %d\n",
			fbxMesh->GetNode()->GetUniqueID(),
			fbxMesh->GetUniqueID(),
			fbxMesh->GetUVLayerCount(),
			node->GetMaterialCount()
		);

		SS_LOG("\tmateria IDs: ");
		for (uint32 i = 0; i < node->GetMaterialCount(); i++)
		{
			SS_LOG("%llu, ", node->GetMaterial(i)->GetUniqueID());
		}
		SS_LOG("\n");

		if (fbxMesh->GetElementNormal())
		{
			SS_LOG("\tnormal count: %d, ", fbxMesh->GetElementNormal()->GetDirectArray().GetCount());
		}
		if (fbxMesh->GetElementUV())
		{
			SS_LOG("uv count: %d, ", fbxMesh->GetElementUV()->GetDirectArray().GetCount());
		}
		SS_LOG("ctrl count: %d, ", fbxMesh->GetControlPointsCount());
		SS_LOG("\n");

		if (fbxMesh->GetElementMaterial() != nullptr && fbxMesh->GetElementMaterial()->GetMappingMode() == FbxLayerElement::eByPolygon)
		{
			FbxLayerElementArrayTemplate<int>* materialIndices;
			fbxMesh->GetMaterialIndices(&materialIndices);

			SS_LOG("\t(By Polygon) material indice count: %d, polygon count: %d\n", materialIndices->GetCount(), fbxMesh->GetPolygonCount());
		}
		else
		{
			SS_LOG("\t(All Same)\n");
		}
		SS_LOG("\n\n");
	}
}
