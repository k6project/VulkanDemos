#pragma once

#include <vulkan/vulkan.h>

namespace VK
{

    struct PipelineInfo;

    struct SwapchainConfig
    {
        VkSurfaceFormatKHR SurfaceFormat;
        VkPresentModeKHR PresentMode;
        VkExtent2D Extent;
    };

    class Instance;

    class Buffer;

    class CommandProxy;

    class Device
    {

    public:

        enum
        {
            GRAPHICS_QUEUE,
            PRESENT_QUEUE,
            //COMPUTE_QUEUE,
            //TRANSFER_QUEUE,
            TOTAL_QUEUES
        };

        bool InitSwapchain(const SwapchainConfig &config);

        void GetViewportArea(VkRect2D &outRect) const;

        VkViewport GetSwapchainViewport(std::uint32_t swapchainIndex = 0) const;

        VkRect2D GetSwapchainDefaultScissorRect(std::uint32_t swapchainIndex = 0) const;

        std::uint32_t GetNextSwapchaninImage(VkSemaphore waitSemaphore, std::uint32_t swapchainIndex = 0) const;

        VkShaderModule GetShaderModule(const std::string &name);

        VkAttachmentDescription GetColorAttachmentDescription() const;

        bool CreateCommandPool(int queueIndex, VkCommandPool &target);

        bool CreateSemaphores(std::vector<VkSemaphore> &target);

        bool CreateCommandBuffers(VkCommandPool pool, std::vector<VkCommandBuffer> &target);

        bool CreateFramebuffers(VkRenderPass renderPass, std::vector<VkFramebuffer> &target);

        bool CreateRenderPass(const std::vector<VkAttachmentDescription> &attachments, const std::uint32_t *subpassAttachments, VkRenderPass &target);

        bool CreateGraphicsPipeline(PipelineInfo &info, VkPipelineLayout &layout, VkPipeline &target);

        bool BeginCommandBuffer(VkCommandBuffer commandBuffer, CommandProxy &target);

        bool EndCommandBuffer(CommandProxy &proxy);

        bool SubmitCommands(const VkSubmitInfo *submitInfo, std::uint32_t count = 1);

        bool PresentFrame(VkSemaphore *waitSemaphores, std::uint32_t numSemaphores, std::uint32_t *images);

        void Destroy(std::vector<VkCommandBuffer> &objects, VkCommandPool pool);

        void Destroy(std::vector<VkSemaphore> &objects);
        
        void Destroy(std::vector<VkFramebuffer> &objects);

        void Destroy(Buffer &object);

        void Destroy(VkPipelineLayout object);

        void Destroy(VkPipeline object);

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

        std::unordered_map<std::string, VkShaderModule> ShaderModules;

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
