//
// Created by Administrator on 18.11.2023.
//

#include "vk_render.h"

int vk_render::init() {
    getSwapChainInfos(Core.getSelectedDevice(),Core.getSurface());
    createSwapchain(Core.getDevice(),Core.getSurface());
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFrameBuffers();
    createCommandPool();
    createCommandBuffer();
    createSyncObjects();

    return 0;
}

void vk_render::createSwapchain(VkDevice device,VkSurfaceKHR surface) {
   VkSwapchainCreateInfoKHR createInfo = {};
   createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
   createInfo.surface = surface;
   createInfo.clipped = VK_TRUE;
   createInfo.presentMode = swapChainInfos.presentMode;
   createInfo.imageExtent = swapChainInfos.extent;
   createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
   createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
   createInfo.preTransform = swapChainInfos.surfaceCapabilities.currentTransform;
   createInfo.imageFormat = swapChainInfos.surfaceFormat.format;
   createInfo.imageColorSpace = swapChainInfos.surfaceFormat.colorSpace;
   createInfo.minImageCount = swapChainInfos.surfaceCapabilities.minImageCount + 1;
   createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
   createInfo.imageArrayLayers = 1;
   createInfo.oldSwapchain = VK_NULL_HANDLE;

   if(vkCreateSwapchainKHR(device,&createInfo,nullptr,&swapChain) != VK_SUCCESS){
       throw std::runtime_error("failed to create swapchain!");
   }

    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(device, swapChain,&imageCount,swapChainImages.data());
    if(imageCount == 0)throw std::runtime_error("Failed to create swapchain!");
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device,swapChain,&imageCount,swapChainImages.data());


// Kontrol amaçlı log veya çıktı
    std::cout << "Swapchain image count: " << imageCount << std::endl;


}

vk_render::vk_render() {
   Core.init();
}

void vk_render::createImageViews(){
    std::cout << swapChainImages.size() << "\n";
    imageViews.resize(swapChainImages.size());
    int i = 0;
    for(const auto& image : swapChainImages){
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.format = swapChainInfos.surfaceFormat.format;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.components ={.r = VK_COMPONENT_SWIZZLE_IDENTITY,.g = VK_COMPONENT_SWIZZLE_IDENTITY,.b = VK_COMPONENT_SWIZZLE_IDENTITY,.a = VK_COMPONENT_SWIZZLE_IDENTITY};
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.layerCount = 1;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.image = image;

        if(vkCreateImageView(Core.getDevice(),&createInfo,nullptr,&imageViews[i]) != VK_SUCCESS){
            throw std::runtime_error("Failed to create image view!");
        }
        i++;
    }
    std::cout << "Image views created succesfully!\n";

}
void vk_render::createRenderPass() {
   VkAttachmentDescription colorAttachment = {};
   colorAttachment.format = swapChainInfos.surfaceFormat.format;
   colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
   colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

   colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
   colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

   colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
   colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

   colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

   VkAttachmentReference colorAttachmentRef = {};
   colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
   colorAttachmentRef.attachment = 0;

   VkSubpassDescription subpassDesc = {};
   subpassDesc.colorAttachmentCount = 1;
   subpassDesc.pColorAttachments = &colorAttachmentRef;

   VkRenderPassCreateInfo renderPassCreateInfo = {};
   renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
   renderPassCreateInfo.subpassCount = 1;
   renderPassCreateInfo.pSubpasses = &subpassDesc;
   renderPassCreateInfo.attachmentCount = 1;
   renderPassCreateInfo.pAttachments = &colorAttachment;

   VkSubpassDependency subpassDependency = {};
   subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
   subpassDependency.dstSubpass = 0;
   subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
   subpassDependency.srcAccessMask = 0;
   subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
   subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

   renderPassCreateInfo.pDependencies = &subpassDependency;
   renderPassCreateInfo.dependencyCount = 1;

    if(vkCreateRenderPass(Core.getDevice(),&renderPassCreateInfo,nullptr,&renderPass) != VK_SUCCESS){
        throw std::runtime_error("Failed to create render pass!");
    }
    std::cout << "Render pass created succesfully!\n";

}

SwapChainInfos vk_render::getSwapChainInfos(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
      swapChainInfos.surfaceFormat = Core.selectSurfaceFormat(physicalDevice,surface);
      swapChainInfos.presentMode = Core.selectPresentMode(physicalDevice,surface);
      swapChainInfos.surfaceCapabilities = Core.getSurfaceCapabilities(physicalDevice,surface);
      swapChainInfos.extent = Core.getSwapExtent();
      return swapChainInfos;
}

void vk_render::createGraphicsPipeline() {
    auto fragmentShader = readFile("../shaders/frag.spv");
    auto vertexShader = readFile("../shaders/vert.spv");

    VkShaderModule fragmentShaderModule = createShaderModule(fragmentShader);
    VkShaderModule vertexShaderModule = createShaderModule(vertexShader);

    VkPipelineShaderStageCreateInfo fragmentStage = {};
    fragmentStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentStage.module = fragmentShaderModule;
    fragmentStage.pName = "main";
    fragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentStage.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo vertexStage = {};
    vertexStage.pSpecializationInfo = nullptr;
    vertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexStage.pName = "main";
    vertexStage.module = vertexShaderModule;
    vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineShaderStageCreateInfo shaderModules[] {fragmentStage,vertexStage};


    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_SCISSOR,
                                                 VK_DYNAMIC_STATE_VIEWPORT};

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pDynamicStates = dynamicStates.data();
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());

    VkPipelineVertexInputStateCreateInfo vertexInputState = {};
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.pVertexBindingDescriptions = nullptr;
    vertexInputState.vertexBindingDescriptionCount = 0;
    vertexInputState.pVertexAttributeDescriptions = nullptr;
    vertexInputState.vertexAttributeDescriptionCount = 0;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

//    VkViewport viewport = {};
//    viewport.x = 0;
//    viewport.y = 0;
//    viewport.width = swapChainInfos.surfaceCapabilities.currentExtent.width;
//    viewport.height = swapChainInfos.surfaceCapabilities.currentExtent.height;
//    viewport.minDepth = 0.0f;
//    viewport.maxDepth = 1.0f;
//
//    VkRect2D scissor = {};
//    scissor.offset = {0,0};
//    scissor.extent = swapChainInfos.surfaceCapabilities.currentExtent;

    VkPipelineViewportStateCreateInfo viewPortState = {};
    viewPortState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewPortState.scissorCount = 1;
    viewPortState.viewportCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizerState = {};
    rasterizerState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerState.lineWidth = 1.0f;
    rasterizerState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizerState.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizerState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizerState.rasterizerDiscardEnable = VK_FALSE;
    rasterizerState.depthClampEnable = VK_FALSE;
    rasterizerState.depthBiasEnable = VK_FALSE;
    rasterizerState.depthBiasClamp = 0.0f;
    rasterizerState.depthBiasConstantFactor = 0.0f;
    rasterizerState.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multiSampleCreate = {};
    multiSampleCreate.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multiSampleCreate.alphaToCoverageEnable = VK_FALSE;
    multiSampleCreate.sampleShadingEnable = VK_FALSE;
    multiSampleCreate.alphaToOneEnable = VK_FALSE;
    multiSampleCreate.minSampleShading = 1.0f;
    multiSampleCreate.pSampleMask = nullptr;
    multiSampleCreate.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayout pipelineLayout;
    VkPipelineLayoutCreateInfo layoutCreateInfo = {};
    layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutCreateInfo.pPushConstantRanges = nullptr;
    layoutCreateInfo.pSetLayouts = nullptr;
    layoutCreateInfo.setLayoutCount = 0;
    layoutCreateInfo.pushConstantRangeCount = 0;

    if(vkCreatePipelineLayout(Core.getDevice(),&layoutCreateInfo,nullptr,&pipelineLayout) != VK_SUCCESS){
        throw std::runtime_error("Failed to create pipeline layout!");
    }
    std::cout << "Pipeline layout succesfully created!\n";


    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.pViewportState = &viewPortState;
    pipelineCreateInfo.pRasterizationState = &rasterizerState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = 0;
    pipelineCreateInfo.pColorBlendState = &colorBlending;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.pDepthStencilState = nullptr;
    pipelineCreateInfo.pMultisampleState = &multiSampleCreate;
    pipelineCreateInfo.pVertexInputState = &vertexInputState;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderModules;

    if(vkCreateGraphicsPipelines(Core.getDevice(),VK_NULL_HANDLE,1,&pipelineCreateInfo,nullptr,&graphicsPipeline) != VK_SUCCESS){
        throw std::runtime_error("Failed to create graphics pipeline!");
    }
    std::cout << "Graphics pipeline has created!\n";
    vkDestroyShaderModule(Core.getDevice(), fragmentShaderModule, nullptr);
    vkDestroyShaderModule(Core.getDevice(), vertexShaderModule, nullptr);


}

std::vector<char> vk_render::readFile(const std::string &fileName) {
   std::ifstream file(fileName,std::ios::ate | std::ios::binary);
   if(!file.is_open()){
       throw std::runtime_error("Failed to open a file!");
   }
   size_t fileSize = file.tellg();
   std::cout << fileSize<< "\n";
   std::vector<char> buffer(fileSize);

   file.seekg(0);
   file.read(buffer.data(),fileSize);
   file.close();

   return buffer;
}

VkShaderModule vk_render::createShaderModule(std::vector<char>& shader) {
    VkShaderModule shaderModule;
    VkShaderModuleCreateInfo shaderCreate = {};
    shaderCreate.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderCreate.codeSize = shader.size();
    shaderCreate.pCode = reinterpret_cast<uint32_t*>(shader.data());

    if(vkCreateShaderModule(Core.getDevice(),&shaderCreate,nullptr,&shaderModule)!= VK_SUCCESS){
    throw std::runtime_error("Failed to create shader module!");
}
    return shaderModule;

}

void vk_render::createFrameBuffers() {
    frameBuffers.resize(swapChainImages.size());
    for(size_t i = 0;i<swapChainImages.size();i++){
        VkFramebufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = renderPass;
        createInfo.attachmentCount = 1;
        createInfo.layers = 1;
        createInfo.width = Core.getSwapExtent().width;
        createInfo.height = Core.getSwapExtent().height;
        createInfo.pAttachments = &imageViews[i];

        if(vkCreateFramebuffer(Core.getDevice(),&createInfo,nullptr,&frameBuffers[i])!=VK_SUCCESS){
            throw std::runtime_error("Failed to create framebuffer!");
        }
    }
    std::cout << "Frame buffer objects created!\n";

}

void vk_render::createCommandPool() {
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex = Core.getIndices().graphicsFamilyIndice.value();
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    if(vkCreateCommandPool(Core.getDevice(),&createInfo,nullptr,&commandPool) != VK_SUCCESS){
        throw std::runtime_error("Failed to create command pool!");
    }
    std::cout << "Command pool creation succesful!\n";

}

void vk_render::createCommandBuffer() {
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.commandPool = commandPool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

    if(vkAllocateCommandBuffers(Core.getDevice(),&allocateInfo,&commandBuffer) != VK_SUCCESS){
        throw std::runtime_error("Failed to allocate command buffer!");
    }
    std::cout << "Command buffer allocation succesful!\n";

}

void vk_render::recordCommandBuffers(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo bufferBegin = {};
    bufferBegin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if(vkBeginCommandBuffer(commandBuffer,&bufferBegin) != VK_SUCCESS){
        throw std::runtime_error("Failed to begin command buffer!");
    }
    VkRenderPassBeginInfo renderPassBegin = {};
    renderPassBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBegin.renderPass = renderPass;
    renderPassBegin.framebuffer = frameBuffers[imageIndex];
    renderPassBegin.clearValueCount = 1;
    VkClearValue clearValue = {{{0.0,0.0,0.0,1.0}}};
    renderPassBegin.pClearValues = &clearValue;
    renderPassBegin.renderArea.offset = {0,0};
    renderPassBegin.renderArea.extent = swapChainInfos.extent;

    vkCmdBeginRenderPass(commandBuffer,&renderPassBegin,VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS,graphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChainInfos.extent.width);
    viewport.height = static_cast<float>(swapChainInfos.extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainInfos.extent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer,3,1,0,0);
    vkCmdEndRenderPass(commandBuffer);
    if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS){
        throw std::runtime_error("Failed to end commandbuffer!");
    }
}

void vk_render::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreCreate = {};
    semaphoreCreate.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceCreate= {};
    fenceCreate.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreate.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if(vkCreateSemaphore(Core.getDevice(),&semaphoreCreate,nullptr,&imageAvailableSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(Core.getDevice(),&semaphoreCreate,nullptr,&renderFinishedSemaphore) != VK_SUCCESS ||
            vkCreateFence(Core.getDevice(),&fenceCreate,nullptr,&inFlightFence) != VK_SUCCESS){
        throw std::runtime_error("Failed to create sync objects!");
    }
    std::cout << "Sync objects created!\n";


}

void vk_render::drawFrame() {
    vkWaitForFences(Core.getDevice(),1,&inFlightFence,VK_TRUE,UINT64_MAX);
    vkResetFences(Core.getDevice(),1,&inFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(Core.getDevice(),swapChain,UINT64_MAX,imageAvailableSemaphore,VK_NULL_HANDLE,&imageIndex);
    vkResetCommandBuffer(commandBuffer,0);
    recordCommandBuffers(commandBuffer,imageIndex);
    VkSubmitInfo submitInfo =  {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    std::vector<VkSemaphore> waitSemaphores = {imageAvailableSemaphore};
    std::vector<VkSemaphore> signalSemaphores = {renderFinishedSemaphore};
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pSignalSemaphores = signalSemaphores.data();
    submitInfo.waitSemaphoreCount =static_cast<uint32_t>( waitSemaphores.size());
    submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    std::vector<VkPipelineStageFlags> waitStageFlags = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };
    submitInfo.pWaitDstStageMask = waitStageFlags.data();

    if(vkQueueSubmit(Core.getGraphicsQueue(),1,&submitInfo,inFlightFence) != VK_SUCCESS){
        throw std::runtime_error("Failed to submit a queue!");
    }
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pSwapchains = &swapChain;
    presentInfo.pResults = nullptr;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores.data();
    presentInfo.swapchainCount = 1;
    presentInfo.pImageIndices = &imageIndex;

    if(vkQueuePresentKHR(Core.getPresentationQueue(),&presentInfo) != VK_SUCCESS){
        throw std::runtime_error("Failed to present image!");
    }



}

GLFWwindow *vk_render::getWindow()const {
    return Core.getMainWindow();
}
