#include "StaticMesh.h"

namespace ZXEngine
{
	StaticMesh::StaticMesh(vector<Vertex>&& vertices, vector<uint32_t>&& indices, bool setup)
	{
		mVertices = std::move(vertices);
		mIndices = std::move(indices);

		if (setup)
		{
			RenderAPI::GetInstance()->SetUpStaticMesh(VAO, mVertices, mIndices);
			mIsSetUp = true;
		}
	}

	StaticMesh::StaticMesh(const vector<Vertex>& vertices, const vector<uint32_t>& indices, bool setup)
	{
		mVertices = vertices;
		mIndices = indices;

		if (setup)
		{
			RenderAPI::GetInstance()->SetUpStaticMesh(VAO, vertices, indices);
			mIsSetUp = true;
		}
	}

	void StaticMesh::SetUp()
	{
		if (mIsSetUp)
			return;

		RenderAPI::GetInstance()->SetUpStaticMesh(VAO, mVertices, mIndices);
		mIsSetUp = true;
	}
}