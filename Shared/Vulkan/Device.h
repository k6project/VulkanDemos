#pragma once

#include <vulkan.h>

namespace VK
{

    struct SwapchainConfig
    {
        VkSurfaceFormatKHR SurfaceFormat;
        VkPresentModeKHR PresentMode;
        VkExtent2D Extent;
    };

    class Instance;

    class CommandProxy;

    class Device
    {

    public:

        enum
        {
            GRAPHICS_QUEUE,
            PRESENT_QUEUE,
            TOTAL_QUEUES
        };

        bool InitSwapchain(const SwapchainConfig &config);

        void GetViewportArea(VkRect2D &outRect) const;

        std::uint32_t GetNextSwapchaninImage() const;

        VkAttachmentDescription GetColorAttachmentDescription() const;

        bool CreateCommandPool(int queueIndex, VkCommandPool &target);

        bool CreateCommandBuffers(VkCommandPool pool, std::vector<VkCommandBuffer> &target);

        bool CreateFramebuffers(VkRenderPass renderPass, std::vector<VkFramebuffer> &target);

        bool CreateRenderPass(const std::vector<VkAttachmentDescription> &attachments, const std::uint32_t *subpassAttachments, VkRenderPass &target);

        bool BeginCommandBuffer(VkCommandBuffer commandBuffer, CommandProxy &target);

        bool EndCommandBuffer(CommandProxy &proxy);

        void SubmitCommandBuffers(VkSubmitInfo &submitInfo, std::uint32_t imageIndex);

        void Destroy(std::vector<VkCommandBuffer> &objects, VkCommandPool pool);
        
        void Destroy(std::vector<VkFramebuffer> &objects);

        void Destroy(VkRenderPass object);

        void Destroy(VkCommandPool object);

        void Destroy();

    private:

        #define VULKAN_API_DEVICE(proc) PFN_##proc proc;
        #include "Device.inl"

        bool InitQueue(int index, std::uint32_t queueFamily);

        std::vector<VkSurfaceFormatKHR> SurfaceFormats;

        std::vector<VkPresentModeKHR> PresentModes;

        std::vector<VkImage> SwapchainImages;

        std::vector<VkImageView> SwapchainImageViews;

        VkSurfaceFormatKHR SwapchainSurfaceFormat;

        VkExtent2D SwapchainImageExtent;

        VkSurfaceCapabilitiesKHR SurfaceCapabilities;

        std::uint32_t QueueFamilies[TOTAL_QUEUES];

        VkSwapchainKHR Swapchain = VK_NULL_HANDLE;

        VkQueue Queues[TOTAL_QUEUES];
        
        VkSemaphore GraphicsSemaphore;

        VkSemaphore PresentSemaphore;

        VkSurfaceKHR Surface;

        VkDevice Handle;

        friend class VK::Instance;

        friend class VK::CommandProxy;

    };

}
