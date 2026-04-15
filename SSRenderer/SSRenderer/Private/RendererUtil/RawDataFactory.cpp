#include "RawDataFactory.h"

#include "SSRenderer/Public/RenderAsset/RenderAssetType/MeshData/MeshDataDefault.h"
#include "SSRenderer/Public/RenderCommon/SSVertexType.h"

MeshRawDataDefault* CreateCube1mRawData()
{
	MeshRawDataDefault* CubeRawData = DBG_NEW MeshRawDataDefault();

	SSDefaultVertex* Vertices = (SSDefaultVertex*)DBG_MALLOC(sizeof(SSDefaultVertex) * 24);
	Vertices[0] = // 아래면
	{
		.Pos = Vector4f(-0.5, 0, 0.5, 1),
		.Normal = Vector4f(0, -1, 0, 0),
		.Tangent = Vector4f(1, 0, 0, 0),
		.Uv =
			{
				Vector2f(0, 1),
				Vector2f(0, 0)
			}
	};
	Vertices[1] =
	{
		.Pos = Vector4f(-0.5, 0, -0.5, 1),
		.Normal = Vector4f(0, -1, 0, 0),
		.Tangent = Vector4f(1, 0, 0, 0),
		.Uv =
			{
				Vector2f(0, 0),
				Vector2f(0, 0)
			}
	};
	Vertices[2] =
	{
		.Pos = Vector4f(0.5, 0, -0.5, 1),
		.Normal = Vector4f(0, -1, 0, 0),
		.Tangent = Vector4f(1, 0, 0, 0),
		.Uv =
			{
				Vector2f(1, 0),
				Vector2f(0, 0)
			}
	};
	Vertices[3] =
	{
		.Pos = Vector4f(0.5, 0, 0.5, 1),
		.Normal = Vector4f(0, -1, 0, 0),
		.Tangent = Vector4f(1, 0, 0, 0),
		.Uv =
			{
				Vector2f(1, 1),
				Vector2f(0, 0)
			}
	};


	Vertices[4] = // 앞면
	{
		.Pos = Vector4f(0.5, 0, 0.5, 1),
		.Normal = Vector4f(0, 0, 1, 0),
		.Tangent = Vector4f(-1, 0, 0, 0),
		.Uv =
			{
				Vector2f(0, 1),
				Vector2f(0, 0)
			}
	};
	Vertices[5] =
	{
		.Pos = Vector4f(0.5, 1, 0.5, 1),
		.Normal = Vector4f(0, 0, 1, 0),
		.Tangent = Vector4f(-1, 0, 0, 0),
		.Uv =
			{
				Vector2f(0, 0),
				Vector2f(0, 0)
			}
	};
	Vertices[6] =
	{
		.Pos = Vector4f(-0.5, 1, 0.5, 1),
		.Normal = Vector4f(0, 0, 1, 0),
		.Tangent = Vector4f(-1, 0, 0, 0),
		.Uv =
			{
				Vector2f(1, 0),
				Vector2f(0, 0)
			}
	};
	Vertices[7] =
	{
		.Pos = Vector4f(-0.5, 0, 0.5, 1),
		.Normal = Vector4f(0, 0, 1, 0),
		.Tangent = Vector4f(-1, 0, 0, 0),
		.Uv =
			{
				Vector2f(1, 1),
				Vector2f(0, 0)
			}
	};


	Vertices[8] = // 왼쪽면
	{
		.Pos = Vector4f(-0.5, 0, 0.5, 1),
		.Normal = Vector4f(-1, 0, 0, 0),
		.Tangent = Vector4f(0, 0, -1, 0),
		.Uv =
			{
				Vector2f(0, 1),
				Vector2f(0, 0)
			}
	};
	Vertices[9] =
	{
		.Pos = Vector4f(-0.5, 1, 0.5, 1),
		.Normal = Vector4f(-1, 0, 0, 0),
		.Tangent = Vector4f(0, 0, -1, 0),
		.Uv =
			{
				Vector2f(0, 0),
				Vector2f(0, 0)
			}
	};
	Vertices[10] = 
	{
		.Pos = Vector4f(-0.5, 1, -0.5, 1),
		.Normal = Vector4f(-1, 0, 0, 0),
		.Tangent = Vector4f(0, 0, -1, 0),
		.Uv =
			{
				Vector2f(1, 0),
				Vector2f(0, 0)
			}
	};
	Vertices[11] =
	{
		.Pos = Vector4f(-0.5, 0, -0.5, 1),
		.Normal = Vector4f(-1, 0, 0, 0),
		.Tangent = Vector4f(0, 0, -1, 0),
		.Uv =
			{
				Vector2f(1, 1),
				Vector2f(0, 0)
			}
	};


	Vertices[12] = // 오른쪽면
	{
		.Pos = Vector4f(0.5, 0, -0.5, 1),
		.Normal = Vector4f(1, 0, 0, 0),
		.Tangent = Vector4f(0, 0, 1, 0),
		.Uv =
			{
				Vector2f(0, 1),
				Vector2f(0, 0)
			}
	};
	Vertices[13] =
	{
		.Pos = Vector4f(0.5, 1, -0.5, 1),
		.Normal = Vector4f(1, 0, 0, 0),
		.Tangent = Vector4f(0, 0, 1, 0),
		.Uv =
			{
				Vector2f(0, 0),
				Vector2f(0, 0)
			}
	};
	Vertices[14] =
	{
		.Pos = Vector4f(0.5, 1, 0.5, 1),
		.Normal = Vector4f(1, 0, 0, 0),
		.Tangent = Vector4f(0, 0, 1, 0),
		.Uv =
			{
				Vector2f(1, 0),
				Vector2f(0, 0)
			}
	};
	Vertices[15] =
	{
		.Pos = Vector4f(0.5, 0, 0.5, 1),
		.Normal = Vector4f(1, 0, 0, 0),
		.Tangent = Vector4f(0, 0, 1, 0),
		.Uv =
			{
				Vector2f(1, 1),
				Vector2f(0, 0)
			}
	};


	Vertices[16] = // 뒤쪽면
	{
		.Pos = Vector4f(-0.5, 0, -0.5, 1),
		.Normal = Vector4f(0, 0, -1, 0),
		.Tangent = Vector4f(1, 0, 0, 0),
		.Uv =
			{
				Vector2f(0, 1),
				Vector2f(0, 0)
			}
	};
	Vertices[17] =
	{
		.Pos = Vector4f(-0.5, 1, -0.5, 1),
		.Normal = Vector4f(0, 0, -1, 0),
		.Tangent = Vector4f(1, 0, 0, 0),
		.Uv =
			{
				Vector2f(0, 0),
				Vector2f(0, 0)
			}
	};
	Vertices[18] =
	{
		.Pos = Vector4f(0.5, 1, -0.5, 1),
		.Normal = Vector4f(0, 0, -1, 0),
		.Tangent = Vector4f(1, 0, 0, 0),
		.Uv =
			{
				Vector2f(1, 0),
				Vector2f(0, 0)
			}
	};
	Vertices[19] =
	{
		.Pos = Vector4f(0.5, 0, -0.5, 1),
		.Normal = Vector4f(0, 0, -1, 0),
		.Tangent = Vector4f(1, 0, 0, 0),
		.Uv =
			{
				Vector2f(1, 1),
				Vector2f(0, 0)
			}
	};


	Vertices[20] = // 위쪽면
	{
		.Pos = Vector4f(-0.5, 1, -0.5, 1),
		.Normal = Vector4f(0, 1, 0, 0),
		.Tangent = Vector4f(1, 0, 0, 0),
		.Uv =
			{
				Vector2f(0, 1),
				Vector2f(0, 0)
			}
	};
	Vertices[21] =
	{
		.Pos = Vector4f(-0.5, 1, 0.5, 1),
		.Normal = Vector4f(0, 1, 0, 0),
		.Tangent = Vector4f(1, 0, 0, 0),
		.Uv =
			{
				Vector2f(0, 0),
				Vector2f(0, 0)
			}
	};
	Vertices[22] =
	{
		.Pos = Vector4f(0.5, 1, 0.5, 1),
		.Normal = Vector4f(0, 1, 0, 0),
		.Tangent = Vector4f(1, 0, 0, 0),
		.Uv =
			{
				Vector2f(1, 0),
				Vector2f(0, 0)
			}
	};
	Vertices[23] =
	{
		.Pos = Vector4f(0.5, 1, -0.5, 1),
		.Normal = Vector4f(0, 1, 0, 0),
		.Tangent = Vector4f(1, 0, 0, 0),
		.Uv =
			{
				Vector2f(1, 1),
				Vector2f(0, 0)
			}
	};


	CubeRawData->_vertexData = Vertices;
	CubeRawData->_VertexHeader.MeshType = EMeshType::Rigid;
	CubeRawData->_VertexHeader.vertexCnt = 24;

	CubeRawData->_VertexHeader.subMeshCnt = 1;

	uint32* Indices = (uint32*)DBG_MALLOC(sizeof(uint32) * 36);
	Indices[0] = 0; // 아래면
	Indices[1] = 1;
	Indices[2] = 2;

	Indices[3] = 0;
	Indices[4] = 2;
	Indices[5] = 3;


	Indices[6] = 4; // 앞면
	Indices[7] = 5;
	Indices[8] = 6;

	Indices[9] = 4;
	Indices[10] = 6;
	Indices[11] = 7;


	Indices[12] = 8; // 왼쪽면
	Indices[13] = 9;
	Indices[14] = 10;

	Indices[15] = 8;
	Indices[16] = 10;
	Indices[17] = 11;


	Indices[18] = 12; // 오른쪽면
	Indices[19] = 13;
	Indices[20] = 14;

	Indices[21] = 12;
	Indices[22] = 14;
	Indices[23] = 15;


	Indices[24] = 16; // 뒤쪽면
	Indices[25] = 17;
	Indices[26] = 18;

	Indices[27] = 16;
	Indices[28] = 18;
	Indices[29] = 19;


	Indices[30] = 20; // 위쪽면
	Indices[31] = 21;
	Indices[32] = 22;

	Indices[33] = 20;
	Indices[34] = 22;
	Indices[35] = 23;

	CubeRawData->_indexData = Indices;
	CubeRawData->_VertexHeader.indexDataCnt[0] = 36;
	CubeRawData->_VertexHeader.indexDataStartIndex[0] = 0;
	CubeRawData->_VertexHeader.wholeIndexDataCnt = 36;


	return CubeRawData;
}

// DetailLevel이 1이면 정팔면체가 나온다
// DetailLevel은 구체를 8사분면 따라서 짤랐을 때 각가의 경도, 위도의 개수와 일치한다.
MeshRawDataDefault* CreateSphere1mRawData(int32 DetailLevel)
{
	DetailLevel *= 4;

	const int32 VertexCnt =
		(DetailLevel + 1) * // 경도 개수
		(DetailLevel / 2 - 1) // 위도 개수
		+ 2; // 극점 2개

	SSDefaultVertex* Vertices = (SSDefaultVertex*)DBG_MALLOC(sizeof(SSDefaultVertex) * VertexCnt);

	int32 CurVertexCnt = 0;
	for (int32 LatitudeIdx = 1; LatitudeIdx < DetailLevel / 2; LatitudeIdx++) // 위도(지구본 가로줄)
	{
		float LatitudeRad = XM_PI * ((float)LatitudeIdx / (float)(DetailLevel / 2));
		float y = -0.5 * cos(LatitudeRad);
		
		float ScaleXZ = sin(LatitudeRad);
		
		for (int32 LongituteIdx = 0; LongituteIdx <= DetailLevel; LongituteIdx++) // 경도(지구본 세로줄)
		{
			float LongtitudeRad = XM_2PI * ((float)LongituteIdx / (float)DetailLevel);
			
			float x = 0.5 * sin(LongtitudeRad); // Back 방향부터 시작
			float z = -0.5 * cos(LongtitudeRad);
			x *= ScaleXZ;
			z *= ScaleXZ;

			Vertices[CurVertexCnt++] =
				{
					.Pos = Vector4f(x, y, z, 1),
					.Normal = Vector4f(x * 2, y * 2, z * 2, 0),
					.Tangent = Vector4f(0, 0, 0, 0),
					.Uv =
						{
							Vector2f(LongtitudeRad / XM_2PI, 1 - (LatitudeRad / XM_PI)),
							Vector2f(0, 0)
						}
				};
		}
	}

	// North Pole
	Vertices[CurVertexCnt++] =
	{
		.Pos = Vector4f(0, 0.5, 0, 1),
		.Normal = Vector4f(0, 1, 0, 0),
		.Tangent = Vector4f(0, 0, 0, 0),
		.Uv =
			{
				Vector2f(0.5, 0),
				Vector2f(0, 0)
			}
	};

	// South Pole
	Vertices[CurVertexCnt++] =
	{
		.Pos = Vector4f(0, -0.5, 0, 1),
		.Normal = Vector4f(0, -1, 0, 0),
		.Tangent = Vector4f(0, 0, 0, 0),
		.Uv =
			{
				Vector2f(0.5, 1),
				Vector2f(0, 0)
			}
	};
	SS_ASSERT(CurVertexCnt == VertexCnt);

	const int32 IndexCnt =
		6 *
		DetailLevel * // 경도 개수
		(DetailLevel / 2 - 2)// 위도 개수
		+
		2 * // 극점 개수
		3 * // 삼각형당 인덱스 3개
		DetailLevel; // 극점에 몰리는 정점 개수
	uint32* Indices = (uint32*)DBG_MALLOC(sizeof(uint32) * IndexCnt);

	int32 CurIndexCnt = 0;
	for (int32 LatitudeIdx = 0; LatitudeIdx < DetailLevel / 2 - 2; LatitudeIdx++) // 위도(지구본 가로줄)
	{
		for (int32 LongituteIdx = 0; LongituteIdx < DetailLevel; LongituteIdx++) // 경도(지구본 세로줄)
		{
			const int32 NextLatitudeStartOffsetInVertices = (DetailLevel + 1) * (LatitudeIdx + 1);
			const int32 NextLatitudeOffsetInVertices = NextLatitudeStartOffsetInVertices + LongituteIdx;

			const int32 CurLatitudeStartOffsetInVertices = (DetailLevel + 1) * (LatitudeIdx);
			const int32 CurLatitudeOffsetInVertices = CurLatitudeStartOffsetInVertices + LongituteIdx;

			if (CurLatitudeOffsetInVertices >= VertexCnt || NextLatitudeOffsetInVertices >= VertexCnt)
			{
				__debugbreak();
			}

			Indices[CurIndexCnt++] = CurLatitudeOffsetInVertices;
			Indices[CurIndexCnt++] = NextLatitudeOffsetInVertices;
			Indices[CurIndexCnt++] = NextLatitudeOffsetInVertices + 1;

			Indices[CurIndexCnt++] = CurLatitudeOffsetInVertices;
			Indices[CurIndexCnt++] = NextLatitudeOffsetInVertices + 1;
			Indices[CurIndexCnt++] = CurLatitudeOffsetInVertices + 1;
		}
	}

	const int32 LastLatitudeIdx = DetailLevel / 2 - 2;
	const int32 LastLatitudeOffsetInVertices = (DetailLevel + 1) * LastLatitudeIdx;
	for (int32 LongituteIdx = 0; LongituteIdx < DetailLevel; LongituteIdx++) // 북극
	{
		Indices[CurIndexCnt++] = LastLatitudeOffsetInVertices + LongituteIdx;
		Indices[CurIndexCnt++] = CurVertexCnt - 2;
		Indices[CurIndexCnt++] = LastLatitudeOffsetInVertices + LongituteIdx + 1;
	}

	for (int32 LongituteIdx = 0; LongituteIdx < DetailLevel; LongituteIdx++) // 남극
	{
		Indices[CurIndexCnt++] = LongituteIdx;
		Indices[CurIndexCnt++] = LongituteIdx + 1;
		Indices[CurIndexCnt++] = CurVertexCnt - 1;
	}

	SS_ASSERT(CurIndexCnt == IndexCnt);

	for (uint32 i = 0; i < IndexCnt; i += 3)
	{
		SSDefaultVertex& v0 = Vertices[Indices[i]];
		SSDefaultVertex& v1 = Vertices[Indices[i + 1]];
		SSDefaultVertex& v2 = Vertices[Indices[i + 2]];

		Vector4f dv1 = v1.Pos - v0.Pos;
		Vector4f dv2 = v2.Pos - v0.Pos;

		Vector2f duv1 = v1.Uv[0] - v0.Uv[0];
		Vector2f duv2 = v2.Uv[0] - v0.Uv[0];

		float detInverse = 1.0f / (duv1.X * duv2.Y - duv1.Y * duv2.X);
		Vector4f tangent = (dv1 * duv2.Y - dv2 * duv1.Y) * detInverse;
		v2.Tangent = v1.Tangent = v0.Tangent = tangent;
	}


	MeshRawDataDefault* SphereRawData = DBG_NEW MeshRawDataDefault();


	SphereRawData->_vertexData = Vertices;
	SphereRawData->_VertexHeader.MeshType = EMeshType::Rigid;
	SphereRawData->_VertexHeader.vertexCnt = VertexCnt;

	SphereRawData->_VertexHeader.subMeshCnt = 1;

	SphereRawData->_indexData = Indices;
	SphereRawData->_VertexHeader.indexDataCnt[0] = CurIndexCnt;
	SphereRawData->_VertexHeader.indexDataStartIndex[0] = 0;
	SphereRawData->_VertexHeader.wholeIndexDataCnt = CurIndexCnt;


	return SphereRawData;
}
