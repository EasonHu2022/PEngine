#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <Queue>
#include <string>

namespace pengine
{
	struct Vertex;
	class Scene;
	//gpuscene which attach to scene
	class GPUScene 
	{
	public:
		GPUScene(Scene* parrentScene);
		void UpdateBuffers();
	private:
		void BuildSceneVertexBuffer();
		void BuildSceneIndexBuffer();
	private:
		Scene* scene;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		
	};
}