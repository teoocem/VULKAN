//
// Created by Administrator on 15.11.2023.
//

#include "vk_core.h"


void vk_core::createInstance(const char* appName) {
  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = appName;
  appInfo.pEngineName = "Default Engine";
  appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
  appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
  appInfo.apiVersion = VK_API_VERSION_1_0;
;
  VkInstanceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;


  std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};
  createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
  createInfo.ppEnabledLayerNames = layers.data();

  unsigned int extensionCount = 0;
  const char** extensionNeeded = glfwGetRequiredInstanceExtensions(&extensionCount);
  std::vector<const char*> extensions(extensionNeeded,extensionNeeded + extensionCount);
  extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME
  );
  createInfo.ppEnabledExtensionNames = extensions.data();
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());

  if(vkCreateInstance(&createInfo,nullptr,&instance) != VK_SUCCESS){
      throw std::runtime_error("Failed to create an instance object!");
  }
  std::cout << "Instance object created!\n";

}

int vk_core::init() {
    std::cout << "Initialization get started..\n";
    createInstance("ImBored");
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    getQueueFamilies();
    createLogicalDevice();
    return EXIT_SUCCESS;
}

void vk_core::createSurface() {
    if(glfwCreateWindowSurface(instance,main,nullptr,&surface) != VK_SUCCESS){
        throw std::runtime_error("This physical device does not support surface");
    }
    std::cout << "Surface creation succesful!\n";
}

vk_core::vk_core() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);

    main = glfwCreateWindow(800,600,"Default", nullptr, nullptr);

}

void vk_core::pickPhysicalDevice() {
    unsigned int physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance,&physicalDeviceCount,nullptr);
    if(physicalDeviceCount == 0) throw std::runtime_error("There is no available device!");
    physicalDevices.resize(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance,&physicalDeviceCount,physicalDevices.data());

    if(physicalDeviceCount > 1){
        scoreDevices();
    }
    selectedDevice = physicalDevices[0];
    vkGetPhysicalDeviceFeatures(selectedDevice,&deviceFeatures);
}

int vk_core::scoreDevices() {
    return 0;
}

VkQueueFamilyProperties *vk_core::getQueueFamilies() {
    unsigned int queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(selectedDevice,&queueFamilyCount,nullptr);
    if(queueFamilyCount == 0) throw std::runtime_error("This device does not support queues!");
    queueFamilies.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(selectedDevice,&queueFamilyCount,queueFamilies.data());
    int i = 0;
    for(const auto& queueFamily : queueFamilies){
        if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indices.graphicsFamilyIndice = i;
        }
        VkBool32 presentationSupported = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(selectedDevice,i,surface,&presentationSupported);
        if(presentationSupported == VK_TRUE){
            indices.presentationFamilyIndice = i;
        }
        if(indices.isCompleted()){
            break;
        }

    }
    return queueFamilies.data();
}

void vk_core::createLogicalDevice() {
    std::vector<float> priorities {1.0f};
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.pQueuePriorities = priorities.data();
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamilyIndice.value();
    queueCreateInfo.queueCount = 1;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;

//    unsigned int deviceExtensionCount = 0;
//    vkEnumerateDeviceExtensionProperties(selectedDevice,nullptr,&deviceExtensionCount,nullptr);
//    if(deviceExtensionCount == 0) throw std::runtime_error("There is no available device extension!");
//    std::vector<VkExtensionProperties> extensionName(deviceExtensionCount);
//    vkEnumerateDeviceExtensionProperties(selectedDevice,nullptr,&deviceExtensionCount,extensionName.data());
//    for(const auto& extension : extensionName){
//        std::cout << extension.extensionName << "\n";
//    }
    std::vector<const char*> extensionNames {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    deviceCreateInfo.ppEnabledExtensionNames = extensionNames.data();
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>( extensionNames.size());

    if(vkCreateDevice(selectedDevice,&deviceCreateInfo,nullptr,&device) != VK_SUCCESS){
        throw std::runtime_error("Failed to create device!");
    }
    std::cout << "Device creation succesful!\n";

    vkGetDeviceQueue(device,indices.graphicsFamilyIndice.value(),0,&graphicsQueue);
    vkGetDeviceQueue(device,indices.presentationFamilyIndice.value(),0,&presentationQueue);


}

VkPresentModeKHR vk_core::selectPresentMode(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    unsigned int presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice,surface,&presentModeCount,nullptr);
    if(presentModeCount == 0)throw std::runtime_error("There is no available present mode in this device!");
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice,surface,&presentModeCount,presentModes.data());
    for(const auto& presentMode : presentModes){
        if(presentMode == VK_PRESENT_MODE_MAILBOX_KHR){
            return VK_PRESENT_MODE_MAILBOX_KHR;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR vk_core::selectSurfaceFormat(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    unsigned int formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice,surface,&formatCount,nullptr);
    if(formatCount == 0)throw std::runtime_error("There is no available surface format on this device!");
    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice,surface,&formatCount,surfaceFormats.data());
    for(const auto& s_Format : surfaceFormats){
        if(s_Format.format == VK_FORMAT_B8G8R8A8_SRGB && s_Format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR){
            return {
                .format = VK_FORMAT_R8G8B8A8_SRGB,
                .colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR
            };
        }
    }
}


VkSurfaceCapabilitiesKHR vk_core::getSurfaceCapabilities(VkPhysicalDevice physicalDevice,VkSurfaceKHR surface) {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice,surface,&surfaceCapabilities);
    std::cout << "Surface capabilities example (height) --> " << surfaceCapabilities.currentExtent.height << "\n";
    return surfaceCapabilities;
}

const VkDevice &vk_core::getDevice() const {
    return device;
}

const VkPhysicalDevice &vk_core::getSelectedDevice() const {
    return selectedDevice;
}

const VkSurfaceKHR &vk_core::getSurface() const {
    return surface;
}

Indices &vk_core::getIndices() {
    return indices;
}

VkExtent2D vk_core::getSwapExtent() {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(selectedDevice,surface,&capabilities);
    if(capabilities.currentExtent.width != UINT32_MAX){
        return capabilities.currentExtent;
    }
    else{
        int width,height;
        glfwGetFramebufferSize(main,&width,&height);
        VkExtent2D actualExtent = {
                .width = static_cast<uint32_t>(width),
                .height = static_cast<uint32_t>(height)
        };
        std::clamp(actualExtent.width,capabilities.minImageExtent.width,capabilities.maxImageExtent.width);
        std::clamp(actualExtent.height,capabilities.minImageExtent.height,capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

VkQueue vk_core::getGraphicsQueue() const {
    return graphicsQueue;
}

VkQueue vk_core::getPresentationQueue() const {
    return presentationQueue;
}

GLFWwindow *vk_core::getMainWindow() const {
    return main;
}
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
void vk_core::setupDebugMessenger() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}








