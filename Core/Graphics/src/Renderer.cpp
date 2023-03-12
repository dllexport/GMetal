#include <FrameGraph.h>
#include <ImageResourceNode.h>
#include <Material.h>
#include <PipelineBuilder.h>
#include <RenderPassNode.h>
#include <Renderer.h>
#include <ResourceNode.h>
#include <spdlog/spdlog.h>
#include <DepthStencilImageResourceNode.h>
#include <UniformResourceNode.h>
#include <glm/glm.hpp>

Renderer::Renderer(IntrusivePtr<VulkanContext>& context) : context(context) {}

void Renderer::AddView(IntrusivePtr<IView>& view) {
    if (views.find(view) == views.end()) {
        views[view] = {};
    }
}

void Renderer::AddDrawable(IntrusivePtr<Drawable> drawable,
                           IntrusivePtr<IView>& view,
                           IntrusivePtr<FrameGraph> fg) {
    if (!views.count(view)) {
        spdlog::warn("Renderer::AddDrawable view not found in views");
        return;
    }

    auto rp = fg;
    if (!rp) {
        rp = defaultFg;
    }

    auto& dm = this->views[view];
    dm[rp].push_back(drawable);
}

void Renderer::StartFrame() {
    for (auto& [view, renderMap] : views) {
        auto swapChain = view->GetSwapChain();
        auto swapChainExtent = swapChain->Extent();
        auto swapChainImages = swapChain->GetImages();
        auto surfaceFormat = swapChain->GetSurfaceFormat();
        auto depthFormat = swapChain->DepthStencilFormat();

        auto fg = gmetal::make_intrusive<FrameGraph>(this->context, swapChain);

        struct MVPUniform {
            glm::mat4 model;
            glm::mat4 view;
            glm::mat4 projection;
        };

        auto uniform = fg->CreateNode<TypedUniformResourceNode<MVPUniform>>("uniform", MVPUniform{});

        auto gbufferPass = fg->CreatePass<RenderPassNode>("gbuffer pass");
        gbufferPass->Read(uniform, 0);

        gbufferPass->Setup([&](PipelineBuilder& builder) {
            return builder.SetVertexShader("shaders/gbuffer.vert.spv")
                    .SetFragmentShader("shaders/gbuffer.frag.spv")
                    /*.SetVertexInput({VertexComponent::Position,
                                     VertexComponent::Color,
                                     VertexComponent::Normal})*/
                    .SetRasterizer(VK_POLYGON_MODE_FILL,
                                   VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE)
                    .AddDescriptorSetLayoutBinding(
                            {PipelineBuilder::BuildDescriptorSetLayoutBinding(
                                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                    VK_SHADER_STAGE_VERTEX_BIT,
                                    0)})
                    .Build();
        });

        gbufferPass->Execute([&](VkCommandBuffer commandBuffer) { 
            vkCmdDraw(commandBuffer, 3, 1, 0, 0);
        });

        auto depth = fg->CreateAttachment<DepthStencilImageResourceNode>("depth", depthFormat);

        //auto normal = fg->CreateAttachment<ImageResourceNode>("normal", surfaceFormat.format);
        //auto albedo = fg->CreateAttachment<ImageResourceNode>("albedo", surfaceFormat.format);
        //auto position = fg->CreateAttachment<ImageResourceNode>("position", surfaceFormat.format);

        auto color = fg->CreateAttachment<ImageResourceNode>("color", swapChainImages);
        color->SetSwapChainImage();

        /*auto mergePass = fg->CreatePass<RenderPassNode>("merge pass");
        mergePass->Setup([&](PipelineBuilder& builder) {
            std::vector<VkDescriptorSetLayoutBinding> passBindings;
            passBindings.push_back(PipelineBuilder::BuildDescriptorSetLayoutBinding(
                    VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT, 0));
            passBindings.push_back(PipelineBuilder::BuildDescriptorSetLayoutBinding(
                    VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT, 1));
            passBindings.push_back(PipelineBuilder::BuildDescriptorSetLayoutBinding(
                    VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT, 2));

            return builder.SetVertexShader("shaders/merge.vert.spv")
                    .SetFragmentShader("shaders/merge.frag.spv")
                    .SetRasterizer(VK_POLYGON_MODE_FILL,
                                   VK_CULL_MODE_BACK_BIT,
                                   VK_FRONT_FACE_COUNTER_CLOCKWISE)
                    .AddDescriptorSetLayoutBinding(std::move(passBindings))
                    .Build();
        });

        mergePass->Execute([&](VkCommandBuffer commandBuffer) { 
            
        });*/

        gbufferPass->DepthStencilReadWrite(depth);
        //gbufferPass->Write(position, 0);
        //gbufferPass->Write(normal, 1);
        //gbufferPass->Write(albedo, 2);

        //mergePass->Read(position, 0);
        //mergePass->Read(normal, 1);
        //mergePass->Read(albedo, 2);
        //mergePass->DepthStencilReadWrite(depth);

        gbufferPass->Write(color, 0);

        color->Retain();

        fg->Compile();
        fg->ResolveResource();
        fg->Execute();
    }
}
