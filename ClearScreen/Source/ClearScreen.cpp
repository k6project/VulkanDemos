#include <Globals.h>

#include <Platform/Application.h>
#include <VulkanAPI/Device.h>
#include <VulkanAPI/CommandProxy.h>

class ClearScreen : public Application
{

    enum SemaphoreID
    {
        IMAGE_READY, RENDER_DONE, TOTAL
    };

    VkSubmitInfo SubmitInfo = {};

    VkRenderPass RenderPass = VK_NULL_HANDLE;

    VkCommandPool CommandPool = VK_NULL_HANDLE;

    std::vector<VkCommandBuffer> CommandBuffers;

    std::vector<VkFramebuffer> RenderPassFramebuffers;

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

            //Create command buffers
            if (VKDevice->CreateCommandPool(VK::Device::GRAPHICS_QUEUE, CommandPool))
            {
                if (VKDevice->CreateCommandBuffers(CommandPool, CommandBuffers))
                {
                    for (std::uint32_t i = 0; i < CommandBuffers.size(); i++)
                    {
                        VK::CommandProxy cmd = {};
                        VkClearValue clearColor = { 0.f, 0.8f, 0.f, 1.f };
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
        VKDevice->Destroy(RenderPass);
        Application::Destroy();
    }

};

APP_MAIN(ClearScreen)
