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
	CubeRawData->_eachVertexSize = sizeof(SSDefaultVertex);
	CubeRawData->_vertexCnt = 24;

	CubeRawData->_subMeshCnt = 1;

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
	CubeRawData->_indexDataCnt[0] = 36;
	CubeRawData->_indexDataStartIndex[0] = 0;
	CubeRawData->_wholeIndexDataCnt = 36;

	CubeRawData->_MeshType = EMeshType::Rigid;

	return CubeRawData;
}

MeshRawDataDefault* CreateSphere1mRawData()
{
	return nullptr;
}
