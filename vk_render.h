//
// Created by Administrator on 18.11.2023.
//

#ifndef IMBORED_VK_RENDER_H
#define IMBORED_VK_RENDER_H

#include<vulkan/vulkan.h>
#include<iostream>
#include<vector>
#include"vk_core.h"
#include<string>
#include<fstream>

struct SwapChainInfos{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    VkPresentModeKHR presentMode;
    VkSurfaceFormatKHR surfaceFormat;
    VkExtent2D extent;
};
class vk_render {
private:
    VkSwapchainKHR swapChain;
    VkRenderPass renderPass;
    SwapChainInfos swapChainInfos;

    std::vector<VkImageView> imageViews;
    std::vector<VkImage> swapChainImages;
    std::vector<VkFramebuffer> frameBuffers;
    vk_core Core;
    VkPipeline graphicsPipeline;
    VkCommandBuffer commandBuffer;
    VkCommandPool commandPool;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;


public:
    int init();
    vk_render();
    std::vector<char> readFile(const std::string& fileName);
    GLFWwindow* getWindow() const;
    void drawFrame();


private:
    SwapChainInfos getSwapChainInfos(VkPhysicalDevice physicalDevice,VkSurfaceKHR surface);

    void createSwapchain(VkDevice device,VkSurfaceKHR surface);
    void createRenderPass();
    void createGraphicsPipeline();
    void createFrameBuffers();
    void createCommandPool();
    void createCommandBuffer();
    void createSyncObjects();
    void recordCommandBuffers(VkCommandBuffer commandBuffer,uint32_t imageIndex);
    VkShaderModule createShaderModule(std::vector<char>& shader);

    void createImageViews();
};


#endif //IMBORED_VK_RENDER_H
