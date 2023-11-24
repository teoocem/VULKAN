//
// Created by Administrator on 15.11.2023.
//

#ifndef IMBORED_VK_CORE_H
#define IMBORED_VK_CORE_H

#include<vector>
#include<iostream>
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

#include <optional>
#include<algorithm>

struct Indices {
    std::optional<unsigned int> graphicsFamilyIndice;
    std::optional<unsigned int> presentationFamilyIndice;

    bool isCompleted(){
        return graphicsFamilyIndice.has_value() && presentationFamilyIndice.has_value();
    }
};

class vk_core {
private:
    VkInstance instance;
    VkPhysicalDevice selectedDevice;
    VkDevice device;
    VkSurfaceKHR surface;
    GLFWwindow* main;
    std::vector<VkPhysicalDevice> physicalDevices;
    std::vector<VkQueueFamilyProperties> queueFamilies;
    VkPhysicalDeviceFeatures deviceFeatures;
    Indices indices;
    VkQueue graphicsQueue;
    VkQueue presentationQueue;




public:
    int init();
    vk_core();
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData) {

        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
    VkDebugUtilsMessengerEXT debugMessenger;


private:
    void createInstance(const char*);
    void createSurface();
    void pickPhysicalDevice();
    VkQueueFamilyProperties *getQueueFamilies();
    int scoreDevices();
    void createLogicalDevice();
    void setupDebugMessenger();




public:
   const VkDevice& getDevice() const;
   const VkPhysicalDevice& getSelectedDevice()const;
   const VkSurfaceKHR& getSurface()const;
    VkPresentModeKHR selectPresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    VkSurfaceFormatKHR selectSurfaceFormat(VkPhysicalDevice physicalDevice , VkSurfaceKHR surface);
    VkSurfaceCapabilitiesKHR getSurfaceCapabilities(VkPhysicalDevice physicalDevice,VkSurfaceKHR surface);
    VkExtent2D getSwapExtent();
   Indices& getIndices();
   VkQueue getGraphicsQueue()const;
   VkQueue getPresentationQueue()const;
   GLFWwindow* getMainWindow()const;


};




#endif //IMBORED_VK_CORE_H
