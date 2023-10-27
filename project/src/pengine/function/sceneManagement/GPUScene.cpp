#include "Scene.h"
#include "function/engine/Vertex.h"
#include "GPUScene.h"
namespace pengine
{
	
	
	
	GPUScene::GPUScene(Scene* parrentScene)
	{
		scene = parrentScene;
		//Build
		BuildSceneVertexBuffer();
		BuildSceneIndexBuffer();
	}

	void GPUScene::UpdateBuffers()
	{
	}

	void GPUScene::BuildSceneVertexBuffer()
	{
		//merge all meshes' VB

	}

	void GPUScene::BuildSceneIndexBuffer()
	{
	}

};