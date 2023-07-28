//
// Created by Administrator on 26.07.2023.
//

#include <map>
#include <filesystem>
#include "Renderer.h"

int Renderer::init() {
  try{
      createInstance();
      createSurface();
      pickPhysicalDevice();
      createDevice();
      createSwapChain();
      createImageViews();
      createGraphicsPipeline();
  }
  catch(std::runtime_error err){
      printf("%s",err.what());
  }

}

bool Renderer::checkInstanceExtensionSupport(std::vector<const char*>& vec) {
    unsigned int extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr,&extensionCount,nullptr);
    if(extensionCount == 0) throw std::runtime_error("There is no available instane extensions");
    std::vector<VkExtensionProperties> instanceExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr,&extensionCount,instanceExtensions.data());


    bool contains = false;
    for(const auto& requiredExt : vec){
        std::cout << requiredExt << "\n";
        contains = false;
        for(const auto& instanceExt : instanceExtensions){
            if(strcmp(requiredExt,instanceExt.extensionName) == 0){
                contains = true;
                break;
            }
        }
        if(!contains)throw std::runtime_error("This SDK do not contains this extensions...");

    }
    return contains;

}

Renderer::Renderer() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE,GLFW_FALSE);
//    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    window = glfwCreateWindow(800,600,"Default Window",nullptr,nullptr);
}

void Renderer::createInstance() {

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pEngineName = "NO ENGINE";
    appInfo.pApplicationName = "I hate VULKAN";
    appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    unsigned int requiredExtensionCount = 0;
    const char** extensionNames = glfwGetRequiredInstanceExtensions(&requiredExtensionCount);
    createInfo.enabledExtensionCount = requiredExtensionCount;
    createInfo.ppEnabledExtensionNames = extensionNames;
    std::vector<const char*> requiredExtensions;
    for(unsigned int i = 0;i<requiredExtensionCount;i++)requiredExtensions.push_back(extensionNames[i]);

    checkInstanceExtensionSupport(requiredExtensions);

    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.enabledLayerCount = 0;



    if(vkCreateInstance(&createInfo,nullptr,&instance) != VK_SUCCESS){
        throw std::runtime_error("Failed to create instance");
    }
    std::cout << "Instance created succesfully!\n";
}

void Renderer::createSurface() {
    if(glfwCreateWindowSurface(instance,window,nullptr,&surface) != VK_SUCCESS){
        throw std::runtime_error("Failed to create Surface");
    }
    std::cout << "Surface created succesfully ! \n";
}

void Renderer::pickPhysicalDevice() {
    unsigned int physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance,&physicalDeviceCount,nullptr);
    if(physicalDeviceCount == 0)throw std::runtime_error("There is no available GPU !");
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance,&physicalDeviceCount,physicalDevices.data());

    for(const auto& device : physicalDevices){
        // Burada görünen cihazlar arasında bir kıyaslama işlemi yapılabilir. Seçim için // TO-DO
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(device,&props);
        std::cout << props.deviceName << "\n";
    }
    main_devices.physicalDevice = physicalDevices[0];
}

void Renderer::createDevice() {
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;

    std::vector<const char*> requiredExt = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    deviceCreateInfo.ppEnabledExtensionNames = requiredExt.data();
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExt.size());

    std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos = getQueueFamilies(main_devices.physicalDevice);

    deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());

    VkPhysicalDeviceFeatures features = {};
    vkGetPhysicalDeviceFeatures(main_devices.physicalDevice,&features);
    deviceCreateInfo.pEnabledFeatures = &features;

    if(vkCreateDevice(main_devices.physicalDevice,&deviceCreateInfo,nullptr,&main_devices.device) != VK_SUCCESS){
        throw std::runtime_error("Failed to create device");
    }

    std::cout << "Device created succesfully!\n";

}

bool Renderer::checkDeviceExtensionSupport(std::vector<const char *> &){


}

std::vector<VkDeviceQueueCreateInfo> Renderer::getQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices = {};
    unsigned int queueFamiliesCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device,&queueFamiliesCount,nullptr);
    if(queueFamiliesCount==0)throw std::runtime_error("There is no available queue types in this physical device!");
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamiliesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device,&queueFamiliesCount,queueFamilies.data());


    std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
    int i = 0;
    for(const auto& queueFamily : queueFamilies){
        if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) indices.graphicsFamilyIndice = i;
        if(queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) indices.computeFamilyIndice = i;
        float prio = 1.0f;
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueCount = queueFamily.queueCount;
        queueCreateInfo.queueFamilyIndex = i;
        queueCreateInfo.pQueuePriorities = &prio;
        deviceQueueCreateInfos.push_back(queueCreateInfo);
        i++;
    }

    for(uint32_t i = 0;i<queueFamiliesCount;i++){
        VkBool32 presentationSupported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device,i,surface,&presentationSupported);
        if(presentationSupported == VK_TRUE) indices.presentationFamilyIndice = i; break;
    }

    this->indices = indices;

    return deviceQueueCreateInfos;


}

void Renderer::createSwapChain() {
    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = surface;
    std::cout << checkSwapChainSupport(main_devices.physicalDevice) << "\n";
    swapChainExtent = chooseSwapExtent(swapChainSupportInfos.capabilities);
    swapchainCreateInfo.presentMode = choosePresentMode(swapChainSupportInfos.presentModes);
    swapchainCreateInfo.imageExtent = swapChainExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;

    VkSurfaceFormatKHR format = chooseSurfaceFormat(swapChainSupportInfos.formats);
    swapChainImageFormat = format.format;
    swapchainCreateInfo.imageColorSpace = format.colorSpace;
    swapchainCreateInfo.imageFormat = format.format;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.minImageCount = swapChainSupportInfos.capabilities.minImageCount + 1;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.preTransform = swapChainSupportInfos.capabilities.currentTransform;

    if(indices.presentationFamilyIndice != indices.graphicsFamilyIndice){
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        uint32_t indice_arr[2] = {indices.graphicsFamilyIndice,indices.presentationFamilyIndice};
        swapchainCreateInfo.pQueueFamilyIndices = indice_arr;
    }
    else{
        swapchainCreateInfo.queueFamilyIndexCount = 1;
        uint32_t indice_arr[1] = {indices.presentationFamilyIndice};
        swapchainCreateInfo.pQueueFamilyIndices = indice_arr;
    }


    if(vkCreateSwapchainKHR(main_devices.device,&swapchainCreateInfo,nullptr,&swapChain) != VK_SUCCESS){
        throw std::runtime_error("Failed to create swapchain.");
    };
    std::cout << "Swap chain succesfully created!\n";


}

bool Renderer::checkSwapChainSupport(VkPhysicalDevice device){
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device,surface,&swapChainSupportInfos.capabilities);
    unsigned int surfaceFormatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&surfaceFormatCount,nullptr);
    if(surfaceFormatCount == 0)throw std::runtime_error("There is no available surface formats");
    swapChainSupportInfos.formats.resize(surfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device,surface,&surfaceFormatCount,swapChainSupportInfos.formats.data());

    unsigned int presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&presentModeCount,nullptr);
    if(presentModeCount == 0) throw std::runtime_error("There is no available present mode");
    swapChainSupportInfos.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device,surface,&presentModeCount,swapChainSupportInfos.presentModes.data());

    std::cout << "Swap Chain Infos collected --\n";
    return !(swapChainSupportInfos.formats.empty() && swapChainSupportInfos.presentModes.empty());
}

VkSurfaceFormatKHR Renderer::chooseSurfaceFormat(std::vector<VkSurfaceFormatKHR>& formats) const{
    for(const auto& format : formats){
        if(format.format == VK_FORMAT_R8G8B8A8_UNORM && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) return format;
    }
    return  swapChainSupportInfos.formats[0];
}

VkPresentModeKHR Renderer::choosePresentMode(std::vector<VkPresentModeKHR>& presentModes) const {
    for(const auto& presentMode : presentModes) if(presentMode == VK_PRESENT_MODE_MAILBOX_KHR) return presentMode;
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Renderer::chooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities) const {
    if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;
    else{
        int width , height;
        glfwGetFramebufferSize(window,&width,&height);
        VkExtent2D extent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
        };
        return extent;
    }

}

void Renderer::createImageViews() {
    getSwapChainImages();
    std::vector<VkImageView> imageViews(swapChainImages.size());

    for(uint32_t i = 0;i<imageViews.size();i++){
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.format = swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.image = swapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.layerCount = 1;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        if(vkCreateImageView(main_devices.device,&createInfo,nullptr,&imageViews[i]) != VK_SUCCESS){
            throw std::runtime_error("Failed to create swap chain image views.");
        }
    }
    std::cout << "Swap chain image views succesfully created!\n";

}

void Renderer::getSwapChainImages() {
    unsigned int swapchainImageCount = 0;
    vkGetSwapchainImagesKHR(main_devices.device,swapChain,&swapchainImageCount,nullptr);
    swapChainImages.resize(swapchainImageCount);
    vkGetSwapchainImagesKHR(main_devices.device,swapChain,&swapchainImageCount,swapChainImages.data());
    std::cout << "Swapchain image count : " << swapchainImageCount << "\n";

}

void Renderer::createGraphicsPipeline() {
    std::filesystem::path current_path = std::filesystem::current_path();
    std::cout << current_path << std::endl;
    auto fragmentShader = readFile("../shaders/frag.spv");
    auto vertexShader = readFile("../shaders/vert.spv");

    VkShaderModule fragmentModule = createShaderModule(fragmentShader);
    VkShaderModule vertexModule = createShaderModule(vertexShader);

    VkPipelineShaderStageCreateInfo fragShaderCreateInfo = {};
    fragShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderCreateInfo.module = fragmentModule;
    fragShaderCreateInfo.pName = "main";
    fragShaderCreateInfo.stage =  VK_SHADER_STAGE_FRAGMENT_BIT;

    VkPipelineShaderStageCreateInfo vertShaderCreateInfo = {};
    vertShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderCreateInfo.pName = "main";
    vertShaderCreateInfo.module = vertexModule;
    vertShaderCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineShaderStageCreateInfo shaderStageCreateInfos[2] = {fragShaderCreateInfo,vertShaderCreateInfo};

}

std::vector<char> Renderer::readFile(const char* filePath) {
    std::ifstream file(filePath,std::ios::ate | std::ios::binary);

    if(!file.is_open()){
        throw std::runtime_error("Failed to open this file.");
    }

    size_t bufferSize = static_cast<size_t>(file.tellg());
    std::vector<char> fileBuffer(bufferSize);
    file.seekg(0);
    file.read(fileBuffer.data(),bufferSize);

    return fileBuffer;
}

VkShaderModule Renderer::createShaderModule(std::vector<char>& code) {
    VkShaderModule shaderModule;
    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = code.size();
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
    if(vkCreateShaderModule(main_devices.device,&shaderModuleCreateInfo,nullptr,&shaderModule) != VK_SUCCESS){
        throw std::runtime_error("Failed to create shader module.");
    }

    return shaderModule;
}

