#include <Globals.h>

#include "VKClearScreen.h"

#include "VulkanAPI/Device.h"
#include "VulkanAPI/CommandProxy.h"

struct VKClearScreen::ImplDetails
{
    VK::Device &Device;
    VkRenderPass RenderPass = VK_NULL_HANDLE;
    VkCommandPool CommandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> CommandBuffers;
    std::vector<VkFramebuffer> RenderPassFramebuffers;
    ImplDetails(VK::Device &device) : Device(device) {}

    void CreateRenderPass()
    {
        std::uint32_t subpassAttachments[] = { 1, 1, 0 };
        std::vector<VkAttachmentDescription> attachments(1);
        attachments[0] = Device.GetColorAttachmentDescription();
        if (Device.CreateRenderPass(attachments, subpassAttachments, RenderPass))
        {
            Device.CreateFramebuffers(RenderPass, RenderPassFramebuffers);
        }
    }

    void CreateCommandBuffers()
    {
        if (Device.CreateCommandPool(VK::Device::GRAPHICS_QUEUE, CommandPool))
        {
            if (Device.CreateCommandBuffers(CommandPool, CommandBuffers))
            {
                for (std::uint32_t i = 0; i < CommandBuffers.size(); i++)
                {
                    VK::CommandProxy cmd = {};
                    VkClearValue clearColor = {0.f, 0.8f, 0.f, 1.f};
                    if (Device.BeginCommandBuffer(CommandBuffers[i], cmd))
                    {
                        VkRenderPassBeginInfo renderPassInfo = {};
                        Device.GetViewportArea(renderPassInfo.renderArea);
                        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                        renderPassInfo.renderPass = RenderPass;
                        renderPassInfo.framebuffer = RenderPassFramebuffers[i];
                        renderPassInfo.clearValueCount = 1;
                        renderPassInfo.pClearValues = &clearColor;
                        cmd.BeginRenderPass(&renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
                        cmd.EndRenderPass();
                        Device.EndCommandBuffer(cmd);
                    }
                }
            }
        }
    }

    void SubmitNextFrame()
    {
        VkSubmitInfo submitInfo = {};
        std::uint32_t idx = Device.GetNextSwapchaninImage();
        submitInfo.pCommandBuffers = &CommandBuffers[idx];
        submitInfo.commandBufferCount = 1;
        Device.SubmitCommandBuffers(submitInfo, idx);
    }

    void DeleteCommandBuffers()
    {
        Device.Destroy(CommandBuffers, CommandPool);
        Device.Destroy(CommandPool);
    }

    void DeleteRenderPass()
    {
        Device.Destroy(RenderPassFramebuffers);
        Device.Destroy(RenderPass);
    }

};

void VKClearScreen::Init(VK::Device &device)
{
    Impl = new ImplDetails(device);
    Impl->CreateRenderPass();
    Impl->CreateCommandBuffers();
}

void VKClearScreen::Render(float)
{
    Impl->SubmitNextFrame();
}

void VKClearScreen::Shutdown()
{
    Impl->DeleteCommandBuffers();
    Impl->DeleteRenderPass();
    delete Impl;
}
