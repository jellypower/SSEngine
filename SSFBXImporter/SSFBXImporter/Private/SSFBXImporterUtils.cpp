#include "SSFBXImporterUtils.h"

#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSEngineDefault/Public/SSContainer/ContainerUtil/ContainerUtil.h"

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
	FbxAMatrix fbxMat;
	fbxMat.SetIdentity();
	fbxMat.SetT(node->GetGeometricTranslation(FbxNode::eSourcePivot));
	fbxMat.SetR(node->GetGeometricRotation(FbxNode::eSourcePivot));
	fbxMat.SetS(node->GetGeometricScaling(FbxNode::eSourcePivot));

	const FbxAMatrix& AnimTransform = node->EvaluateLocalTransform(fbxTime);
	fbxMat = AnimTransform * fbxMat;

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
	transform.Rotation.X = fbxRotation.mData[0];
	transform.Rotation.Y = -fbxRotation.mData[1];
	transform.Rotation.Z = -fbxRotation.mData[2];
	transform.Rotation.W = fbxRotation.mData[3];


	//	transform.Rotation = Quaternion::RotateAxisAngle(transform.Rotation, Vector4f::Right, SS::DegToRadians(fbxRotation.mData[1]));
	//	transform.Rotation = Quaternion::RotateAxisAngle(transform.Rotation, Vector4f::Forward, -SS::DegToRadians(fbxRotation.mData[2]));
	//	transform.Rotation = Quaternion::RotateAxisAngle(transform.Rotation, Vector4f::Up, SS::DegToRadians(fbxRotation.mData[0]));


	return transform;

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

	if (fbxNormal->GetMappingMode() == FbxLayerElement::eByPolygonVertex) fbxNormalIdx = fbxMesh->GetPolygonVertexIndex(polygonIdx) + positionInPolygon;
	else fbxNormalIdx = fbxMesh->GetPolygonVertex(polygonIdx, positionInPolygon);

	switch (fbxNormal->GetReferenceMode())
	{
	case FbxLayerElement::eDirect:

		normalVector = fbxNormal->GetDirectArray().GetAt(fbxNormalIdx);

		break;
	case FbxLayerElement::eIndex:
	case FbxLayerElement::eIndexToDirect:

		fbxNormalIdx = fbxNormal->GetIndexArray().GetAt(outControlPointIdx);
		normalVector = fbxNormal->GetDirectArray().GetAt(fbxNormalIdx);

		break;
	default:
		SS_ASSERT_MSG(false, L"Invalid Fbxformat. function just return");
	}

	normalVector.Normalize();
	outVertex.Normal.X = -normalVector.mData[0];
	outVertex.Normal.Y = normalVector.mData[1];
	outVertex.Normal.Z = normalVector.mData[2];
	outVertex.Normal.W = normalVector.mData[3];


	// UV
	uint32 uvChannelCnt = fbxMesh->GetUVLayerCount();
	if (uvChannelCnt > VERTEX_UV_MAP_COUNT_MAX)
	{
		// SS_ASSERT_MSG(false, L"Too many uv channel");
		uvChannelCnt = VERTEX_UV_MAP_COUNT_MAX;
	}

	for (uint32 i = 0; i < uvChannelCnt; i++)
	{
		FbxVector2 uvVector;
		uint32 uvIdx;
		uint32 polygonVertexIdx;
		uint32 directIdx;
		const FbxGeometryElementUV* fbxUV = fbxMesh->GetElementUV(i);
		SS_ASSERT(fbxUV != nullptr);

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

				uvIdx = fbxUV->GetIndexArray().GetAt(outControlPointIdx);
				uvVector = fbxUV->GetDirectArray().GetAt(uvIdx);

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
	SS_ASSERT(fbxMesh != nullptr);

	IAssetManagerMutable* AssetManager = g_Renderer->GetMutableAssetManager();

	
	IMeshAssetMutable* NewMeshAsset = AssetManager->CreateEmptyMeshAsset(NewAssetName, InAssetPath);
	MeshRawDataDefault* NewMeshRawData = DBG_NEW MeshRawDataSkinned();
	NewMeshRawData->_MeshType = EMeshType::Rigid;

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
	const FbxGeometryElementUV* fbxUV[VERTEX_UV_MAP_COUNT_MAX];
	if (uvChannelCnt > VERTEX_UV_MAP_COUNT_MAX)
	{
		// SS_ASSERT_MSG(false, L"Too many uv channel");
		uvChannelCnt = VERTEX_UV_MAP_COUNT_MAX;
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
	NewMeshRawData->_vertexCnt = ssVertexBuffer.GetSize();
	NewMeshRawData->_eachVertexSize = sizeof(SSDefaultVertex);
	uint32 validVertexBufferSize = NewMeshRawData->_eachVertexSize * NewMeshRawData->_vertexCnt;
	NewMeshRawData->_vertexData = DBG_MALLOC(validVertexBufferSize);
	SSDefaultVertex* ssVertex = (SSDefaultVertex*)NewMeshRawData->_vertexData;

	// - copy to real time vertex buffer
	memcpy_s(ssVertex, validVertexBufferSize, ssVertexBuffer.GetData(), validVertexBufferSize);


	// - alloc index memory
	if (fbxMesh->GetNode()->GetMaterial(0) != nullptr)
		NewMeshRawData->_subMeshCnt = fbxMesh->GetNode()->GetMaterialCount();
	else
		NewMeshRawData->_subMeshCnt = 1;
	SS_ASSERT(NewMeshRawData->_subMeshCnt < SUBMESH_COUNT_MAX);

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
			NewMeshRawData->_indexDataCnt[matIdx] += ((fbxMesh->GetPolygonSize(i) - 2) * 3);
		}
	}
	else
	{
		for (uint32 i = 0; i < PolygonCount; i++) // 메테리얼 1개 고정이니까 그냥 다 더함
		{
			NewMeshRawData->_indexDataCnt[0] += (fbxMesh->GetPolygonSize(i) - 2);
		}
		NewMeshRawData->_indexDataCnt[0] *= 3;
	}

	uint32 idxAcc = 0;
	for (uint32 i = 0; i < NewMeshRawData->_subMeshCnt; i++)
	{
		NewMeshRawData->_indexDataStartIndex[i] = idxAcc;
		idxAcc += NewMeshRawData->_indexDataCnt[i];
	}
	NewMeshRawData->_wholeIndexDataCnt = idxAcc;
	NewMeshRawData->_indexData = (uint32*)DBG_MALLOC(sizeof(uint32) * NewMeshRawData->_wholeIndexDataCnt);


	// 5. load index memory
	uint32 subMaterialIdxDataCounter[SUBMESH_COUNT_MAX] = { 0, };

	if (fbxMesh->GetElementMaterial() != nullptr && fbxMesh->GetElementMaterial()->GetMappingMode() == FbxLayerElement::eByPolygon) // 만약 머티리얼이 여러개면
	{
		for (uint32 i = 0; i < PolygonCount; i++)
		{
			uint32 PolygonVertexCount = fbxMesh->GetPolygonSize(i);
			uint32 matIdx = materialIndices->GetAt(i);
			uint32 idxDataStart = NewMeshRawData->_indexDataStartIndex[matIdx];
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

	for (uint32 i = 0; i < NewMeshRawData->_subMeshCnt; i++)
		SS_ASSERT(NewMeshRawData->_indexDataCnt[i] == subMaterialIdxDataCounter[i]);



	// Calculate tangent if there is no tangent data on FbxFormat
	FbxGeometryElementTangent* fbxTangent = fbxMesh->GetElementTangent();
	if (fbxTangent == nullptr)
	{
		for (uint32 subGeomIdx = 0; subGeomIdx < NewMeshRawData->_subMeshCnt; subGeomIdx++)
		{
			uint32* thisIdxData = NewMeshRawData->_indexData + NewMeshRawData->_indexDataStartIndex[subGeomIdx];
			int32 thisIdxDataNum = NewMeshRawData->_indexDataCnt[subGeomIdx];
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

				float detInverse = 1.0f / (duv1.X * duv2.Y - duv1.Y * duv2.X);
				Vector4f tangent = (dv1 * duv2.Y - dv2 * duv1.Y) * detInverse;
				v2.Tangent = v1.Tangent = v0.Tangent = tangent;
			}
		}
	}

	NewMeshAsset->InjectRawDataXXX(NewMeshRawData);
	return NewMeshAsset;
}

IMeshAsset* SSFBXImporterUtils::GenerateNewSkinnedMeshAssestFromFbxMesh(FbxMesh* fbxMesh, SS::SHasherW NewAssetName,
	const utf16* InAssetPath)
{
	if (fbxMesh == nullptr)
	{
		SS_INTERRUPT();
		return nullptr;
	}

	IAssetManagerMutable* AssetManager = g_Renderer->GetMutableAssetManager();
	IMeshAssetMutable* NewMeshAsset = AssetManager->CreateEmptyMeshAsset(NewAssetName, InAssetPath);
	MeshRawDataSkinned* NewSkinnedMeshRawData = DBG_NEW MeshRawDataSkinned();
	NewSkinnedMeshRawData->_MeshType = EMeshType::Skinned;

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
	FbxGeometryElementUV* fbxUV[VERTEX_UV_MAP_COUNT_MAX];
	if (uvChannelCnt > VERTEX_UV_MAP_COUNT_MAX)
	{
		SS_ASSERT_MSG(false, L"Too many uv channel");
		uvChannelCnt = VERTEX_UV_MAP_COUNT_MAX;
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

	uint32 deformerCnt = fbxMesh->GetDeformerCount();
	assert(deformerCnt == 1);

	FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(0, FbxDeformer::eSkin));
	SS_ASSERT(fbxSkin != nullptr);

	ExtractOriginalBoneFromFbxSkin(NewSkinnedMeshRawData->_BoneOriginalPose, fbxSkin);
	
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
			if (ctrlPointWeight <= SKIN_WEIGHT_THRESHOLD)
			{
				for (uint32 ssIdx : ControlPointToSSIdxMap[ctrlPointIdx])
				{
					ssVertexBuffer[ssIdx].Weight[boneCnt] += ctrlPointWeight;
				}
				continue;
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
	NewSkinnedMeshRawData->_vertexCnt = ssVertexBuffer.GetSize();
	NewSkinnedMeshRawData->_eachVertexSize = sizeof(SSSkinnedVertex);
	uint32 validVertexBufferSize = NewSkinnedMeshRawData->_eachVertexSize * NewSkinnedMeshRawData->_vertexCnt;
	NewSkinnedMeshRawData->_vertexData = DBG_MALLOC(validVertexBufferSize);
	SSSkinnedVertex* ssSkinnedVertex = (SSSkinnedVertex*)NewSkinnedMeshRawData->_vertexData;

	// 3. copy to real time vertex buffer
	memcpy_s(ssSkinnedVertex, validVertexBufferSize, ssVertexBuffer.GetData(), validVertexBufferSize);


	// 4. alloc index memory
	if (fbxMesh->GetNode()->GetMaterial(0) != nullptr)
	{
		NewSkinnedMeshRawData->_subMeshCnt = fbxMesh->GetNode()->GetMaterialCount();
	}
	else
	{
		NewSkinnedMeshRawData->_subMeshCnt = 1;
	}
	SS_ASSERT(NewSkinnedMeshRawData->_subMeshCnt < SUBMESH_COUNT_MAX);


	FbxLayerElementArrayTemplate<int>* materialIndices = nullptr;
	if (fbxMesh->GetElementMaterial() != nullptr && fbxMesh->GetElementMaterial()->GetMappingMode() == FbxLayerElement::eByPolygon)
	{
		fbxMesh->GetMaterialIndices(&materialIndices);

		for (uint32 i = 0; i < PolygonCount; i++)
		{
			uint8 matIdx = materialIndices->GetAt(i);
			NewSkinnedMeshRawData->_indexDataCnt[matIdx] += ((fbxMesh->GetPolygonSize(i) - 2) * 3);
		}
	}
	else
	{
		for (uint32 i = 0; i < PolygonCount; i++)
		{
			NewSkinnedMeshRawData->_indexDataCnt[0] += (fbxMesh->GetPolygonSize(i) - 2);
		}
		NewSkinnedMeshRawData->_indexDataCnt[0] *= 3;
	}

	uint32 idxAcc = 0;
	for (uint8 i = 0; i < NewSkinnedMeshRawData->_subMeshCnt; i++)
	{
		NewSkinnedMeshRawData->_indexDataStartIndex[i] = idxAcc;
		idxAcc += NewSkinnedMeshRawData->_indexDataCnt[i];
	}
	NewSkinnedMeshRawData->_wholeIndexDataCnt = idxAcc;
	NewSkinnedMeshRawData->_indexData = (uint32*)DBG_MALLOC(sizeof(uint32) * NewSkinnedMeshRawData->_wholeIndexDataCnt);


	// 5. load index memory
	uint32 subMaterialIdxDataCounter[SUBMESH_COUNT_MAX] = { 0, };

	if (fbxMesh->GetElementMaterial() != nullptr && fbxMesh->GetElementMaterial()->GetMappingMode() == FbxLayerElement::eByPolygon)
	{
		for (uint32 i = 0; i < PolygonCount; i++)
		{
			uint32 PolygonVertexCount = fbxMesh->GetPolygonSize(i);
			uint32 matIdx = materialIndices->GetAt(i);
			uint32 idxDataStart = NewSkinnedMeshRawData->_indexDataStartIndex[matIdx];
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

	for (uint32 i = 0; i < NewSkinnedMeshRawData->_subMeshCnt; i++)
	{
		if (NewSkinnedMeshRawData->_indexDataCnt[i] != subMaterialIdxDataCounter[i])
		{
			SS_INTERRUPT();
			return nullptr;
		}
	}



	// Load Tangent Data
	FbxGeometryElementTangent* fbxTangent = fbxMesh->GetElementTangent();
	if (fbxTangent == nullptr)
	{
		for (uint32 subGeomIdx = 0; subGeomIdx < NewSkinnedMeshRawData->_subMeshCnt; subGeomIdx++)
		{
			uint32* thisIdxData = NewSkinnedMeshRawData->_indexData + NewSkinnedMeshRawData->_indexDataStartIndex[subGeomIdx];
			uint32 thisIdxDataNum = NewSkinnedMeshRawData->_indexDataCnt[subGeomIdx];
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

void SSFBXImporterUtils::ExtractOriginalBoneFromFbxSkin(SS::PooledList<BonePlacement>& OutBones, FbxSkin* fbxSkin)
{
	uint32 ClusterCnt = fbxSkin->GetClusterCount();

	OutBones.Reserve(ClusterCnt);

	SS::PooledList<int32> BoneParentIndices;
	BoneParentIndices.Reserve(ClusterCnt);

	for (int32 BoneIdx = 0; BoneIdx < ClusterCnt; BoneIdx++)
	{
		FbxCluster* CurCluster = fbxSkin->GetCluster(BoneIdx);
		FbxNode* CurBoneNode = CurCluster->GetLink();

		SS::SHasherW CurBoneName = CurBoneNode->GetName();

		BonePlacement NewBonePlacement;
		NewBonePlacement.BoneName = CurBoneName;
		OutBones.PushBack(NewBonePlacement);
	}


	for (int32 i = 0; i < ClusterCnt; i++)
	{
		FbxCluster* CurCluster = fbxSkin->GetCluster(i);
		FbxNode* CurNode = CurCluster->GetLink();
		SS::SHasherW ParentNodeName = CurNode->GetParent()->GetName();
		int32 ParentNodeIdx = INVALID_IDX;

		int32 FindIdx = 0;
		for (; FindIdx < ClusterCnt; FindIdx++)
		{
			if (OutBones[FindIdx].BoneName == ParentNodeName)
			{
				ParentNodeIdx = FindIdx;
				break;
			}
		}

		BoneParentIndices.PushBack(ParentNodeIdx); // 부모노드의 인덱스를 찾는다
	}

	// 전에 드래곤 모델처럼 리깅포인트의 Root가 2개 이상일 수 있음


	for (int32 BoneItemIdx = 0; BoneItemIdx < ClusterCnt; BoneItemIdx++)
	{
		int32 ParentBoneIdx = BoneParentIndices[BoneItemIdx];


		FbxCluster* CurCluster = fbxSkin->GetCluster(BoneItemIdx);
		FbxNode* CurNode = CurCluster->GetLink();

		Transform BoneTransformResult;
		SS::SHasherW FORDEBUG_CurNodeName = CurNode->GetName();
		if (ParentBoneIdx != INVALID_IDX) // Root본이 아니면
		{
			BoneTransformResult = ExtractTransformFromNode(CurNode); // 현재 노드의 Transform을 가지고온다.
		}


		while (ParentBoneIdx != INVALID_IDX)
		{
			Transform ParentTransform;

			if (BoneParentIndices[ParentBoneIdx] == INVALID_IDX) // 부모가 Root 본이면
			{
				ParentTransform = Transform::Identity; // 부모의 Transform은 원점이어야 한다.
			}
			else // 부모가 Root 본이 아니면 
			{
				FbxCluster* ParentCluster = fbxSkin->GetCluster(ParentBoneIdx);
				FbxNode* ParentNode = ParentCluster->GetLink();

				ParentTransform = ExtractTransformFromNode(ParentNode); // 부모의 상대좌표를 가져온다.
			}

			BoneTransformResult = ParentTransform * BoneTransformResult; // 곱해준다.

			ParentBoneIdx = BoneParentIndices[ParentBoneIdx];
		}

		OutBones[BoneItemIdx].BoneTransform = BoneTransformResult;
	}

	int a = 0;
}
