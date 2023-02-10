#include <Renderer.h>
#include <Material.h>
#include <FrameGraph.h>
#include <RenderPassNode.h>
#include <ResourceNode.h>

#include <spdlog/spdlog.h>

void Renderer::AddView(IntrusivePtr<IView>& view) {
	if (views.find(view) == views.end()) {
		views[view] = {};
	}
}

void Renderer::SetDefaultRenderPass(IntrusivePtr<RenderPass> renderPass)
{
	this->defaultRenderPass = renderPass;
}

void Renderer::AddDrawable(IntrusivePtr<Drawable> drawable, IntrusivePtr<IView>& view, IntrusivePtr<RenderPass> renderPass)
{
	if (!views.count(view)) {
		spdlog::warn("Renderer::AddDrawable view not found in views");
		return;
	}

	auto rp = renderPass;
	if (!rp) {
		rp = defaultRenderPass;
	}

	auto& dm = this->views[view];
	dm[rp].push_back(drawable);
}

void Renderer::StartFrame()
{
	for (auto& [view, renderMap] : views)
	{
		FrameGraph fg;
		auto gbufferPass = fg.CreateNode<RenderPassNode>("gbuffer pass");
		auto fb = fg.CreateNode<ResourceNode>("fb");
		auto depth = fg.CreateNode<ResourceNode>("depth");
		auto color = fg.CreateNode<ResourceNode>("color");
		auto normal = fg.CreateNode<ResourceNode>("normal");
		auto position = fg.CreateNode<ResourceNode>("position");
		auto mergePass = fg.CreateNode<RenderPassNode>("merge pass");

		gbufferPass->Write(color);
		gbufferPass->Write(normal);
		gbufferPass->Write(position);

		mergePass->Read(color);
		mergePass->Read(normal);
		mergePass->Read(position);

		mergePass->Write(fb);

		fg.TopoSort();

		fg.Compile();

		for (auto& [rp, drawables] : renderMap)
		{
			BuildFrameCommandBuffer(view, rp, drawables);
		}
	}
}

void Renderer::BuildFrameCommandBuffer(const IntrusivePtr<IView>& view, const IntrusivePtr<RenderPass>& renderPass, std::vector<IntrusivePtr<Drawable>>& drawables)
{
	auto swapChain = view->GetSwapChain();
	auto swapChainExtent = swapChain->Extent();
	VkCommandBufferBeginInfo cbbi = {};
	cbbi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderPass->GetRenderPass();
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent.width = swapChainExtent.width;
	renderPassBeginInfo.renderArea.extent.height = swapChainExtent.height;
	renderPassBeginInfo.clearValueCount = renderPass->GetClearValue().size();
	renderPassBeginInfo.pClearValues = renderPass->GetClearValue().data();
	
	//for (int i = 0; i < swapChain->Size(); i++)
	//{
	//	// Set target frame buffer
	//	renderPassBeginInfo.framebuffer = frameBuffers[i];

	//	vkBeginCommandBuffer(drawCmdBuffers[i], &cbbi);

	//	vkCmdBeginRenderPass(drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	//	VkViewport viewport = { (float)swapChainExtent.width, (float)swapChainExtent.height, 0.0f, 1.0f };
	//	vkCmdSetViewport(drawCmdBuffers[i], 0, 1, &viewport);

	//	VkRect2D scissor = { (float)swapChainExtent.width, (float)swapChainExtent.height, 0, 0 };

	//	vkCmdSetScissor(drawCmdBuffers[i], 0, 1, &scissor);

	//	renderPass->Draw(drawCmdBuffers[i], drawables);

	//	vkCmdEndRenderPass(drawCmdBuffers[i]);

	//	vkEndCommandBuffer(drawCmdBuffers[i]);
	//}
}
