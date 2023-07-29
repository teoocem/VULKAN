
#ifndef UNTITLED_RENDERER_H
#define UNTITLED_RENDERER_H

#include<iostream>

#define GLFW_INCLUDE_VULKAN
#include<glfw3.h>

#include<vector>
#include <limits>
#include<cstring>
#include<fstream>
#include<set>
struct QueueFamilyIndices{
    unsigned int graphicsFamilyIndice;
    unsigned int presentationFamilyIndice;
    unsigned int computeFamilyIndice;
    unsigned int transferFamilyIndice;

};
class Renderer {

private:
    // COMPONENTS
    GLFWwindow* window;
    VkInstance instance;
    VkSurfaceKHR surface;
    QueueFamilyIndices indices;
    VkSwapchainKHR swapChain;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> imageViews;
    std::vector<VkFramebuffer> frameBuffers;
    VkRenderPass renderPass;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;

    struct {std::vector<VkSurfaceFormatKHR> formats;std::vector<VkPresentModeKHR>presentModes;VkSurfaceCapabilitiesKHR capabilities;} swapChainSupportInfos;

    struct mainDevices {VkDevice device; VkPhysicalDevice physicalDevice;} main_devices;
public:
    //CTORS
    Renderer(GLFWwindow* window) : window(window){};
    Renderer();


    int init();


    // Creation functions
    void createSurface();
    void createInstance();
    void createDevice();
    void createSwapChain();
    void createImageViews();
    void createGraphicsPipeline();
    VkShaderModule createShaderModule();


    //Getter functions
    std::vector<VkDeviceQueueCreateInfo> getQueueFamilies(VkPhysicalDevice);
    void getSwapChainImages();


    // Choose & Pick functions
    void pickPhysicalDevice();
    VkPresentModeKHR choosePresentMode(std::vector<VkPresentModeKHR> &presentModes) const;
    VkSurfaceFormatKHR chooseSurfaceFormat(std::vector<VkSurfaceFormatKHR> &formats) const;
    VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR)const;


public:
    //Checker Functions
    bool checkInstanceExtensionSupport(std::vector<const char*>&);
    bool checkDeviceExtensionSupport(std::vector<const char*>&);

    bool checkSwapChainSupport(VkPhysicalDevice device);


    std::vector<char> readFile(const char*);

    VkShaderModule createShaderModule(std::vector<char> &code);

    void createRenderPass();

    void createFrameBuffers();

    void createCommandPool();

    void createCommandBuffer();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
};


#endif //UNTITLED_RENDERER_H
