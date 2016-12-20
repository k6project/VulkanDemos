#include <Globals.h>

#include <Platform/Application.h>
#include <VulkanAPI/Device.h>
#include <VulkanAPI/CommandProxy.h>
#include <VulkanAPI/Pipeline.h>

class FullScreenQuad : public Application
{

    enum SemaphoreID
    {
        IMAGE_READY, RENDER_DONE, TOTAL
    };

    VkPipeline Pipeline;

    VkSubmitInfo SubmitInfo = {};

    VkPipelineLayout PipelineLayout;

    VkRenderPass RenderPass = VK_NULL_HANDLE;

    VkCommandPool CommandPool = VK_NULL_HANDLE;

    std::vector<VkCommandBuffer> CommandBuffers;

    std::vector<VkFramebuffer> RenderPassFramebuffers;

    VkShaderModule VertexModule = VK_NULL_HANDLE, FragmentModule = VK_NULL_HANDLE;

protected:

    virtual bool Init(const NativeWindow &nativeWindow) override
    {
        if (Application::Init(nativeWindow))
        {
            SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            //Create semaphores
            if (InitSemaphores(SemaphoreID::TOTAL))
            {
                //Prepare submit info
                SubmitInfo.pWaitSemaphores = &Semaphores[IMAGE_READY];
                SubmitInfo.waitSemaphoreCount = 1;
                SubmitInfo.pSignalSemaphores = &Semaphores[RENDER_DONE];
                SubmitInfo.signalSemaphoreCount = 1;
            }

            //Create render pass
            std::uint32_t subpassAttachments[] = { 1, 1, 0 };
            std::vector<VkAttachmentDescription> attachments(1);
            attachments[0] = VKDevice->GetColorAttachmentDescription();
            if (VKDevice->CreateRenderPass(attachments, subpassAttachments, RenderPass))
            {
                VKDevice->CreateFramebuffers(RenderPass, RenderPassFramebuffers);
            }

            VK::PipelineInfo pipelineInfo = VK::PipelineInfo::GetDefaults();

            std::string moduleName = "Shaders/NoVertices.vert.spv";
            VertexModule = VKDevice->GetShaderModule(moduleName);
            moduleName.replace(moduleName.find('.') + 1, 4, "frag");
            FragmentModule = VKDevice->GetShaderModule(moduleName);
            pipelineInfo.ShaderStages[VERTEX_STAGE].module = VertexModule;
            pipelineInfo.ShaderStages[FRAGMENT_STAGE].module = FragmentModule;

            pipelineInfo.InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

            VkViewport viewport = VKDevice->GetSwapchainViewport();
            VkRect2D scissor = VKDevice->GetSwapchainDefaultScissorRect();
            pipelineInfo.ViewportState.pViewports = &viewport;
            pipelineInfo.ViewportState.pScissors = &scissor;
            pipelineInfo.ViewportState.viewportCount = 1;
            pipelineInfo.ViewportState.scissorCount = 1;

            VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
            pipelineInfo.ColorBlending.attachmentCount = 1;
            pipelineInfo.ColorBlending.pAttachments = &colorBlendAttachment;

            pipelineInfo.CreateInfo.pDepthStencilState = nullptr;
            pipelineInfo.CreateInfo.pDynamicState = nullptr;
            pipelineInfo.CreateInfo.renderPass = RenderPass;
            pipelineInfo.CreateInfo.subpass = 0;

            VKDevice->CreateGraphicsPipeline(pipelineInfo, PipelineLayout, Pipeline);

            //Create command buffers
            if (VKDevice->CreateCommandPool(VK::Device::GRAPHICS_QUEUE, CommandPool))
            {
                if (VKDevice->CreateCommandBuffers(CommandPool, CommandBuffers))
                {
                    for (std::uint32_t i = 0; i < CommandBuffers.size(); i++)
                    {
                        VK::CommandProxy cmd = {};
                        VkClearValue clearColor = { 0.5f, 0.5f, 0.5f, 1.f };
                        if (VKDevice->BeginCommandBuffer(CommandBuffers[i], cmd))
                        {
                            VkRenderPassBeginInfo renderPassInfo = {};
                            VKDevice->GetViewportArea(renderPassInfo.renderArea);
                            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                            renderPassInfo.renderPass = RenderPass;
                            renderPassInfo.framebuffer = RenderPassFramebuffers[i];
                            renderPassInfo.clearValueCount = 1;
                            renderPassInfo.pClearValues = &clearColor;
                            cmd.BeginRenderPass(&renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
                            cmd.BindGraphicsPipeline(Pipeline);
                            cmd.Draw(6, 1, 0, 0);
                            cmd.EndRenderPass();
                            VKDevice->EndCommandBuffer(cmd);
                        }
                    }
                }
            }

            return true;
        }
        return false;
    }

    virtual void RenderFrame() override
    {
        std::uint32_t idx = VKDevice->GetNextSwapchaninImage(Semaphores[IMAGE_READY]);
        VkPipelineStageFlags flags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        SubmitInfo.pCommandBuffers = &CommandBuffers[idx];
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pWaitDstStageMask = flags;
        if (VKDevice->SubmitCommands(&SubmitInfo))
        {
            VKDevice->PresentFrame(&Semaphores[RENDER_DONE], 1, &idx);
        }
    }

    virtual void Destroy() override
    {
        VKDevice->Destroy(CommandBuffers, CommandPool);
        VKDevice->Destroy(CommandPool);
        VKDevice->Destroy(RenderPassFramebuffers);
        VKDevice->Destroy(Pipeline);
        VKDevice->Destroy(PipelineLayout);
        VKDevice->Destroy(RenderPass);
        Application::Destroy();
    }

};

APP_MAIN(FullScreenQuad)
