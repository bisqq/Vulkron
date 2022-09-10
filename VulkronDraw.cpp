
#include "VulkronInternal.h"

#include <thread>
#include <mutex>
#include <functional>
#include <queue>

// Source
// https://github.com/SaschaWillems/Vulkan/blob/master/base/threadpool.hpp

class VulrkonThread {
private:
    bool destroying = false;
    std::thread worker;
    std::queue<std::function<void()>> jobQueue;
    std::mutex queueMutex;
    std::condition_variable condition;

    // Loop through all remaining jobs
    void queueLoop() {
        while (true) {

            std::function<void()> job;

            {
                std::unique_lock<std::mutex> lock(queueMutex);
                condition.wait(lock, [this] { return !jobQueue.empty() || destroying; });
                if (destroying)
                {
                    break;
                }
                job = jobQueue.front();
            }

            job();

            {
                std::lock_guard<std::mutex> lock(queueMutex);
                jobQueue.pop();
                condition.notify_one();
            }
        }
    }

public:
    VulrkonThread() {
        worker = std::thread(&VulrkonThread::queueLoop, this);
    }

    ~VulrkonThread() {
        if (worker.joinable()) {
            wait();
            queueMutex.lock();
            destroying = true;
            condition.notify_one();
            queueMutex.unlock();
            worker.join();
        }
    }

    // Add a new job to the thread's queue
    void addJob(std::function<void()> function) {
        std::lock_guard<std::mutex> lock(queueMutex);
        jobQueue.push(std::move(function));
        condition.notify_one();
    }

    // Wait until all work items have been finished
    void wait() {
        std::unique_lock<std::mutex> lock(queueMutex);
        condition.wait(lock, [this]() { return jobQueue.empty(); });
    }
};

struct VulkronThreadPool {
    std::vector<std::unique_ptr<VulrkonThread>> threads;

    // Sets the number of threads to be allocated in this pool
    VulkronThreadPool() {
        threads.clear();
        for (auto i = 0; i < std::thread::hardware_concurrency(); i++) {
            threads.push_back(std::make_unique<VulrkonThread>());
        }
    }

    // Wait until all threads have finished their work items
    void wait() {
        for (auto& thread : threads) {
            thread->wait();
        }
    }
};

/*

    NOTE: If there are no draw commands, and empty buffers are being submitted. You'll get a validation error for a invalid presentable image

*/

DrawInternal* drawInternal = new DrawInternal();

const uint32_t                              MAX_FRAMES_IN_FLIGHT = 2;
VkCommandPool                               primaryCommandPool;
static const uint32_t                       NUMBER_OF_THREADS       = std::thread::hardware_concurrency();
static size_t                               currentFrame            = 0;
static std::vector<CommandBufferData>       drawData;
static std::vector<VulkronBaseObject>       tempStaticObjectsList;
static std::vector<VulkronBaseObject>       tempDynamicObjectsList;


static void createSyncObjects();
static void updateRendererCommandBuffers(uint32_t imageIndex);
static void updateStaticSecondaryCommandBuffers(VkCommandBufferInheritanceInfo inheritanceInfo, VkCommandBuffer staticBuffer, std::vector<VulkronBaseObject>& objectsList);
static void threadJobs(uint32_t imageIndex, std::vector<ThreadData> threadBufferList, std::vector<VulkronBaseObject> objectsList, VkCommandBufferInheritanceInfo inheritanceInfo);
static void resetFrameCommandPool(uint32_t imageIndex);
static void recreateSwapchain();

void destroyCommands() {

}

//-------------------------------------------------------------------------------------
// SECTION [DRAW] ---------------------------------------------------------------------
//-------------------------------------------------------------------------------------

static void recreateSwapchain() { // work on

    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(instance->pWindow, &width, &height);

    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(instance->pWindow, &width, &height);
        glfwWaitEvents();
    }

    uint32_t uwidth = static_cast<uint32_t>(width);
    uint32_t uheight = static_cast<uint32_t>(height);

    VulkronGraphicsCommands commands = {};
    commands.staticObjectlist = drawData.at(0).staticObjectsList;
    commands.dynamicObjectsList = drawData.at(0).dynamicObjectsList;

    vkDeviceWaitIdle(deviceInternal->logicalDevice);
    cleanUpSwapchain();

    createSwapchain(&uwidth, &uheight, swapchainInternal->vsync);
    createRenderPass(renderPassInternal->flag);
    createGraphicsPipeline();
    vulkronCreateRendererCommandBuffers(&commands);
}

void vulkronDrawFrame() {
    vkWaitForFences(deviceInternal->logicalDevice, 1, &drawInternal->inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(deviceInternal->logicalDevice, swapchainInternal->swapChain, UINT64_MAX, drawInternal->imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapchain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    resetFrameCommandPool(imageIndex);
    updateRendererCommandBuffers(imageIndex);

    if (drawInternal->imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(deviceInternal->logicalDevice, 1, &drawInternal->imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }

    drawInternal->imagesInFlight[imageIndex] = drawInternal->inFlightFences[currentFrame];
    
    VkSemaphore waitSemaphores[] = { drawInternal->imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signalSemaphores[] = { drawInternal->renderFinishedSemaphores[currentFrame] };

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = drawData.size();
    submitInfo.pCommandBuffers = &drawData.data()->primaryBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(deviceInternal->logicalDevice, 1, &drawInternal->inFlightFences[currentFrame]);

    if (vkQueueSubmit(queue->graphics, 1, &submitInfo, drawInternal->inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkSwapchainKHR swapChains[] = { swapchainInternal->swapChain };

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(queue->graphics, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        recreateSwapchain();
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

VulkronResult vulkronCreateRendererCommandBuffers(VulkronGraphicsCommands* info) {

    if (nullptr == info) {
        return VULKRON_ERROR_INVALID_ARGUMENT;
    }

    createSyncObjects();

    // create command pool for primary command buffers (non resetable)
    VkCommandPoolCreateInfo primaryCommandPoolInfo = {};
    primaryCommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    primaryCommandPoolInfo.queueFamilyIndex = deviceInternal->queuefamily.graphicsQueueIndex;

    if (vkCreateCommandPool(deviceInternal->logicalDevice, &primaryCommandPoolInfo, nullptr, &primaryCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool");
    }

    CommandBufferData* commandBufferData = new CommandBufferData();

    for (auto object : info->staticObjectlist) {
        commandBufferData->staticObjectsList.push_back(object);
    }

    for (auto object : info->dynamicObjectsList) {
        commandBufferData->dynamicObjectsList.push_back(object);
    }

    // create primary command buffers
    VkCommandBufferAllocateInfo commandbufferAllocate = {};
    commandbufferAllocate.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandbufferAllocate.commandPool = primaryCommandPool;
    commandbufferAllocate.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandbufferAllocate.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(deviceInternal->logicalDevice, &commandbufferAllocate, &commandBufferData->primaryBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffer!");
    }

    // create 1 static secondary command buffer
    commandbufferAllocate.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;

    if (vkAllocateCommandBuffers(deviceInternal->logicalDevice, &commandbufferAllocate, &commandBufferData->secondaryStaticBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate secondary command buffer!");
    }

    for (uint32_t threadIndex = 0; threadIndex < NUMBER_OF_THREADS; threadIndex++) {
        std::vector<ThreadData> tempThreadData;

        // for each thread, we have 3 command pools per thread per 3 frames, cycled in a ring buffer
        for (uint32_t i = 0; i < swapchainInternal->imageCount; i++) {
            ThreadData* threadData = new ThreadData();

            // create resetable command pool
            VkCommandPoolCreateInfo commandPoolInfo = {};
            commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            commandPoolInfo.queueFamilyIndex = deviceInternal->queuefamily.graphicsQueueIndex;

            if (vkCreateCommandPool(deviceInternal->logicalDevice, &commandPoolInfo, nullptr, &threadData->commandPool) != VK_SUCCESS) {
                throw std::runtime_error("failed to create command pool");
            }

            // create N disposable (dynamic) secondary command buffers
            VkCommandBufferAllocateInfo secondaryCommandBuffers = {};
            secondaryCommandBuffers.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            secondaryCommandBuffers.commandPool = threadData->commandPool;
            secondaryCommandBuffers.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
            secondaryCommandBuffers.commandBufferCount = commandBufferData->dynamicObjectsList.size(); // 1 buffer per dynamic object

            if (vkAllocateCommandBuffers(deviceInternal->logicalDevice, &secondaryCommandBuffers, threadData->secondarydynamicBufferList.data()) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate secondary command buffer!");
            }

            tempThreadData.push_back(*threadData);
        }

        commandBufferData->threadBuffersMap.insert(std::make_pair(threadIndex, tempThreadData));
    }

    // scene will always be index 0
    drawData.push_back(*commandBufferData);

    return VULKRON_SUCCESS;

}

static void updateRendererCommandBuffers(uint32_t imageIndex) {

    std::vector<VkCommandBuffer> executableCommandBuffers;
    CommandBufferData commandBuffers = drawData.at(0); // grab scene buffers

    // Test
    float flash = sin(imageIndex * 2) * 0.3 + 0.5;

    std::array<VkClearValue, 2> clearValues = {};
    clearValues[0].color = { {flash, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkCommandBufferBeginInfo commandBufferBegin = {};
    commandBufferBegin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffers.primaryBuffer, &commandBufferBegin) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = *pipeline->pRenderPass;
    renderPassInfo.renderArea.offset.x = 0;
    renderPassInfo.renderArea.offset.y = 0;
    renderPassInfo.renderArea.extent = swapchainInternal->swapChainExtent;
    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();
    renderPassInfo.framebuffer = swapchainInternal->bufferList[imageIndex].frameBuffer;

    vkCmdBeginRenderPass(commandBuffers.primaryBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

    VkCommandBufferInheritanceInfo inheritanceInfo = {};
    inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    inheritanceInfo.renderPass = *pipeline->pRenderPass;
    inheritanceInfo.framebuffer = swapchainInternal->bufferList[imageIndex].frameBuffer;

    if (!commandBuffers.staticObjectsList.empty()) {
        updateStaticSecondaryCommandBuffers(inheritanceInfo, commandBuffers.secondaryStaticBuffer, commandBuffers.staticObjectsList);
    }

    if (!commandBuffers.dynamicObjectsList.empty()) {
        VulkronThreadPool threadPool;

        for (uint32_t threadIndex = 0; threadIndex < NUMBER_OF_THREADS; threadIndex++) {
            threadPool.threads[threadIndex]->addJob([=] {
                threadJobs(imageIndex, commandBuffers.threadBuffersMap.at(threadIndex), commandBuffers.dynamicObjectsList, inheritanceInfo);
                });
        }

        threadPool.wait();

        // Submit if object is within the current view frustum
        for (auto& [threadIndex, threadList] : commandBuffers.threadBuffersMap) {
            
            // we grab the vector in that thread and return the object that is associated with the current frame
            std::vector<VkCommandBuffer> dynamicThreadBuffer;

            for (const auto& data : threadList) {
                if (data.imageIndex == imageIndex) {
                    dynamicThreadBuffer = data.secondarydynamicBufferList;
                }
            }

            // for every dynamic buffer we check if the object is visible, if true then the buffer is can be executed
            for (uint32_t j = 0; j < dynamicThreadBuffer.size(); j++) {

                // check the visibility of the object in every buffer
                if (commandBuffers.dynamicObjectsList.at(j).isVisible) {
                    executableCommandBuffers.push_back(dynamicThreadBuffer.at(j));
                }
            }
        }

        vkCmdExecuteCommands(commandBuffers.primaryBuffer, executableCommandBuffers.size(), executableCommandBuffers.data());
    }

    vkCmdEndRenderPass(commandBuffers.primaryBuffer);

    if (vkEndCommandBuffer(commandBuffers.primaryBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to execute commands!");
    }
}

static void updateStaticSecondaryCommandBuffers(VkCommandBufferInheritanceInfo inheritanceInfo, VkCommandBuffer staticBuffer, std::vector<VulkronBaseObject>& objectsList) {

    VkCommandBufferBeginInfo commandBufferBegin = {};
    commandBufferBegin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBegin.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    commandBufferBegin.pInheritanceInfo = &inheritanceInfo;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapchainInternal->swapChainExtent.width;
    viewport.height = (float)swapchainInternal->swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = swapchainInternal->swapChainExtent;

    if (vkBeginCommandBuffer(staticBuffer, &commandBufferBegin) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin command buffer!");
    }

    vkCmdSetViewport(staticBuffer, 0, 1, &viewport);
    vkCmdSetScissor(staticBuffer, 0, 1, &scissor);

    for (auto object : objectsList) {
        vkCmdBindPipeline(staticBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *object.pPipeline);
        
        // update static objects here
    }

    if (vkEndCommandBuffer(staticBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

static void threadJobs(uint32_t imageIndex, std::vector<ThreadData> threadBufferList, std::vector<VulkronBaseObject> objectsList, VkCommandBufferInheritanceInfo inheritanceInfo) {
    
    ThreadData threadData = threadBufferList.at(imageIndex);
    threadData.imageIndex = imageIndex;

    // for each buffer we update an object
    for (uint32_t i = 0; i < threadData.secondarydynamicBufferList.size(); i++) {

        VkCommandBufferBeginInfo commandBufferBegin = {};
        commandBufferBegin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBegin.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
        commandBufferBegin.pInheritanceInfo = &inheritanceInfo;

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swapchainInternal->swapChainExtent.width;
        viewport.height = (float)swapchainInternal->swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = swapchainInternal->swapChainExtent;

        VkCommandBuffer dynamicBuffer = threadData.secondarydynamicBufferList.at(i);

        if (vkBeginCommandBuffer(dynamicBuffer, &commandBufferBegin) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin command buffer!");
        }

        vkCmdSetViewport(dynamicBuffer, 0, 1, &viewport);
        vkCmdSetScissor(dynamicBuffer, 0, 1, &scissor);

        vkCmdBindPipeline(dynamicBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *objectsList.at(i).pPipeline);
        
        // update dynamic objects here

        if (vkEndCommandBuffer(dynamicBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

}

static void resetFrameCommandPool(uint32_t imageIndex) {
    // first index is always the scene
    auto sceneThreadMap = drawData.at(0).threadBuffersMap;

    for (const auto& [threadIndex, threadData] : sceneThreadMap) {
        for (auto data : threadData) {
            if (data.imageIndex == imageIndex) {
                vkResetCommandPool(deviceInternal->logicalDevice, data.commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
            }
        }
    }
}

static void createSyncObjects() {
    drawInternal->imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    drawInternal->renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    drawInternal->inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    drawInternal->imagesInFlight.resize(swapchainInternal->swapChainImagesList.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(deviceInternal->logicalDevice, &semaphoreInfo, nullptr, &drawInternal->imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(deviceInternal->logicalDevice, &semaphoreInfo, nullptr, &drawInternal->renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(deviceInternal->logicalDevice, &fenceInfo, nullptr, &drawInternal->inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

