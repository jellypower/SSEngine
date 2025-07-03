#include "SSFBXImporterUtils.h"

#include "SSEngineDefault/Public/SSContainer/SSString/SSStringW.h"
#include "SSRenderer/Public/RenderAsset/MeshAssetManager.h"
#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshAsset.h"
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


	fbxMat = node->EvaluateLocalTransform(fbxTime) * fbxMat;

	Transform transform;

	const FbxDouble3 fbxTranslate = fbxMat.GetT();
	transform.Position.X = -fbxTranslate.mData[0] * 0.01;
	transform.Position.Y = fbxTranslate.mData[1] * 0.01;
	transform.Position.Z = fbxTranslate.mData[2] * 0.01;
	transform.Position.W = 0;

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
		assert(fbxUV != nullptr);

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

MeshAsset* SSFBXImporterUtils::GenerateNewMeshAssestFromFbxMesh(FbxMesh* fbxMesh, SS::SHasherW NewAssetName, const utf16* InAssetPath)
{
	assert(fbxMesh != nullptr);

	MeshAsset* NewMeshAsset = DBG_NEW MeshAsset(NewAssetName, InAssetPath, EMeshType::Rigid);

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
	NewMeshAsset->_vertexCnt = ssVertexBuffer.GetSize();
	NewMeshAsset->_eachVertexSize = sizeof(SSDefaultVertex);
	uint32 validVertexBufferSize = NewMeshAsset->_eachVertexSize * NewMeshAsset->_vertexCnt;
	NewMeshAsset->_vertexData = DBG_MALLOC(validVertexBufferSize);
	SSDefaultVertex* ssVertex = (SSDefaultVertex*)NewMeshAsset->_vertexData;

	// - copy to real time vertex buffer
	memcpy_s(ssVertex, validVertexBufferSize, ssVertexBuffer.GetData(), validVertexBufferSize);


	// - alloc index memory
	if (fbxMesh->GetNode()->GetMaterial(0) != nullptr)
		NewMeshAsset->_subMeshCnt = fbxMesh->GetNode()->GetMaterialCount();
	else
		NewMeshAsset->_subMeshCnt = 1;
	assert(NewMeshAsset->_subMeshCnt < SUBMESH_COUNT_MAX);

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
			NewMeshAsset->_indexDataCnt[matIdx] += ((fbxMesh->GetPolygonSize(i) - 2) * 3);
		}
	}
	else
	{
		for (uint32 i = 0; i < PolygonCount; i++) // 메테리얼 1개 고정이니까 그냥 다 더함
		{
			NewMeshAsset->_indexDataCnt[0] += (fbxMesh->GetPolygonSize(i) - 2);
		}
		NewMeshAsset->_indexDataCnt[0] *= 3;
	}

	uint32 idxAcc = 0;
	for (uint32 i = 0; i < NewMeshAsset->_subMeshCnt; i++)
	{
		NewMeshAsset->_indexDataStartIndex[i] = idxAcc;
		idxAcc += NewMeshAsset->_indexDataCnt[i];
	}
	NewMeshAsset->_wholeIndexDataCnt = idxAcc;
	NewMeshAsset->_indexData = (uint32*)DBG_MALLOC(sizeof(uint32) * NewMeshAsset->_wholeIndexDataCnt);


	// 5. load index memory
	uint32 subMaterialIdxDataCounter[SUBMESH_COUNT_MAX] = { 0, };

	if (fbxMesh->GetElementMaterial() != nullptr && fbxMesh->GetElementMaterial()->GetMappingMode() == FbxLayerElement::eByPolygon) // 만약 머티리얼이 여러개면
	{
		for (uint32 i = 0; i < PolygonCount; i++)
		{
			uint32 PolygonVertexCount = fbxMesh->GetPolygonSize(i);
			uint32 matIdx = materialIndices->GetAt(i);
			uint32 idxDataStart = NewMeshAsset->_indexDataStartIndex[matIdx];
			for (uint32 j = 1; j < PolygonVertexCount - 1; j++)
			{
				SS::pair<uint32, int32> CtrlPointIdx = PolygonVertexToCtrlPointMap[i][0];
				uint32 ssVertexBufferIdx = ControlPointToSSIdxMap[CtrlPointIdx.first][CtrlPointIdx.second];
				NewMeshAsset->_indexData[idxDataStart + subMaterialIdxDataCounter[matIdx]] = ssVertexBufferIdx;

				CtrlPointIdx = PolygonVertexToCtrlPointMap[i][j];
				ssVertexBufferIdx = ControlPointToSSIdxMap[CtrlPointIdx.first][CtrlPointIdx.second];
				NewMeshAsset->_indexData[idxDataStart + subMaterialIdxDataCounter[matIdx] + 1] = ssVertexBufferIdx;

				CtrlPointIdx = PolygonVertexToCtrlPointMap[i][j + 1];
				ssVertexBufferIdx = ControlPointToSSIdxMap[CtrlPointIdx.first][CtrlPointIdx.second];
				NewMeshAsset->_indexData[idxDataStart + subMaterialIdxDataCounter[matIdx] + 2] = ssVertexBufferIdx;

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
				NewMeshAsset->_indexData[subMaterialIdxDataCounter[0]] = ssVertexBufferIdx;

				CtrlPointIdx = PolygonVertexToCtrlPointMap[i][j];
				ssVertexBufferIdx = ControlPointToSSIdxMap[CtrlPointIdx.first][CtrlPointIdx.second];
				NewMeshAsset->_indexData[subMaterialIdxDataCounter[0] + 1] = ssVertexBufferIdx;

				CtrlPointIdx = PolygonVertexToCtrlPointMap[i][0];
				ssVertexBufferIdx = ControlPointToSSIdxMap[CtrlPointIdx.first][CtrlPointIdx.second];
				NewMeshAsset->_indexData[subMaterialIdxDataCounter[0] + 2] = ssVertexBufferIdx;

				subMaterialIdxDataCounter[0] += 3;
			}
		}
	}

	for (uint32 i = 0; i < NewMeshAsset->_subMeshCnt; i++)
		assert(NewMeshAsset->_indexDataCnt[i] == subMaterialIdxDataCounter[i]);



	// Calculate tangent if there is no tangent data on FbxFormat
	FbxGeometryElementTangent* fbxTangent = fbxMesh->GetElementTangent();
	if (fbxTangent == nullptr)
	{
		for (uint32 subGeomIdx = 0; subGeomIdx < NewMeshAsset->_subMeshCnt; subGeomIdx++)
		{
			uint32* thisIdxData = NewMeshAsset->_indexData + NewMeshAsset->_indexDataStartIndex[subGeomIdx];
			int32 thisIdxDataNum = NewMeshAsset->_indexDataCnt[subGeomIdx];
			assert(thisIdxDataNum % 3 == 0);

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

	return NewMeshAsset;
}
