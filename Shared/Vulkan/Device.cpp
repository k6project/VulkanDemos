#include <Globals.h>

#include "Device.h"
#include "CommandProxy.h"

#include <algorithm>

bool VK::Device::InitSwapchain(const SwapchainConfig &config)
{
    if (Surface == VK_NULL_HANDLE)
    {
        return true; //Device without a surface requires no swap chain
    }
    VkExtent2D imageExtent;
    uint32_t imageCount = SurfaceCapabilities.minImageCount + 1;
    imageExtent.width = max(SurfaceCapabilities.minImageExtent.width, min(config.Extent.width, SurfaceCapabilities.maxImageExtent.width));
    imageExtent.height = max(SurfaceCapabilities.minImageExtent.height, min(config.Extent.height, SurfaceCapabilities.maxImageExtent.height));
    if (SurfaceCapabilities.maxImageCount > 0 && imageCount > SurfaceCapabilities.maxImageCount)
    {
        imageCount = SurfaceCapabilities.maxImageCount;
    }
    auto pmPos = std::find_if(PresentModes.begin(), PresentModes.end(), [&](const VkPresentModeKHR &mode)
    {
        return mode == config.PresentMode;
    });
    auto sfPos = std::find_if(SurfaceFormats.begin(), SurfaceFormats.end(), [&](const VkSurfaceFormatKHR &fmt)
    {
        return (fmt.colorSpace == config.SurfaceFormat.colorSpace) && (fmt.format == config.SurfaceFormat.format);
    });
    if (pmPos != PresentModes.end() && sfPos != SurfaceFormats.end())
    {
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = Surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = config.SurfaceFormat.format;
        createInfo.imageColorSpace = config.SurfaceFormat.colorSpace;
        createInfo.imageExtent = imageExtent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if (QueueFamilies[GRAPHICS_QUEUE] != QueueFamilies[PRESENT_QUEUE])
        {
            std::uint32_t families[] = { QueueFamilies[GRAPHICS_QUEUE], QueueFamilies[PRESENT_QUEUE] };
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = families;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }
        createInfo.preTransform = SurfaceCapabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = config.PresentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = Swapchain;
        if (vkCreateSwapchainKHR(Handle, &createInfo, nullptr, &Swapchain) == VK_SUCCESS)
        {
            std::uint32_t numImages = 0;
            vkGetSwapchainImagesKHR(Handle, Swapchain, &numImages, nullptr);
            SwapchainImages.resize(numImages);
            SwapchainImageViews.resize(numImages);
            vkGetSwapchainImagesKHR(Handle, Swapchain, &numImages, SwapchainImages.data());
            for (std::uint32_t i = 0; i < numImages; i++)
            {
                VkImageViewCreateInfo createInfo = {};
                createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                createInfo.image = SwapchainImages[i];
                createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                createInfo.format = config.SurfaceFormat.format;
                createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                createInfo.subresourceRange.baseMipLevel = 0;
                createInfo.subresourceRange.levelCount = 1;
                createInfo.subresourceRange.baseArrayLayer = 0;
                createInfo.subresourceRange.layerCount = 1;
                vkCreateImageView(Handle, &createInfo, nullptr, &SwapchainImageViews[i]);
            }
            SwapchainSurfaceFormat = config.SurfaceFormat;
            SwapchainImageExtent = imageExtent;
            VkSemaphoreCreateInfo semCreate = {};
            semCreate.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            vkCreateSemaphore(Handle, &semCreate, nullptr, &GraphicsSemaphore);
            vkCreateSemaphore(Handle, &semCreate, nullptr, &PresentSemaphore);
            return true;
        }
    }
    return false;
}

void VK::Device::GetViewportArea(VkRect2D &outRect) const
{
    outRect.offset = { 0, 0 };
    outRect.extent = SwapchainImageExtent;
}

std::uint32_t VK::Device::GetNextSwapchaninImage() const
{
    std::uint32_t result = VK_NONE;
    vkAcquireNextImageKHR(Handle, Swapchain, VK_NONE, GraphicsSemaphore, VK_NULL_HANDLE, &result);
    return result;
}

VkShaderModule VK::Device::GetShaderModule(const std::string &name)
{
    auto pos = ShaderModules.find(name);
    if (pos != ShaderModules.end())
    {
        return pos->second;
    }
    else
    {
        //Try to load module
    }
    return VK_NULL_HANDLE;
}

VkAttachmentDescription VK::Device::GetColorAttachmentDescription() const
{
    VkAttachmentDescription result = {};
    result.format = SwapchainSurfaceFormat.format;
    result.samples = VK_SAMPLE_COUNT_1_BIT;
    result.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    result.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    result.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    result.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    result.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    result.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    return result;
}

bool VK::Device::CreateCommandPool(int queueIndex, VkCommandPool &target)
{
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex = QueueFamilies[queueIndex];
    createInfo.flags = 0;
    return (vkCreateCommandPool(Handle, &createInfo, nullptr, &target) == VK_SUCCESS);
}

bool VK::Device::CreateCommandBuffers(VkCommandPool pool, std::vector<VkCommandBuffer> &target)
{
    bool result = false;
    target.resize(SwapchainImages.size());
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = target.size() & 0xffffffffu;
    if (vkAllocateCommandBuffers(Handle, &allocInfo, target.data()) == VK_SUCCESS)
    {
        result = true;
    }
    else
    {
        target.clear();
    }
    target.shrink_to_fit();
    return result;
}

bool VK::Device::CreateFramebuffers(VkRenderPass renderPass, std::vector<VkFramebuffer> &target)
{
    target.resize(SwapchainImages.size());
    for (std::uint32_t i = 0; i < SwapchainImages.size(); i++)
    {
        VkFramebufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = renderPass;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &SwapchainImageViews[i];
        createInfo.width = SwapchainImageExtent.width;
        createInfo.height = SwapchainImageExtent.height;
        createInfo.layers = 1;
        if (vkCreateFramebuffer(Handle, &createInfo, nullptr, &target[i]) != VK_SUCCESS)
        {
            target.clear();
            break;
        }
    }
    target.shrink_to_fit();
    return (target.size() == SwapchainImages.size());
}

bool VK::Device::CreateRenderPass(const std::vector<VkAttachmentDescription> &attachments, const std::uint32_t *subpassAttachments, VkRenderPass &target)
{
    bool result = false;
    std::vector<VkSubpassDescription> subpassData;
    std::uint32_t subpasses = subpassAttachments[0], i = 0;
    while (subpasses-- > 0)
    {
        subpassData.emplace_back();
        std::uint32_t numAttachments = subpassAttachments[++i];
        VkAttachmentReference *refs = new VkAttachmentReference[numAttachments];
        for (std::uint32_t j = 0; j < numAttachments; j++)
        {
            refs[j].attachment = subpassAttachments[++i];
            refs[j].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }
        subpassData.back().pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassData.back().colorAttachmentCount = numAttachments;
        subpassData.back().pColorAttachments = refs;
    }
    VkSubpassDependency subpassDependency = {};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = attachments.size() & 0xffffffffu;
    createInfo.pAttachments = attachments.data();
    createInfo.subpassCount = subpassAttachments[0];
    createInfo.pSubpasses = subpassData.data();
    createInfo.pDependencies = &subpassDependency;
    createInfo.dependencyCount = 1;
    if (vkCreateRenderPass(Handle, &createInfo, nullptr, &target) == VK_SUCCESS)
    {
        for (VkSubpassDescription &subpass : subpassData)
        {
            delete[] subpass.pColorAttachments;
        }
        result = true;
    }
    return result;
}

bool VK::Device::BeginCommandBuffer(VkCommandBuffer commandBuffer, CommandProxy &target)
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    beginInfo.pInheritanceInfo = nullptr;
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) == VK_SUCCESS)
    {
        target.CommandBuffer = commandBuffer;
        target.TargetDevice = this;
        return true;
    }
    target.CommandBuffer = VK_NULL_HANDLE;
    target.TargetDevice = nullptr;
    return false;
}

bool VK::Device::EndCommandBuffer(CommandProxy &proxy)
{
    bool result = (vkEndCommandBuffer(proxy.CommandBuffer) == VK_SUCCESS);
    proxy.CommandBuffer = VK_NULL_HANDLE;
    proxy.TargetDevice = nullptr;
    return result;
}

void VK::Device::SubmitCommandBuffers(VkSubmitInfo &submitInfo, std::uint32_t imageIndex)
{
    VkPipelineStageFlags flags[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.pWaitSemaphores = &GraphicsSemaphore;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitDstStageMask = flags;
    submitInfo.pSignalSemaphores = &PresentSemaphore;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    if (vkQueueSubmit(Queues[GRAPHICS_QUEUE], 1, &submitInfo, VK_NULL_HANDLE) == VK_SUCCESS)
    {
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pWaitSemaphores = &PresentSemaphore;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pSwapchains = &Swapchain;
        presentInfo.swapchainCount = 1;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;
        vkQueuePresentKHR(Queues[PRESENT_QUEUE], &presentInfo);
    }
}

void VK::Device::Destroy(std::vector<VkFramebuffer> &objects)
{
    for (VkFramebuffer object : objects)
    {
        vkDestroyFramebuffer(Handle, object, nullptr);
    }
}

bool VK::Device::InitQueue(int index, std::uint32_t queueFamily)
{
    vkGetDeviceQueue(Handle, queueFamily, 0, &Queues[index]);
    QueueFamilies[index] = queueFamily;
    return true;
}

void VK::Device::Destroy(VkCommandPool object)
{
    vkDeviceWaitIdle(Handle);
    vkDestroyCommandPool(Handle, object, nullptr);
}

void VK::Device::Destroy(VkRenderPass object)
{
    vkDeviceWaitIdle(Handle);
    vkDestroyRenderPass(Handle, object, nullptr);
}

void VK::Device::Destroy(std::vector<VkCommandBuffer> &objects, VkCommandPool pool)
{
    vkDeviceWaitIdle(Handle);
    vkFreeCommandBuffers(Handle, pool, objects.size() & 0xffffffffu, objects.data());
}

void VK::Device::Destroy()
{
    vkDeviceWaitIdle(Handle);
    vkDestroySemaphore(Handle, GraphicsSemaphore, nullptr);
    vkDestroySemaphore(Handle, PresentSemaphore, nullptr);
    for (VkImageView imageView : SwapchainImageViews)
    {
        vkDestroyImageView(Handle, imageView, nullptr);
    }
    vkDestroySwapchainKHR(Handle, Swapchain, nullptr);
    vkDestroyDevice(Handle, nullptr);
}
