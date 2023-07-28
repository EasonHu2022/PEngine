#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <entt/entity/fwd.hpp>
#include "IPass.h"
#include "IRenderGraphResource.h"
#include "function/render/rhi/CommandBuffer.h"
#include "function/engine/Material.h"
#include "glm/glm.hpp"
#include "function/engine/Mesh.h"
#include "entt/entt.hpp"
#include "function/resource/IResource.h"
#include "PassGroup.h"
#include "OutputPass.h"

namespace pengine
{
	struct RenderUnit
	{
		Mesh* mesh = nullptr;
		Material* material = nullptr;
		PipelineInfo pipelineInfo;
		PipelineInfo stencilPipelineInfo;
		glm::mat4 transform;
	};
	//datas be shared between each pass
	//must write in onUpdate 
	//must read in execute
	//because no dependency will be recorded here
	struct CommonRenderData
	{
		glm::mat4* shadowTransforms;
		glm::vec4* splitDepth;
		uint32_t  shadowMapSize;
		uint32_t  shadowMapNum;
		glm::mat4 inverseCameraVP;
		glm::mat4 biasMatrix;//used for caculate shadow coords//https://blog.csdn.net/qq_35312463/article/details/117912599
	};

	class RenderGraph : public IResource
	{
	public:
		RenderGraph() = default;
		RenderGraph(std::string& path);
		~RenderGraph();
		auto init(entt::registry& registry,uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void;
		auto setup() -> void;
		//update cpu data for render
		auto update(entt::registry& registry,std::vector<entt::entity>& culledEnts) -> void;
		auto compile() -> void;
		auto execute(CommandBuffer* cmdBuffer) -> void;
		auto bindInput(uint32_t outputTask, size_t bindpos, uint32_t inputTask, size_t bindPos) -> bool;
		auto bindOutput(uint32_t prevTask, size_t prevbindpos, uint32_t postTask, size_t postbindPos) -> bool;
		auto onResize(uint32_t width, uint32_t height, uint32_t displayWidth, uint32_t displayHeight) -> void;
		auto getResourceByID(uint32_t id) -> std::shared_ptr<IRenderGraphResource>;

		auto inline getPassByID(uint32_t id) ->std::shared_ptr<IPass> { return passMap[id]; };
		auto inline getPassCount()->size_t { return passUids.size(); };

		auto getResourceType() const->FileType { return FileType::RenderGraph; };
		auto getPath() const->std::string { return path; };
		inline auto getRenderExtend() -> glm::vec2 { return renderExtend; };
		inline auto getOutputExtend() -> glm::vec2 { return outputExtend; };
		auto compileDependency() -> void;
		auto createResourceMap() -> void;
		inline auto getCommonData() -> CommonRenderData& { return m_commonData; }
		auto setOutputTexture(std::shared_ptr<Texture> texture) ->void { outputTexture = texture; };
	private:
		//Topological Sorting
		auto passSorting(std::vector<uint32_t> src) -> void;
	public:
		//
		std::shared_ptr<TextureDepth>                                   depthBuffer;
	private:	
		template<class T>
		auto addPass(uint32_t uid) -> std::shared_ptr<T>
		{
			static_assert(std::is_base_of<IPass, T>::value, "class T should extend from IPass");
			PLOGI("Add Task: {0}", typeid(T).name());
			passUids.push_back(uid);
			return std::static_pointer_cast<T>(passMap.emplace(uid, std::make_shared<T>(uid,this)).first->second);
		}

		std::string name;
		std::string path;

		std::vector<uint32_t> passUids;
		std::unordered_map<uint32_t,std::shared_ptr<IPass>> passMap;
		std::vector<std::shared_ptr<IRenderGraphResource>> resources;
	
		glm::vec2 renderExtend;
		glm::vec2 outputExtend;
		//set of group
		std::vector<PassGroup> groupSet;
		//common render data
		CommonRenderData m_commonData;
		
		//target output texture : set by application (offscreen/swapchain image)
		std::shared_ptr<Texture> outputTexture;
		auto getOutputTexture() -> std::shared_ptr<Texture> { return outputTexture; };
		friend class OutputPass;
	};

};