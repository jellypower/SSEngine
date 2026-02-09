#include "SSFBXImporterUtils.h"

#include <SSEngineDefault/Public/RawProfiler/ProfilerUtils.h>
#include <SSRenderer/Public/RenderAsset/RenderAssetType/MtlData/MtlDataDefaultPBR.h>

#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSEngineDefault/Public/SSContainer/ContainerUtil/ContainerUtil.h"
#include "SSFBXImporter/Public/FRAN.h"

#include "SSRenderer/Public/SSRendererGlobalVariableSet.h"
#include "SSRenderer/Public/RenderAsset/Mutable/RenderAssetType/IMeshAssetMutable.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshRawDataSkinned.h"
#include "SSRenderer/Public/RenderBase/IRenderer.h"
#include "SSRenderer/Public/RenderCommon/SSVertexType.h"

int32 SSFBXImporterUtils::CalcWholeNodeCnt_Recursion(const FbxNode* node)
{
	int32 sum = 1;
	int32 childCnt = node->GetChildCount();

	for (int32 i = 0; i < childCnt; i++)
	{
		sum += CalcWholeNodeCnt_Recursion(node->GetChild(i));
	}

	return sum;
}

Transform SSFBXImporterUtils::ExtractTransformFromNode(FbxNode* node, FbxTime fbxTime)
{
	//	FbxAMatrix fbxMat;
	//	fbxMat.SetIdentity();
	//	fbxMat.SetT(node->GetGeometricTranslation(FbxNode::eSourcePivot));
	//	fbxMat.SetR(node->GetGeometricRotation(FbxNode::eSourcePivot));
	//	fbxMat.SetS(node->GetGeometricScaling(FbxNode::eSourcePivot));


	const FbxAMatrix& fbxMat = node->EvaluateLocalTransform(fbxTime);
	//	const FbxAMatrix& AnimTransform = node->EvaluateLocalTransform(fbxTime);
	//	fbxMat = AnimTransform * fbxMat;

	Transform transform;

	const FbxDouble3 fbxTranslate = fbxMat.GetT();
	transform.Position.X = -fbxTranslate.mData[0] * 0.01;
	transform.Position.Y = fbxTranslate.mData[1] * 0.01;
	transform.Position.Z = fbxTranslate.mData[2] * 0.01;
	transform.Position.W = 1;

	const FbxDouble3 fbxScale = fbxMat.GetS();
	transform.Scale.X = fbxScale.mData[0];
	transform.Scale.Y = fbxScale.mData[1];
	transform.Scale.Z = fbxScale.mData[2];
	transform.Scale.W = 0;

	const FbxQuaternion fbxRotation = fbxMat.GetQ(); // pitch yaw roll 으로 바꿔줘야 함
	transform.Rotation.X = -fbxRotation.mData[0];
	transform.Rotation.Y = fbxRotation.mData[1];
	transform.Rotation.Z = fbxRotation.mData[2];
	transform.Rotation.W = -fbxRotation.mData[3];

	//	transform.Rotation = Quaternion::RotateAxisAngle(transform.Rotation, Vector4f::Right, SS::DegToRadians(fbxRotation.mData[1]));
	//	transform.Rotation = Quaternion::RotateAxisAngle(transform.Rotation, Vector4f::Forward, -SS::DegToRadians(fbxRotation.mData[2]));
	//	transform.Rotation = Quaternion::RotateAxisAngle(transform.Rotation, Vector4f::Up, SS::DegToRadians(fbxRotation.mData[0]));


	return transform;

}

Transform SSFBXImporterUtils::ExtractBoneRootRelativeTransform(FbxNode* InNode, FbxTime fbxTime)
{
	Transform Result;

	FbxNodeAttribute::EType nodeAttribute = InNode->GetNodeAttribute()->GetAttributeType();
	if (nodeAttribute != FbxNodeAttribute::eSkeleton)
	{
		SS_ASSERT(false);
		return Result;
	}


	FbxNode* NodeItem = InNode;
	Result = ExtractTransformFromNode(NodeItem, fbxTime);

	while (NodeItem->GetSkeleton()->IsSkeletonRoot() == false)
	{
		NodeItem = NodeItem->GetParent();

		Transform TransformItem = ExtractTransformFromNode(NodeItem, fbxTime);
		Result = Result * TransformItem;
	}

	return Result;
}

constexpr float POS_SQR_THRESHOLD = 0.0001;
constexpr float DEG_COS_THRESHOLD = 0.001;
constexpr float UV_DIST_THRESHOlD = 0.0001;
constexpr float FLOAT_ZERO_THRESHOLD = 0.0001;
bool AreSimilarVertex(const SSDefaultVertex& lhs, const SSDefaultVertex& rhs)
{
	if (SS::SqrDistance(lhs.Pos, rhs.Pos) > POS_SQR_THRESHOLD)
		return false;

	if (SS::SqrDistance(rhs.Normal, lhs.Normal) > FLOAT_ZERO_THRESHOLD &&
		SS::abs(1 - SS::Dot3D(lhs.Normal, rhs.Normal)) > DEG_COS_THRESHOLD)
		return false;

	if (SS::SqrDistance(rhs.Tangent, lhs.Tangent) > FLOAT_ZERO_THRESHOLD &&
		SS::abs(1 - SS::Dot3D(lhs.Tangent, rhs.Tangent)) > DEG_COS_THRESHOLD)
		return false;

	if (SS::SqrDistance(lhs.Uv[0], rhs.Uv[0]) > UV_DIST_THRESHOlD)
		return false;

	if (SS::SqrDistance(lhs.Uv[1], rhs.Uv[1]) > UV_DIST_THRESHOlD)
		return false;

	return true;
}

SSDefaultVertex ExtractVertex(::FbxMesh* fbxMesh, uint32 polygonIdx, uint32 positionInPolygon, uint32& outControlPointIdx)
{
	SSDefaultVertex outVertex;
	outControlPointIdx = fbxMesh->GetPolygonVertex(polygonIdx, positionInPolygon);


	// Position
	FbxVector4 controlPointPosition = fbxMesh->GetControlPointAt(outControlPointIdx);
	outVertex.Pos.X = -controlPointPosition.mData[0] * 0.01;
	outVertex.Pos.Y = controlPointPosition.mData[1] * 0.01;
	outVertex.Pos.Z = controlPointPosition.mData[2] * 0.01;
	outVertex.Pos.W = 1;


	// Normal
	const FbxGeometryElementNormal* const fbxNormal = fbxMesh->GetElementNormal();
	FbxVector4 normalVector;
	int32 fbxNormalIdx;



	FbxLayerElement::EMappingMode NormalMappingMode = fbxNormal->GetMappingMode();
	if (NormalMappingMode == FbxLayerElement::eByPolygonVertex)
	{
		fbxNormalIdx = fbxMesh->GetPolygonVertexIndex(polygonIdx) + positionInPolygon;
	}
	else
	{
		fbxNormalIdx = fbxMesh->GetPolygonVertex(polygonIdx, positionInPolygon);
	}

	bool bResult;

	FbxLayerElement::EReferenceMode ReferenceMode = fbxNormal->GetReferenceMode();
	switch (ReferenceMode)
	{
	case FbxLayerElement::eDirect:

		normalVector = fbxNormal->GetDirectArray().GetAt(fbxNormalIdx);

		break;
	case FbxLayerElement::eIndex:
	case FbxLayerElement::eIndexToDirect:

		bResult = fbxMesh->GetPolygonVertexNormal(polygonIdx, positionInPolygon, normalVector);
		SS_ASSERT(bResult);

		//		fbxNormalIdx = fbxNormal->GetIndexArray().GetAt(outControlPointIdx); // TODO: 여기가 문제다
		//		normalVector = fbxNormal->GetDirectArray().GetAt(fbxNormalIdx);

		break;
	default:
		SS_ASSERT_MSG(false, L"Invalid Fbxformat. function just return");
	}

	normalVector.Normalize();
	outVertex.Normal.X = -normalVector.mData[0];
	outVertex.Normal.Y = normalVector.mData[1];
	outVertex.Normal.Z = normalVector.mData[2];
	outVertex.Normal.W = 0;


	// UV
	uint32 uvChannelCnt = fbxMesh->GetUVLayerCount();
	if (uvChannelCnt > DEFAULT_VERTEX_UV_TYPE_CNT_MAX)
	{
		// SS_ASSERT_MSG(false, L"Too many uv channel");
		uvChannelCnt = DEFAULT_VERTEX_UV_TYPE_CNT_MAX;
	}

	for (uint32 i = 0; i < uvChannelCnt; i++)
	{
		FbxVector2 uvVector;
		uint32 uvIdx;
		uint32 polygonVertexIdx;
		uint32 directIdx;
		const FbxGeometryElementUV* fbxUV = fbxMesh->GetElementUV(i);
		const char* UVName = fbxUV->GetName();

		bool bUnmapped = false;

		switch (fbxUV->GetMappingMode())
		{
		case FbxLayerElement::eByControlPoint:

			switch (fbxUV->GetReferenceMode())
			{
			case FbxLayerElement::eDirect:

				uvVector = fbxUV->GetDirectArray().GetAt(outControlPointIdx);

				break;
			case FbxLayerElement::eIndex:
			case FbxLayerElement::eIndexToDirect:

				fbxMesh->GetPolygonVertexUV(polygonIdx, positionInPolygon, UVName, uvVector, bUnmapped);
				SS_ASSERT(bResult && bUnmapped == false);

				//				uvIdx = fbxUV->GetIndexArray().GetAt(outControlPointIdx);
				//				uvVector = fbxUV->GetDirectArray().GetAt(uvIdx);

				break;
			default:
				SS_ASSERT_MSG(false, L"Invalid Fbxformat. function just return");
				break;
			}

			break;


		case FbxLayerElement::eByPolygonVertex:
			// uv같은 경우에는 특이하게 eByPolygonVertex일 때 
			// reference mode를 무시하고 GetTextureUVIndex를 통해 바로 directarray의 index를 얻어와야 한다.

			uvIdx = fbxMesh->GetTextureUVIndex(polygonIdx, positionInPolygon);
			uvVector = fbxUV->GetDirectArray().GetAt(uvIdx);

			break;
		}

		outVertex.Uv[i].X = uvVector.mData[0];
		outVertex.Uv[i].Y = 1 - uvVector.mData[1];

		break;
	}



	// Tangent
	FbxGeometryElementTangent* fbxTangent = fbxMesh->GetElementTangent();
	FbxVector4 tanVector;
	if (fbxTangent != nullptr) {

		switch (fbxTangent->GetReferenceMode())
		{
		case FbxLayerElement::eDirect:

			tanVector = fbxTangent->GetDirectArray().GetAt(outControlPointIdx);

			break;
		case FbxLayerElement::eIndex:
		case FbxLayerElement::eIndexToDirect:

			int32 tangentIdx = fbxTangent->GetIndexArray().GetAt(outControlPointIdx);
			tanVector = fbxTangent->GetDirectArray().GetAt(tangentIdx);

			break;
		}

		outVertex.Tangent.X = -tanVector.mData[0];
		outVertex.Tangent.Y = tanVector.mData[1];
		outVertex.Tangent.Z = tanVector.mData[2];
		outVertex.Tangent.W = tanVector.mData[3];
	}
	else
	{
		outVertex.Tangent.X = 0.0f;
		outVertex.Tangent.Y = 0.0f;
		outVertex.Tangent.Z = 0.0f;
		outVertex.Tangent.W = 0.0f;
	}

	return outVertex;
}

SSSkinnedVertex ExtractSkinnedVertexWithoutSkinData(FbxMesh* fbxMesh, uint32 polygonIdx, uint32 positionInPolygon, uint32& outControlPointIdx)
{
	SSDefaultVertex DefaultVertex = ExtractVertex(fbxMesh, polygonIdx, positionInPolygon, outControlPointIdx);
	SSSkinnedVertex outVertex;
	outVertex.Pos = DefaultVertex.Pos;
	outVertex.Normal = DefaultVertex.Normal;
	outVertex.Tangent = DefaultVertex.Tangent;
	outVertex.Uv[0] = DefaultVertex.Uv[0];
	outVertex.Uv[1] = DefaultVertex.Uv[1];


	// Skinning
	for (int32 i = 0; i < VERTEX_SKINNING_BONE_COUNT_MAX; i++)
	{
		outVertex.BoneIdx[i] = SS_UINT32_MAX;
		outVertex.Weight[i] = 0;
	}

	return outVertex;
}

IMeshAsset* SSFBXImporterUtils::GenerateNewMeshAssestFromFbxMesh(FbxMesh* fbxMesh, SS::SHasherW NewAssetName, const utf16* InAssetPath)
{
	static const SS::SHasherW HASHER_FBX_IMPORT = FRAN::NS_FBX_IMPORT;

	SS_ASSERT(fbxMesh != nullptr);

	IAssetManagerMutable* AssetManager = g_Renderer->GetMutableAssetManager();


	IMeshAssetMutable* NewMeshAsset = AssetManager->CreateEmptyMeshAsset(HASHER_FBX_IMPORT, NewAssetName, InAssetPath);
	MeshRawDataDefault* NewMeshRawData = DBG_NEW MeshRawDataDefault();

	// - Load num
	const uint32 layerNum = fbxMesh->GetLayerCount();
	const uint32 ControlPointCnt = fbxMesh->GetControlPointsCount();
	const uint32 PolygonCount = fbxMesh->GetPolygonCount();
	const uint32 PolygonVertexNum = fbxMesh->GetPolygonVertexCount();
	const FbxGeometryElementNormal* const FbxNormal = fbxMesh->GetElementNormal();
	SS_ASSERT(FbxNormal != nullptr, "normal must be exists.");


	// ControlPointToSSIdxMap[ControlPointIdx][배열에 들어온대로의 순서] = SSVertexBuffer의Idx
	// i번째 ControlPoint에 해당되는(물리적 위치가 같은) SSVertexBufferIdx의 리스트를 들고있음
	SS::PooledList<SS::PooledList<uint32>> ControlPointToSSIdxMap(ControlPointCnt);
	ControlPointToSSIdxMap.Resize(ControlPointCnt);
	for (SS::PooledList<uint32>& item : ControlPointToSSIdxMap)
	{
		constexpr uint32 PLENTY_VALUE_FOR_EACH_IDX_MAP = 10;
		item.Reserve(PLENTY_VALUE_FOR_EACH_IDX_MAP);
	}



	uint32 uvChannelCnt = fbxMesh->GetUVLayerCount();
	const FbxGeometryElementUV* fbxUV[DEFAULT_VERTEX_UV_TYPE_CNT_MAX];
	if (uvChannelCnt > DEFAULT_VERTEX_UV_TYPE_CNT_MAX)
	{
		// SS_ASSERT_MSG(false, L"Too many uv channel");
		uvChannelCnt = DEFAULT_VERTEX_UV_TYPE_CNT_MAX;
	}
	for (uint32 i = 0; i < uvChannelCnt; i++)
	{
		fbxUV[i] = fbxMesh->GetElementUV(i);
		SS_ASSERT_MSG(fbxUV[i] != nullptr, L"uv must be exists of idx %d", i);
	}


	SS::PooledList<SSDefaultVertex> ssVertexBuffer(ControlPointCnt * 2);

	// PolygonVertexToCtrlPointMap[PolygonIdx][PolygonVertexIdx] = <FBX파일의 ControlPoint의 Idx, ControlPointToSSIdxMap의 Idx>
	// i번째 Polygon에 해당되는 FBXControlPointIdx와 SSVertexBufferIdx의 리스트를 담고있음
	SS::PooledList<SS::PooledList<SS::pair<uint32, int32>>> PolygonVertexToCtrlPointMap;
	PolygonVertexToCtrlPointMap.Resize(PolygonCount);


	// - Build SSVertex, FbxVertex related Map
	for (uint32 i = 0; i < PolygonCount; i++)
	{
		uint32 PolygonVertexCount = fbxMesh->GetPolygonSize(i);
		PolygonVertexToCtrlPointMap[i].Reserve(PolygonVertexCount);
		for (uint32 j = 0; j < PolygonVertexCount; j++)
		{
			uint32 fbxFileControlPointIdx = -1;
			SSDefaultVertex extractedVertex = ExtractVertex(fbxMesh, i, j, fbxFileControlPointIdx);
			SS_ASSERT(fbxFileControlPointIdx != -1, "Invalid ControlPoint");

			int32 ssVertexBufferIdx = -1;
			for (uint32 k = 0; k < ControlPointToSSIdxMap[fbxFileControlPointIdx].GetSize(); k++)
			{
				uint32 ssIdx = ControlPointToSSIdxMap[fbxFileControlPointIdx][k];
				if (AreSimilarVertex(ssVertexBuffer[ssIdx], extractedVertex))
				{
					ssVertexBufferIdx = k;
					break;
				}
			}

			if (ssVertexBufferIdx == -1)
			{
				// 아래 코드대로 대입되면 PolygonVertexToCtrlPointMap[PolygonIdx][PolygonVertexIdx] = <FBX파일의 ControlPoint의 Idx, ControlPointToSSIdxMap의 Idx>가 됨.
				PolygonVertexToCtrlPointMap[i].PushBack({ fbxFileControlPointIdx, (int32)ControlPointToSSIdxMap[fbxFileControlPointIdx].GetSize() });

				// ControlPointToSSIdxMap[ControlPointIdx][배열에 들어온대로의 순서] = SSVertexBuffer의Idx
				ControlPointToSSIdxMap[fbxFileControlPointIdx].PushBack(ssVertexBuffer.GetSize());

				// SimilarVertex가 없을땐 FbxMesh의 PolygonVeretex를 하나씩 돌면서 SSVertex 버퍼에 값을 차곡차곡 채워넣음
				ssVertexBuffer.PushBack(extractedVertex);
			}
			else
			{
				// SimilarVertex가 있으면 해당 FBX Vertex의 
				PolygonVertexToCtrlPointMap[i].PushBack({ fbxFileControlPointIdx, ssVertexBufferIdx });
			}
		}
	}


	// - alloc vertex memory
	NewMeshRawData->_VertexHeader.vertexCnt = ssVertexBuffer.GetSize();
	NewMeshRawData->_VertexHeader.MeshType = EMeshType::Rigid;
	uint32 validVertexBufferSize = EachVertexSizeOfType(EMeshType::Rigid) * NewMeshRawData->_VertexHeader.vertexCnt;

	NewMeshRawData->_vertexData = DBG_MALLOC(validVertexBufferSize);
	SSDefaultVertex* ssVertex = (SSDefaultVertex*)NewMeshRawData->_vertexData;

	// - copy to real time vertex buffer
	memcpy_s(ssVertex, validVertexBufferSize, ssVertexBuffer.GetData(), validVertexBufferSize);


	// - alloc index memory
	if (fbxMesh->GetNode()->GetMaterial(0) != nullptr)
		NewMeshRawData->_VertexHeader.subMeshCnt = fbxMesh->GetNode()->GetMaterialCount();
	else
		NewMeshRawData->_VertexHeader.subMeshCnt = 1;
	SS_ASSERT(NewMeshRawData->_VertexHeader.subMeshCnt < SUBMESH_COUNT_MAX);

	FbxGeometryElementMaterial* fbxElementMaterial = fbxMesh->GetElementMaterial();

	FbxLayerElementArrayTemplate<int>* materialIndices = nullptr;

	// https://blog.naver.com/PostView.naver?blogId=lifeisforu&logNo=80105592736
	// FBX SDK 공식 문서에서도 찾아볼 수 있는데 보통 eByPolygon과 eAllSame만이 Material을 위한 index로 사용된다.
	// 즉, 특정 fbxMesh에서 Polygon마다 다른 Material이 할당될 수 있다는 것은 다른 Material을 사용하는 SubGeometry가 필요할 수 있다는 것이다.
	if (fbxMesh->GetElementMaterial() != nullptr && fbxMesh->GetElementMaterial()->GetMappingMode() == FbxLayerElement::eByPolygon)
	{
		fbxMesh->GetMaterialIndices(&materialIndices);

		for (uint32 i = 0; i < PolygonCount; i++) // 메테리얼이 1개 이상이니까 SubGeomtry별로 나눔
		{
			uint8 matIdx = materialIndices->GetAt(i);
			NewMeshRawData->_VertexHeader.indexDataCnt[matIdx] += ((fbxMesh->GetPolygonSize(i) - 2) * 3);
		}
	}
	else
	{
		for (uint32 i = 0; i < PolygonCount; i++) // 메테리얼 1개 고정이니까 그냥 다 더함
		{
			NewMeshRawData->_VertexHeader.indexDataCnt[0] += (fbxMesh->GetPolygonSize(i) - 2);
		}
		NewMeshRawData->_VertexHeader.indexDataCnt[0] *= 3;
	}

	uint32 idxAcc = 0;
	for (uint32 i = 0; i < NewMeshRawData->_VertexHeader.subMeshCnt; i++)
	{
		NewMeshRawData->_VertexHeader.indexDataStartIndex[i] = idxAcc;
		idxAcc += NewMeshRawData->_VertexHeader.indexDataCnt[i];
	}
	NewMeshRawData->_VertexHeader.wholeIndexDataCnt = idxAcc;
	NewMeshRawData->_indexData = (uint32*)DBG_MALLOC(sizeof(uint32) * NewMeshRawData->_VertexHeader.wholeIndexDataCnt);


	// 5. load index memory
	uint32 subMaterialIdxDataCounter[SUBMESH_COUNT_MAX] = { 0, };

	if (fbxMesh->GetElementMaterial() != nullptr && fbxMesh->GetElementMaterial()->GetMappingMode() == FbxLayerElement::eByPolygon) // 만약 머티리얼이 여러개면
	{
		for (uint32 i = 0; i < PolygonCount; i++)
		{
			uint32 PolygonVertexCount = fbxMesh->GetPolygonSize(i);
			uint32 matIdx = materialIndices->GetAt(i);
			uint32 idxDataStart = NewMeshRawData->_VertexHeader.indexDataStartIndex[matIdx];
			for (uint32 j = 1; j < PolygonVertexCount - 1; j++)
			{
				SS::pair<uint32, int32> CtrlPointIdx = PolygonVertexToCtrlPointMap[i][j + 1];
				uint32 ssVertexBufferIdx = ControlPointToSSIdxMap[CtrlPointIdx.first][CtrlPointIdx.second];
				NewMeshRawData->_indexData[idxDataStart + subMaterialIdxDataCounter[matIdx]] = ssVertexBufferIdx;

				CtrlPointIdx = PolygonVertexToCtrlPointMap[i][j];
				ssVertexBufferIdx = ControlPointToSSIdxMap[CtrlPointIdx.first][CtrlPointIdx.second];
				NewMeshRawData->_indexData[idxDataStart + subMaterialIdxDataCounter[matIdx] + 1] = ssVertexBufferIdx;

				CtrlPointIdx = PolygonVertexToCtrlPointMap[i][0];
				ssVertexBufferIdx = ControlPointToSSIdxMap[CtrlPointIdx.first][CtrlPointIdx.second];
				NewMeshRawData->_indexData[idxDataStart + subMaterialIdxDataCounter[matIdx] + 2] = ssVertexBufferIdx;

				subMaterialIdxDataCounter[matIdx] += 3;
			}
		}
	}
	else // 만약 머티리얼이 하나면
	{
		for (uint32 i = 0; i < fbxMesh->GetPolygonCount(); i++)
		{
			uint32 thisPolygonSize = fbxMesh->GetPolygonSize(i);
			for (uint32 j = 1; j < thisPolygonSize - 1; j++)
			{
				SS::pair<uint32, int32> CtrlPointIdx = PolygonVertexToCtrlPointMap[i][j + 1];
				uint32 ssVertexBufferIdx = ControlPointToSSIdxMap[CtrlPointIdx.first][CtrlPointIdx.second];
				NewMeshRawData->_indexData[subMaterialIdxDataCounter[0]] = ssVertexBufferIdx;

				CtrlPointIdx = PolygonVertexToCtrlPointMap[i][j];
				ssVertexBufferIdx = ControlPointToSSIdxMap[CtrlPointIdx.first][CtrlPointIdx.second];
				NewMeshRawData->_indexData[subMaterialIdxDataCounter[0] + 1] = ssVertexBufferIdx;

				CtrlPointIdx = PolygonVertexToCtrlPointMap[i][0];
				ssVertexBufferIdx = ControlPointToSSIdxMap[CtrlPointIdx.first][CtrlPointIdx.second];
				NewMeshRawData->_indexData[subMaterialIdxDataCounter[0] + 2] = ssVertexBufferIdx;

				subMaterialIdxDataCounter[0] += 3;
			}
		}
	}

	for (uint32 i = 0; i < NewMeshRawData->_VertexHeader.subMeshCnt; i++)
		SS_ASSERT(NewMeshRawData->_VertexHeader.indexDataCnt[i] == subMaterialIdxDataCounter[i]);



	// Calculate tangent if there is no tangent data on FbxFormat
	FbxGeometryElementTangent* fbxTangent = fbxMesh->GetElementTangent();
	if (fbxTangent == nullptr)
	{
		for (uint32 subGeomIdx = 0; subGeomIdx < NewMeshRawData->_VertexHeader.subMeshCnt; subGeomIdx++)
		{
			uint32* thisIdxData = NewMeshRawData->_indexData + NewMeshRawData->_VertexHeader.indexDataStartIndex[subGeomIdx];
			int32 thisIdxDataNum = NewMeshRawData->_VertexHeader.indexDataCnt[subGeomIdx];
			SS_ASSERT(thisIdxDataNum % 3 == 0);

			for (uint32 i = 0; i < thisIdxDataNum; i += 3)
			{
				SSDefaultVertex& v0 = ssVertex[thisIdxData[i]];
				SSDefaultVertex& v1 = ssVertex[thisIdxData[i + 1]];
				SSDefaultVertex& v2 = ssVertex[thisIdxData[i + 2]];

				Vector4f dv1 = v1.Pos - v0.Pos;
				Vector4f dv2 = v2.Pos - v0.Pos;

				Vector2f duv1 = v1.Uv[0] - v0.Uv[0];
				Vector2f duv2 = v2.Uv[0] - v0.Uv[0];

				constexpr float EPSILON = 0.0001;
				if (duv1.X * duv2.Y - duv1.Y * duv2.X > EPSILON)
				{
					float detInverse = 1.0f / (duv1.X * duv2.Y - duv1.Y * duv2.X);
					Vector4f tangent = (dv1 * duv2.Y - dv2 * duv1.Y) * detInverse;


					v2.Tangent = v1.Tangent = v0.Tangent = tangent;
				}
				else
				{
					Vector4f tangent = Vector4f(1, 0, 0, 0);
					v2.Tangent = v1.Tangent = v0.Tangent = tangent;
				}


			}
		}
	}

	NewMeshAsset->InjectRawDataXXX(NewMeshRawData);
	return NewMeshAsset;
}

IMeshAsset* SSFBXImporterUtils::GenerateNewSkinnedMeshAssestFromFbxMesh(
	FbxMesh* fbxMesh,
	SS::SHasherW NewAssetName,
	const utf16* InAssetPath)
{
	static const SS::SHasherW HASHER_FBX_IMPORT = FRAN::NS_FBX_IMPORT;

	if (fbxMesh == nullptr)
	{
		SS_INTERRUPT();
		return nullptr;
	}

	const utf16* AssetStr = NewAssetName.C_Str();


	IAssetManagerMutable* AssetManager = g_Renderer->GetMutableAssetManager();
	IMeshAssetMutable* NewMeshAsset = AssetManager->CreateEmptyMeshAsset(HASHER_FBX_IMPORT, NewAssetName, InAssetPath);
	MeshRawDataSkinned* NewSkinnedMeshRawData = DBG_NEW MeshRawDataSkinned();

	// 1. Load num
	const uint32 layerNum = fbxMesh->GetLayerCount();
	const uint32 ControlPointCnt = fbxMesh->GetControlPointsCount();
	const uint32 PolygonCount = fbxMesh->GetPolygonCount();
	const uint32 PolygonVertexCnt = fbxMesh->GetPolygonVertexCount(); // sum of vertex in each polygon
	const FbxGeometryElementNormal* const FbxNormal = fbxMesh->GetElementNormal();
	SS_ASSERT(FbxNormal != nullptr, "normal must be exists.");


	// ControlPointToSSIdxMap[ControlPointIdx][배열에 들어온대로의 순서] = SSVertexBuffer의Idx
	// i번째 ControlPoint에 해당되는(물리적 위치가 같은) SSVertexBufferIdx의 리스트를 들고있음
	SS::PooledList<SS::PooledList<uint32>> ControlPointToSSIdxMap(ControlPointCnt);
	ControlPointToSSIdxMap.Resize(ControlPointCnt);
	for (SS::PooledList<uint32>& item : ControlPointToSSIdxMap)
	{
		constexpr uint32 PLENTY_VALUE_FOR_EACH_IDX_MAP = 10;
		item.Reserve(PLENTY_VALUE_FOR_EACH_IDX_MAP);
	}

	// PolygonVertexToCtrlPointMap[PolygonIdx][PolygonVertexIdx] = <FBX파일의 ControlPoint의 Idx, ControlPointToSSIdxMap의 Idx>
	// i번째 Polygon에 해당되는 FBXControlPointIdx와 SSVertexBufferIdx의 리스트를 담고있음

	uint32 uvChannelCnt = fbxMesh->GetUVLayerCount();
	FbxGeometryElementUV* fbxUV[DEFAULT_VERTEX_UV_TYPE_CNT_MAX];
	if (uvChannelCnt > DEFAULT_VERTEX_UV_TYPE_CNT_MAX)
	{
		// SS_ASSERT_MSG(false, L"Too many uv channel");
		uvChannelCnt = DEFAULT_VERTEX_UV_TYPE_CNT_MAX;
	}
	for (uint32 i = 0; i < uvChannelCnt; i++)
	{
		fbxUV[i] = fbxMesh->GetElementUV(i);
		SS_ASSERT(fbxUV[i] != nullptr, "uv must be exists of idx %d", i);
	}


	SS::PooledList<SSSkinnedVertex> ssVertexBuffer(ControlPointCnt * 2);

	// PolygonVertexToCtrlPointMap[PolygonIdx][PolygonVertexIdx] = <FBX파일의 ControlPoint의 Idx, ControlPointToSSIdxMap의 Idx>
	// i번째 Polygon에 해당되는 FBXControlPointIdx와 SSVertexBufferIdx의 리스트를 담고있음
	SS::PooledList<SS::PooledList<SS::pair<uint32, int32>>> PolygonVertexToCtrlPointMap;
	PolygonVertexToCtrlPointMap.Resize(PolygonCount);

	for (uint32 i = 0; i < PolygonCount; i++)
	{
		uint32 PolygonVertexCount = fbxMesh->GetPolygonSize(i);
		PolygonVertexToCtrlPointMap[i].Reserve(PolygonVertexCount);
		for (uint32 j = 0; j < PolygonVertexCount; j++)
		{
			uint32 ControlPointIdx;
			SSSkinnedVertex extractedVertex = ExtractSkinnedVertexWithoutSkinData(fbxMesh, i, j, ControlPointIdx);
			SS_ASSERT(ControlPointIdx != -1, "Invalid ControlPoint");

			int32 ctrlPointListIdx = -1;
			for (uint32 k = 0; k < ControlPointToSSIdxMap[ControlPointIdx].GetSize(); k++)
			{
				uint32 ssIdx = ControlPointToSSIdxMap[ControlPointIdx][k];
				if (AreSimilarVertex(ssVertexBuffer[ssIdx], extractedVertex))
				{
					ctrlPointListIdx = k;
					break;
				}
			}

			if (ctrlPointListIdx == -1)
			{
				// 아래 코드대로 대입되면 PolygonVertexToCtrlPointMap[PolygonIdx][PolygonVertexIdx] = SSVertexBuffer의 Idx가 됨.
				PolygonVertexToCtrlPointMap[i].PushBack({ ControlPointIdx, (int32)ControlPointToSSIdxMap[ControlPointIdx].GetSize() });

				// ControlPointToSSIdxMap[ControlPointIdx] = SSVertexBuffer의Idx
				ControlPointToSSIdxMap[ControlPointIdx].PushBack(ssVertexBuffer.GetSize());

				// SimilarVertex가 없을땐 FbxMesh의 PolygonVeretex를 하나씩 돌면서 SSVertex 버퍼에 값을 차곡차곡 채워넣음
				ssVertexBuffer.PushBack(extractedVertex);
			}
			else
			{
				// SimilarVertex가 있으면 해당 FBX Vertex의 
				PolygonVertexToCtrlPointMap[i].PushBack({ ControlPointIdx, ctrlPointListIdx });
			}
		}
	}


	uint32 ssVertexCnt = 0;
	for (uint32 i = 0; i < ControlPointToSSIdxMap.GetSize(); i++)
	{
		ssVertexCnt += ControlPointToSSIdxMap[i].GetSize();
	}


	// Load Skinning Data
	SS::PooledList<uint8> boneCntArr;
	boneCntArr.Resize(ControlPointCnt);
	for (uint32 i = 0; i < ControlPointCnt; i++)
	{
		boneCntArr[i] = 0;
	}

	uint32 deformerCnt = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	if (deformerCnt > 1)
	{
		SS_ASSERT(false);
		deformerCnt = 1;
	}

	FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(0, FbxDeformer::eSkin));
	SS_ASSERT(fbxSkin != nullptr);

	ExtractOriginalBoneFromFbxSkin(NewAssetName, NewSkinnedMeshRawData, fbxSkin);

	uint32 ClusterCnt = fbxSkin->GetClusterCount();
	for (int32 BoneIdx = 0; BoneIdx < ClusterCnt; BoneIdx++)
	{
		FbxCluster* CurCluster = fbxSkin->GetCluster(BoneIdx);

		uint32 clusterIndicesCnt = CurCluster->GetControlPointIndicesCount();
		int* curClusterCtlrPointIndices = CurCluster->GetControlPointIndices();
		double* curClusterCtrlPointWeights = CurCluster->GetControlPointWeights();

		for (uint32 j = 0; j < clusterIndicesCnt; j++)
		{
			int ctrlPointIdx = curClusterCtlrPointIndices[j];
			double ctrlPointWeight = curClusterCtrlPointWeights[j];
			uint32 boneCnt = boneCntArr[ctrlPointIdx];

			if (boneCnt >= VERTEX_SKINNING_BONE_COUNT_MAX)
			{
				SS_INTERRUPT("Too many bones for a vertex");
				return nullptr;
			}

			constexpr float SKIN_WEIGHT_THRESHOLD = 0.03;
			if (ctrlPointWeight <= SKIN_WEIGHT_THRESHOLD) // 만약에 바인딩하려는 본의 Weight가 너무 작으면 패스한다
			{
				for (uint32 ssIdx : ControlPointToSSIdxMap[ctrlPointIdx])
				{
					if (boneCnt == 0) // 처음 마주하는 bone이면 0번 인덱스에 해당 가중치를 더해준다.
					{
						ssVertexBuffer[ssIdx].Weight[0] += ctrlPointWeight;
					}
					else
					{
						ssVertexBuffer[ssIdx].Weight[boneCnt - 1] += ctrlPointWeight;
					}
				}
				continue; // 그리고 다음 본으로 스킵
			}

			for (uint32 ssIdx : ControlPointToSSIdxMap[ctrlPointIdx])
			{
				ssVertexBuffer[ssIdx].BoneIdx[boneCnt] = BoneIdx;
				ssVertexBuffer[ssIdx].Weight[boneCnt] += ctrlPointWeight;
			}
			boneCntArr[ctrlPointIdx]++;

		}
	}

	// ================================================================


	// 2. alloc vertex memory
	NewSkinnedMeshRawData->_VertexHeader.vertexCnt = ssVertexBuffer.GetSize();
	NewSkinnedMeshRawData->_VertexHeader.MeshType = EMeshType::Skinned;
	uint32 validVertexBufferSize = EachVertexSizeOfType(EMeshType::Skinned) * NewSkinnedMeshRawData->_VertexHeader.vertexCnt;
	NewSkinnedMeshRawData->_vertexData = DBG_MALLOC(validVertexBufferSize);
	SSSkinnedVertex* ssSkinnedVertex = (SSSkinnedVertex*)NewSkinnedMeshRawData->_vertexData;

	// 3. copy to real time vertex buffer
	memcpy_s(ssSkinnedVertex, validVertexBufferSize, ssVertexBuffer.GetData(), validVertexBufferSize);


	// 4. alloc index memory
	if (fbxMesh->GetNode()->GetMaterial(0) != nullptr)
	{
		NewSkinnedMeshRawData->_VertexHeader.subMeshCnt = fbxMesh->GetNode()->GetMaterialCount();
	}
	else
	{
		NewSkinnedMeshRawData->_VertexHeader.subMeshCnt = 1;
	}

	if (NewSkinnedMeshRawData->_VertexHeader.subMeshCnt > SUBMESH_COUNT_MAX)
	{
		SS_ASSERT(false);
		NewSkinnedMeshRawData->_VertexHeader.subMeshCnt = SUBMESH_COUNT_MAX;
	}


	FbxLayerElementArrayTemplate<int>* materialIndices = nullptr;
	if (fbxMesh->GetElementMaterial() != nullptr && fbxMesh->GetElementMaterial()->GetMappingMode() == FbxLayerElement::eByPolygon)
	{
		fbxMesh->GetMaterialIndices(&materialIndices);

		for (uint32 i = 0; i < PolygonCount; i++)
		{
			uint8 matIdx = materialIndices->GetAt(i);
			NewSkinnedMeshRawData->_VertexHeader.indexDataCnt[matIdx] += ((fbxMesh->GetPolygonSize(i) - 2) * 3);
		}
	}
	else
	{
		for (uint32 i = 0; i < PolygonCount; i++)
		{
			NewSkinnedMeshRawData->_VertexHeader.indexDataCnt[0] += (fbxMesh->GetPolygonSize(i) - 2);
		}
		NewSkinnedMeshRawData->_VertexHeader.indexDataCnt[0] *= 3;
	}

	uint32 idxAcc = 0;
	for (uint8 i = 0; i < NewSkinnedMeshRawData->_VertexHeader.subMeshCnt; i++)
	{
		NewSkinnedMeshRawData->_VertexHeader.indexDataStartIndex[i] = idxAcc;
		idxAcc += NewSkinnedMeshRawData->_VertexHeader.indexDataCnt[i];
	}
	NewSkinnedMeshRawData->_VertexHeader.wholeIndexDataCnt = idxAcc;
	NewSkinnedMeshRawData->_indexData = (uint32*)DBG_MALLOC(sizeof(uint32) * NewSkinnedMeshRawData->_VertexHeader.wholeIndexDataCnt);


	// 5. load index memory
	uint32 subMaterialIdxDataCounter[SUBMESH_COUNT_MAX] = { 0, };

	if (fbxMesh->GetElementMaterial() != nullptr && fbxMesh->GetElementMaterial()->GetMappingMode() == FbxLayerElement::eByPolygon)
	{
		for (uint32 i = 0; i < PolygonCount; i++)
		{
			uint32 PolygonVertexCount = fbxMesh->GetPolygonSize(i);
			uint32 matIdx = materialIndices->GetAt(i);
			uint32 idxDataStart = NewSkinnedMeshRawData->_VertexHeader.indexDataStartIndex[matIdx];
			for (uint32 j = 1; j < PolygonVertexCount - 1; j++)
			{
				SS::pair<uint32, int32> CtrlPointIdx = PolygonVertexToCtrlPointMap[i][0];
				uint32 ssIdx = ControlPointToSSIdxMap[CtrlPointIdx.first][CtrlPointIdx.second];
				NewSkinnedMeshRawData->_indexData[idxDataStart + subMaterialIdxDataCounter[matIdx] + 2] = ssIdx;

				CtrlPointIdx = PolygonVertexToCtrlPointMap[i][j];
				ssIdx = ControlPointToSSIdxMap[CtrlPointIdx.first][CtrlPointIdx.second];
				NewSkinnedMeshRawData->_indexData[idxDataStart + subMaterialIdxDataCounter[matIdx] + 1] = ssIdx;

				CtrlPointIdx = PolygonVertexToCtrlPointMap[i][j + 1];
				ssIdx = ControlPointToSSIdxMap[CtrlPointIdx.first][CtrlPointIdx.second];
				NewSkinnedMeshRawData->_indexData[idxDataStart + subMaterialIdxDataCounter[matIdx]] = ssIdx;

				subMaterialIdxDataCounter[matIdx] += 3;
			}
		}
	}
	else
	{
		for (uint32 i = 0; i < fbxMesh->GetPolygonCount(); i++)
		{
			uint32 thisPolygonSize = fbxMesh->GetPolygonSize(i);
			for (uint32 j = 1; j < thisPolygonSize - 1; j++)
			{
				SS::pair<uint32, int32> CtrlPointIdx = PolygonVertexToCtrlPointMap[i][j + 1];
				uint32 ssIdx = ControlPointToSSIdxMap[CtrlPointIdx.first][CtrlPointIdx.second];
				NewSkinnedMeshRawData->_indexData[subMaterialIdxDataCounter[0]] = ssIdx;

				CtrlPointIdx = PolygonVertexToCtrlPointMap[i][j];
				ssIdx = ControlPointToSSIdxMap[CtrlPointIdx.first][CtrlPointIdx.second];
				NewSkinnedMeshRawData->_indexData[subMaterialIdxDataCounter[0] + 1] = ssIdx;

				CtrlPointIdx = PolygonVertexToCtrlPointMap[i][0];
				ssIdx = ControlPointToSSIdxMap[CtrlPointIdx.first][CtrlPointIdx.second];
				NewSkinnedMeshRawData->_indexData[subMaterialIdxDataCounter[0] + 2] = ssIdx;

				subMaterialIdxDataCounter[0] += 3;
			}
		}
	}

	for (uint32 i = 0; i < NewSkinnedMeshRawData->_VertexHeader.subMeshCnt; i++)
	{
		if (NewSkinnedMeshRawData->_VertexHeader.indexDataCnt[i] != subMaterialIdxDataCounter[i])
		{
			SS_INTERRUPT();
			return nullptr;
		}
	}



	// Load Tangent Data
	FbxGeometryElementTangent* fbxTangent = fbxMesh->GetElementTangent();
	if (fbxTangent == nullptr)
	{
		for (uint32 subGeomIdx = 0; subGeomIdx < NewSkinnedMeshRawData->_VertexHeader.subMeshCnt; subGeomIdx++)
		{
			uint32* thisIdxData = NewSkinnedMeshRawData->_indexData + NewSkinnedMeshRawData->_VertexHeader.indexDataStartIndex[subGeomIdx];
			uint32 thisIdxDataNum = NewSkinnedMeshRawData->_VertexHeader.indexDataCnt[subGeomIdx];
			SS_ASSERT(thisIdxDataNum % 3 == 0);

			for (uint32 i = 0; i < thisIdxDataNum; i += 3)
			{
				SSDefaultVertex& v0 = ssSkinnedVertex[thisIdxData[i]];
				SSDefaultVertex& v1 = ssSkinnedVertex[thisIdxData[i + 1]];
				SSDefaultVertex& v2 = ssSkinnedVertex[thisIdxData[i + 2]];

				Vector4f dv1 = v1.Pos - v0.Pos;
				Vector4f dv2 = v2.Pos - v0.Pos;

				Vector2f duv1 = v1.Uv[0] - v0.Uv[0];
				Vector2f duv2 = v2.Uv[0] - v0.Uv[0];

				float detInverse = 1.0f / (duv1.X * duv2.Y - duv1.Y * duv2.X);
				Vector4f tangent = (dv1 * duv2.Y - dv2 * duv1.Y) * detInverse;
				v2.Tangent = v1.Tangent = v0.Tangent = tangent;
			}
		}
	}

	NewMeshAsset->InjectRawDataXXX(NewSkinnedMeshRawData);
	return NewMeshAsset;
}

SS::SHasherW SSFBXImporterUtils::FindPBRTextureNameFromFBXMaterial(
	const FbxSurfaceMaterial* FbxMtl,
	EDefaultPBRMatTexTypes InTexType)
{
	const char* FbxTexName = nullptr;
	switch (InTexType)
	{
	case EDefaultPBRMatTexTypes::BaseColor: FbxTexName = FbxSurfaceMaterial::sDiffuse; break;
	case EDefaultPBRMatTexTypes::Normal: FbxTexName = FbxSurfaceMaterial::sDiffuse; break;
	case EDefaultPBRMatTexTypes::Metallic: FbxTexName = FbxSurfaceMaterial::sDiffuse; break;
	case EDefaultPBRMatTexTypes::Emissive: FbxTexName = FbxSurfaceMaterial::sDiffuse; break;
	case EDefaultPBRMatTexTypes::Occlusion: FbxTexName = FbxSurfaceMaterial::sDiffuse; break;
	}

	if (FbxTexName == nullptr)
	{
		SS_ASSERT(false);
		return SS::SHasherW::GetEmpty();
	}

	FbxProperty prop = FbxMtl->FindProperty(FbxTexName);
	if (prop.IsValid() == false)
	{
		SS_ASSERT(false);
		return SS::SHasherW::GetEmpty();
	}

	const FbxFileTexture* fbxTexture = prop.GetSrcObject<FbxFileTexture>();
	if (fbxTexture == nullptr)
	{
		SS_ASSERT(false);
		return SS::SHasherW::GetEmpty();
	}


	SS::StringW strTextureAssetName;
	SS::StringW TextureAssetPath = fbxTexture->GetFileName();
	ExtractFileNameFromPath(strTextureAssetName, TextureAssetPath.C_Str());

	SS::SHasherW TextureAssetName = strTextureAssetName.C_Str();

	return TextureAssetName;
}

void SSFBXImporterUtils::ExtractOriginalBoneFromFbxSkin(SS::SHasherW RootBoneName, MeshRawDataSkinned* RawDataToSaveBone, FbxSkin* fbxSkin)
{
	uint32 ClusterCnt = fbxSkin->GetClusterCount();

	RawDataToSaveBone->_BoneHeader._BoneCnt = ClusterCnt;
	SS::PooledList<Transform>& OutBoneTransforms = RawDataToSaveBone->_BonePlacements;
	SS::PooledList<SS::SHasherW>& OutBoneNames = RawDataToSaveBone->_BoneNames;
	OutBoneTransforms.Reserve(ClusterCnt);
	OutBoneNames.Reserve(ClusterCnt);

	SS::PooledList<int32, SS::InlineAllocator<200>> BoneParentIndices(ClusterCnt);
	SS::PooledList<FbxNode*, SS::InlineAllocator<200>> BoneMatchingNodes(ClusterCnt);


	constexpr int32 STR_BUFFER_SIZE = 512;
	utf16 Utf16Buffer[STR_BUFFER_SIZE];

	for (int32 BoneIdx = 0; BoneIdx < ClusterCnt; BoneIdx++)
	{
		FbxCluster* CurCluster = fbxSkin->GetCluster(BoneIdx);
		FbxNode* CurBoneNode = CurCluster->GetLink();


		FbxString fStrName = CurBoneNode->GetNameOnly();
		int32 StrLen = fStrName.GetLen();
		char8_t* u8Name = reinterpret_cast<char8_t*>(fStrName.Buffer());
		UTF8StrToUTF16Str(reinterpret_cast<char*>(u8Name), StrLen, Utf16Buffer, STR_BUFFER_SIZE);


		SS::SHasherW CurBoneName = Utf16Buffer;


		OutBoneTransforms.PushBack(Transform::Identity);
		OutBoneNames.PushBack(CurBoneName);
		BoneMatchingNodes.PushBack(CurBoneNode);
	}


	for (int32 i = 0; i < ClusterCnt; i++)
	{
		FbxNode* CurNode = BoneMatchingNodes[i];
		FbxNode* ParentNode = CurNode->GetParent();

		int32 ParentNodeIdx = INVALID_IDX;
		for (int32 FindIdx = 0; FindIdx < ClusterCnt; FindIdx++)
		{
			if (BoneMatchingNodes[FindIdx] == ParentNode)
			{
				ParentNodeIdx = FindIdx;
				break;
			}
		}

		BoneParentIndices.PushBack(ParentNodeIdx); // 부모노드의 인덱스를 찾는다
	}



	for (int32 BoneItemIdx = 0; BoneItemIdx < ClusterCnt; BoneItemIdx++)
	{
		int32 ParentBoneIdx = BoneParentIndices[BoneItemIdx];


		FbxNode* CurNode = BoneMatchingNodes[BoneItemIdx];

		Transform BoneTransformResult;
		const char8_t* FORDEBUG_CurNodeName = (char8_t*)CurNode->GetName();

		BoneTransformResult = ExtractBoneRootRelativeTransform(CurNode); // 현재 노드의 Transform을 가지고온다.
		OutBoneTransforms[BoneItemIdx] = BoneTransformResult;
	}

	int a = 0;
}
