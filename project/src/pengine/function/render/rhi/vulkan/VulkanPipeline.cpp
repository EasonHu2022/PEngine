#include "VulkanPipeline.h"
#include "function/render/rhi/FrameBuffer.h"
#include "function/render/rhi/CommandBuffer.h"
#include "VulkanShader.h"
#include "VulkanRenderPass.h"
#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "Application.h"
#include "VulkanSwapChain.h"
#include "VulkanCommandBuffer.h"
#include <function/render/rhi/vulkan/VulkanTexture.h>
#include <stdexcept>
namespace pengine
{
	namespace
	{
		inline auto convertCompareOp(StencilType type)
		{
			switch (type)
			{
			case pengine::StencilType::Equal:
				return VK_COMPARE_OP_EQUAL;
			case pengine::StencilType::Notequal:
				return VK_COMPARE_OP_NOT_EQUAL;
			case pengine::StencilType::Always:
				return VK_COMPARE_OP_ALWAYS;
			case StencilType::Never:
				return VK_COMPARE_OP_NEVER;
			case StencilType::Less:
				return VK_COMPARE_OP_LESS;
			case StencilType::LessOrEqual:
				return VK_COMPARE_OP_LESS_OR_EQUAL;
			case StencilType::Greater:
				return VK_COMPARE_OP_GREATER;
			case StencilType::GreaterOrEqual:
				return VK_COMPARE_OP_GREATER_OR_EQUAL;
			case pengine::StencilType::Keep:
			case pengine::StencilType::Replace:
			case pengine::StencilType::Zero:
				throw std::logic_error("Compare Op should not be Keep/Replace/Zero");
			}

			return VK_COMPARE_OP_LESS_OR_EQUAL;
		}
		inline auto convertStencilOp(StencilType type)
		{
			switch (type)
			{
			case pengine::StencilType::Keep:
				return VK_STENCIL_OP_KEEP;
			case pengine::StencilType::Replace:
				return VK_STENCIL_OP_REPLACE;
			case pengine::StencilType::Zero:
				return VK_STENCIL_OP_ZERO;
			}
			throw std::logic_error("Compare Op should be Keep/Replace/Zero");
		}
		inline auto createDepthStencil(VkPipelineDepthStencilStateCreateInfo& ds, const PipelineInfo& info) -> void
		{
			ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			ds.pNext = NULL;
			ds.depthTestEnable = info.depthTest ? VK_TRUE : VK_FALSE;
			ds.depthWriteEnable = VK_TRUE;
			ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
			ds.depthBoundsTestEnable = VK_FALSE;
			ds.stencilTestEnable = info.stencilTest ? VK_TRUE : VK_FALSE;

			ds.back.failOp = convertStencilOp(info.stencilFail);
			ds.back.passOp = convertStencilOp(info.stencilDepthPass);
			ds.back.compareOp = convertCompareOp(info.stencilFunc);
			ds.back.compareMask = 0xFF;
			ds.back.reference = 1;
			ds.back.depthFailOp = convertStencilOp(info.stencilDepthFail);
			ds.back.writeMask = info.stencilMask;

			ds.minDepthBounds = 0;
			ds.maxDepthBounds = 0;
			ds.front = ds.back;
		}
		inline auto createMultisample(VkPipelineMultisampleStateCreateInfo& ms) -> void
		{
			ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			ms.pNext = NULL;
			ms.pSampleMask = NULL;
			ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			ms.sampleShadingEnable = VK_FALSE;
			ms.alphaToCoverageEnable = VK_FALSE;
			ms.alphaToOneEnable = VK_FALSE;
			ms.minSampleShading = 0.0;
		}
		inline auto createVertexLayout(VkVertexInputBindingDescription& vertexBindingDescription, VkPipelineVertexInputStateCreateInfo& vi, std::shared_ptr<VulkanShader> vkShader) -> void
		{
			vertexBindingDescription.binding = 0;
			vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			vertexBindingDescription.stride = vkShader->getVertexInputStride();

			auto& vertexInputAttributeDescription = vkShader->getVertexInputAttributeDescription();

			vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vi.pNext = NULL;
			vi.vertexBindingDescriptionCount = 1;
			vi.pVertexBindingDescriptions = &vertexBindingDescription;
			vi.vertexAttributeDescriptionCount = uint32_t(vertexInputAttributeDescription.size());
			vi.pVertexAttributeDescriptions = vertexInputAttributeDescription.data();
		}
		inline auto createRasterization(VkPipelineInputAssemblyStateCreateInfo& inputAssemblyCI, VkPipelineRasterizationStateCreateInfo& rs, const PipelineInfo& info) -> void
		{
			inputAssemblyCI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssemblyCI.pNext = NULL;
			inputAssemblyCI.primitiveRestartEnable = VK_FALSE;
			inputAssemblyCI.topology = VkConverter::drawTypeToTopology(info.drawType);

			rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rs.polygonMode = VkConverter::polygonModeToVk(info.polygonMode);
			rs.cullMode = VkConverter::cullModeToVk(info.cullMode);//VK_CULL_MODE_NONE;//VkConverter::cullModeToVk(info.cullMode);

			//TODO there is a bug here.
			//because vulkan Y axis is up to down and different with opengl
			//http://anki3d.org/vulkan-coordinate-system/
			rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;        //VK_FRONT_FACE_COUNTER_CLOCKWISE;//
			rs.depthClampEnable = VK_FALSE;
			rs.rasterizerDiscardEnable = VK_FALSE;
			rs.depthBiasEnable = (info.depthBiasEnabled ? VK_TRUE : VK_FALSE);
			rs.depthBiasConstantFactor = 0;
			rs.depthBiasClamp = 0;
			rs.depthBiasSlopeFactor = 0;
			rs.lineWidth = 1.0f;
			rs.pNext = NULL;
		}
		inline auto createColorBlend(VkPipelineColorBlendStateCreateInfo& cb, std::vector<VkPipelineColorBlendAttachmentState>& blendAttachState, const PipelineInfo& info, std::shared_ptr<VulkanRenderPass> renderPass) -> void
		{
			blendAttachState.resize(renderPass->getColorAttachmentCount());
			cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			cb.pNext = NULL;
			cb.flags = 0;
			for (unsigned int i = 0; i < blendAttachState.size(); i++)
			{
				blendAttachState[i] = VkPipelineColorBlendAttachmentState();
				blendAttachState[i].colorWriteMask = 0x0f;
				blendAttachState[i].alphaBlendOp = VK_BLEND_OP_ADD;
				blendAttachState[i].colorBlendOp = VK_BLEND_OP_ADD;

				if (info.transparencyEnabled)
				{
					blendAttachState[i].blendEnable = VK_TRUE;
					blendAttachState[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
					blendAttachState[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;

					if (info.blendMode == BlendMode::SrcAlphaOneMinusSrcAlpha)
					{
						blendAttachState[i].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
						blendAttachState[i].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
					}
					else if (info.blendMode == BlendMode::ZeroSrcColor)
					{
						blendAttachState[i].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
						blendAttachState[i].dstColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
					}
					else if (info.blendMode == BlendMode::OneZero)
					{
						blendAttachState[i].srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
						blendAttachState[i].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
					}
					else
					{
						blendAttachState[i].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
						blendAttachState[i].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
					}
				}
				else
				{
					blendAttachState[i].blendEnable = VK_FALSE;
					blendAttachState[i].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
					blendAttachState[i].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
					blendAttachState[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
					blendAttachState[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
				}
			}

			cb.attachmentCount = static_cast<uint32_t>(blendAttachState.size());
			cb.pAttachments = blendAttachState.data();
			cb.logicOpEnable = VK_FALSE;
			cb.logicOp = VK_LOGIC_OP_NO_OP;
			cb.blendConstants[0] = 1.0f;
			cb.blendConstants[1] = 1.0f;
			cb.blendConstants[2] = 1.0f;
			cb.blendConstants[3] = 1.0f;
		}
		inline auto createViewport(VkPipelineViewportStateCreateInfo& vp, std::vector<VkDynamicState>& dynamicState) -> void
		{
			vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			vp.pNext = NULL;
			vp.viewportCount = 1;
			vp.scissorCount = 1;
			vp.pScissors = NULL;
			vp.pViewports = NULL;
			dynamicState.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
			dynamicState.emplace_back(VK_DYNAMIC_STATE_SCISSOR);
		}
	};
	VulkanPipeline::VulkanPipeline(const PipelineInfo& info)
	{
		init(info);
	}
	VulkanPipeline::~VulkanPipeline()
	{
		auto& deletionQueue = VulkanContext::getDeletionQueue();
		auto  pipeline = this->pipeline;
		deletionQueue.emplace([pipeline] { vkDestroyPipeline(*VulkanDevice::get(), pipeline, VK_NULL_HANDLE); });
	}
	auto VulkanPipeline::init(const PipelineInfo& info) -> bool
	{
		shader = info.shader;
		auto vkShader = std::static_pointer_cast<VulkanShader>(info.shader);

		description = info;
		pipelineLayout = vkShader->getPipelineLayout();

		transitionAttachments();
		createFrameBuffers();

		// Pipeline
		std::vector<VkDynamicState>      dynamicStateDescriptors;
		VkPipelineDynamicStateCreateInfo dynamicStateCI{};
		dynamicStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCI.pNext = NULL;
		dynamicStateCI.pDynamicStates = dynamicStateDescriptors.data();

		// Vertex layout
		VkVertexInputBindingDescription      vertexBindingDescription{};
		VkPipelineVertexInputStateCreateInfo vi{};
		createVertexLayout(vertexBindingDescription, vi, vkShader);

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyCI{};
		VkPipelineRasterizationStateCreateInfo rs{};
		createRasterization(inputAssemblyCI, rs, info);

		VkPipelineColorBlendStateCreateInfo              cb{};
		std::vector<VkPipelineColorBlendAttachmentState> blendAttachState;
		createColorBlend(cb, blendAttachState, info, std::static_pointer_cast<VulkanRenderPass>(renderPass));

		VkPipelineViewportStateCreateInfo vp{};
		createViewport(vp, dynamicStateDescriptors);

		if (info.depthBiasEnabled)
		{
			dynamicStateDescriptors.emplace_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
			depthBiasConstant = 1.25f;
			depthBiasSlope = 1.75f;
			depthBiasEnabled = true;
		}
		else
		{
			depthBiasEnabled = false;
		}

		VkPipelineDepthStencilStateCreateInfo ds{};
		createDepthStencil(ds, info);
		VkPipelineMultisampleStateCreateInfo ms{};
		createMultisample(ms);

		dynamicStateCI.dynamicStateCount = uint32_t(dynamicStateDescriptors.size());
		dynamicStateCI.pDynamicStates = dynamicStateDescriptors.data();

		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
		graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsPipelineCreateInfo.pNext = NULL;
		graphicsPipelineCreateInfo.layout = pipelineLayout;
		graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		graphicsPipelineCreateInfo.basePipelineIndex = -1;
		graphicsPipelineCreateInfo.pVertexInputState = &vi;
		graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyCI;
		graphicsPipelineCreateInfo.pRasterizationState = &rs;
		graphicsPipelineCreateInfo.pColorBlendState = &cb;
		graphicsPipelineCreateInfo.pTessellationState = VK_NULL_HANDLE;
		graphicsPipelineCreateInfo.pMultisampleState = &ms;
		graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCI;
		graphicsPipelineCreateInfo.pViewportState = &vp;
		graphicsPipelineCreateInfo.pDepthStencilState = &ds;
		graphicsPipelineCreateInfo.pStages = vkShader->getShaderStages().data();
		graphicsPipelineCreateInfo.stageCount = vkShader->getShaderStages().size();
		graphicsPipelineCreateInfo.renderPass = *std::static_pointer_cast<VulkanRenderPass>(renderPass);
		graphicsPipelineCreateInfo.subpass = 0;

		VK_CHECK_RESULT(vkCreateGraphicsPipelines(*VulkanDevice::get(), VulkanDevice::get()->getPipelineCache(), 1, &graphicsPipelineCreateInfo, VK_NULL_HANDLE, &pipeline));

		return true;
	}
	auto VulkanPipeline::getWidth() -> uint32_t 
	{
		if (description.swapChainTarget)
		{
			return Application::getGraphicsContext()->getSwapChain()->getCurrentImage()->getWidth();
		}
		if (description.colorTargets[0])
			return description.colorTargets[0]->getWidth();

		if (description.depthTarget)
			return description.depthTarget->getWidth();

		if (description.depthArrayTarget)
			return description.depthArrayTarget->getWidth();

		PLOGE("Invalid pipeline width");

		return 0;
	}
	auto VulkanPipeline::getHeight() -> uint32_t 
	{
		if (description.swapChainTarget)
		{
			return Application::getGraphicsContext()->getSwapChain()->getCurrentImage()->getHeight();
		}
		if (description.colorTargets[0])
			return description.colorTargets[0]->getHeight();

		if (description.depthTarget)
			return description.depthTarget->getHeight();

		if (description.depthArrayTarget)
			return description.depthArrayTarget->getHeight();

		PLOGE("Invalid pipeline height");

		return 0;
	}
	auto VulkanPipeline::bind(CommandBuffer* commandBuffer, uint32_t layer, int32_t cubeFace, int32_t mipMapLevel) -> FrameBuffer* 
	{
		transitionAttachments();

		if (depthBiasEnabled)
			vkCmdSetDepthBias(static_cast<VulkanCommandBuffer*>(commandBuffer)->getCommandBuffer(), depthBiasConstant, 0.0f, depthBiasSlope);

		FrameBuffer* framebuffer = nullptr;

		if (description.swapChainTarget)
		{
			framebuffer = framebuffers[VulkanContext::get()->getSwapChain()->getCurrentImageIndex()].get();
		}
		else if (description.depthArrayTarget)
		{
			framebuffer = framebuffers[layer].get();
		}
		else
		{
			framebuffer = framebuffers[0].get();
		}

		auto mipScale = std::pow(0.5, mipMapLevel);

		renderPass->beginRenderPass(commandBuffer, description.clearColor, framebuffer, SubPassContents::Inline, getWidth() * mipScale, getHeight() * mipScale, cubeFace, mipMapLevel);
		vkCmdBindPipeline(static_cast<VulkanCommandBuffer*>(commandBuffer)->getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		return framebuffer;
	}
	auto VulkanPipeline::end(CommandBuffer* commandBuffer) -> void 
	{
		renderPass->endRenderPass(commandBuffer);
	}
	auto VulkanPipeline::clearRenderTargets(CommandBuffer* commandBuffer) -> void 
	{
		if (description.swapChainTarget)
		{
			for (uint32_t i = 0; i < Application::getGraphicsContext()->getSwapChain()->getSwapChainBufferCount(); i++)
			{
				Application::getRenderDevice()->clearRenderTarget(Application::getGraphicsContext()->getSwapChain()->getImage(i), commandBuffer);
			}
		}

		if (description.depthArrayTarget)
		{
			Application::getRenderDevice()->clearRenderTarget(description.depthArrayTarget, commandBuffer);
		}

		if (description.depthTarget)
		{
			Application::getRenderDevice()->clearRenderTarget(description.depthTarget, commandBuffer);
		}

		for (auto texture : description.colorTargets)
		{
			if (texture != nullptr)
			{
				Application::getRenderDevice()->clearRenderTarget(texture, commandBuffer);
			}
		}
	}
	auto VulkanPipeline::transitionAttachments() -> void
	{
		auto commandBuffer = static_cast<VulkanCommandBuffer*>(Application::getGraphicsContext()->getSwapChain()->getCurrentCommandBuffer());
		/*if (!commandBuffer->isRecording())
		{
			commandBuffer = nullptr;
		}*/

		if (description.swapChainTarget)
		{
			for (uint32_t i = 0; i < Application::getGraphicsContext()->getSwapChain()->getSwapChainBufferCount(); i++)
			{
				((VulkanTexture2D*)Application::getGraphicsContext()->getSwapChain()->getImage(i).get())->transitionImage(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, commandBuffer);
			}
		}

		if (description.depthArrayTarget)
		{
			((VulkanTextureDepthArray*)description.depthArrayTarget.get())->transitionImage(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, commandBuffer);
		}

		if (description.depthTarget)
		{
			((VulkanTextureDepth*)description.depthTarget.get())->transitionImage(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, commandBuffer);
		}

		for (auto texture : description.colorTargets)
		{
			if (texture != nullptr)
			{
				if (texture->getType() == TextureType::Color)
				{
					((VulkanTexture2D*)texture.get())->transitionImage(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, commandBuffer);
				}
				/*else if (texture->getType() == TextureType::Cube)
				{
					((VulkanTextureCube *) texture.get())->transitionImage(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, commandBuffer);
				}*/
			}
		}
	}
	auto VulkanPipeline::createFrameBuffers() -> void
	{
		std::vector<std::shared_ptr<Texture>> textures;

		if (description.swapChainTarget)
		{
			textures.emplace_back(Application::getGraphicsContext()->getSwapChain()->getImage(0));
		}
		else
		{
			for (auto texture : description.colorTargets)
			{
				if (texture)
				{
					textures.emplace_back(texture);
				}
			}
		}

		if (description.depthTarget)
		{
			textures.emplace_back(description.depthTarget);
		}

		if (description.depthArrayTarget)
		{
			textures.emplace_back(description.depthArrayTarget);
		}

		RenderPassInfo renderPassInfo;
		renderPassInfo.attachments = textures;
		renderPassInfo.clear = description.clearTargets;

		renderPass = RenderPass::create(renderPassInfo);

		FrameBufferInfo frameBufferInfo{};
		frameBufferInfo.width = getWidth();
		frameBufferInfo.height = getHeight();
		frameBufferInfo.renderPass = renderPass;
		frameBufferInfo.attachments = textures;

		if (description.swapChainTarget)
		{
			auto bufferCount = Application::getGraphicsContext()->getSwapChain()->getSwapChainBufferCount();

			for (uint32_t i = 0; i < bufferCount; i++)
			{
				//todo ...............
				frameBufferInfo.screenFBO = true;
				textures[0] = Application::getGraphicsContext()->getSwapChain()->getImage(i);
				frameBufferInfo.attachments = textures;
				framebuffers.emplace_back(FrameBuffer::create(frameBufferInfo));
			}
		}
		else if (description.depthArrayTarget)
		{
			auto depth = (VulkanTextureDepthArray*)description.depthArrayTarget.get();

			for (uint32_t i = 0; i < depth->getCount(); ++i)
			{
				frameBufferInfo.layer = i;
				frameBufferInfo.screenFBO = false;
				//auto indDepthArray = textures.size() - 1;
				//textures[indDepthArray] = description.depthArrayTarget;
				frameBufferInfo.attachments = textures;

				framebuffers.emplace_back(FrameBuffer::create(frameBufferInfo));
			}
		}
		else
		{
			frameBufferInfo.attachments = textures;
			frameBufferInfo.screenFBO = false;
			framebuffers.emplace_back(FrameBuffer::create(frameBufferInfo));
		}
	}
};