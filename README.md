![](https://badgen.net/badge/License/EULA/yellow?icon=github)
![](https://badgen.net/badge/Version/0.0.1/green?icon=github)

# Introduction
Vulkron is an abstraction of Vulkan in C++. As mentioned in the main [repository](https://github.com/bisqq/LegacyEngine) we use C as the foundation when commincating with the hardware and C++ as the main engine programming language for a higher level abstraction. Since Vulkan is already written in C for GPU communication we only need to write it in C++ for the API to successfully operate.

Vulkron is also going to be written in a way were we can export it as a dll/lib file (for future use).

## Current Setup
The structure works up to submitting command buffers, at the moment I am currently working on the buffers. Although you can set it up to get a red flickering screen.

### Required Libraries
- Vulkan
- Glfw
- GLM

### Code

```C++
#include "VulkronCore"
#include "Glfw/glfw3.h"

#include <Windows.h> // for console
#include <iostream>
#include <array>

static GLFWwindow* window;
static uint32_t width = 1280;
static uint32_t height = 720;

void updateViewPort();
void beginGlfw();
void windowLoop();
void shutdownGlfw();

VkVertexInputBindingDescription vulkronGetBindingDescription() {
    VkVertexInputBindingDescription VertexInputBindingDescription = {};

    VertexInputBindingDescription.binding = 0;
    VertexInputBindingDescription.stride = sizeof(VulkronVertex);
    VertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return VertexInputBindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> vulkronGetAttributeDescriptionNoTexture() {

    std::array<VkVertexInputAttributeDescription, 2> VertexInputAttributeDescriptions = {};

    VertexInputAttributeDescriptions[0].binding = 0;
    VertexInputAttributeDescriptions[0].location = 0;
    VertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    VertexInputAttributeDescriptions[0].offset = offsetof(VulkronVertex, position);

    VertexInputAttributeDescriptions[1].binding = 0;
    VertexInputAttributeDescriptions[1].location = 1;
    VertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    VertexInputAttributeDescriptions[1].offset = offsetof(VulkronVertex, color);

    return VertexInputAttributeDescriptions;
    
    int main() {
#if !_DEBUG || VULKRON_ENGINE_DEBUGGING
	auto myConsole = GetConsoleWindow();
	ShowWindow(myConsole, 0);
#endif

	beginGlfw();

	VkInstance instance;
	VulkronInstanceCreateInfo instanceCreate = {};
	instanceCreate.pInstance = &instance;
	instanceCreate.pWindow = window;

	if (vulkronCreateInstance(&instanceCreate) != VULKRON_SUCCESS) {
		throw std::runtime_error("failed to create vulkron instance!");
	}

	VulkronGpuFeatures features = {};
	auto gpuList = vulkronGetGpuDevicesList();

	vulkronGpuProperties();
	vulkronGpuFeatures();
	vulkronGpuSurfaceCapabilities();
	vulkronGpuSurfaceFormats();
	vulkronGpuSurfacePresentModes();
	vulkronGpuLimits();
	vulkronGpuQueueFamilyProperties();
	vulkronGpuMemoryProperties(false);

	VulkronDeviceCreateInfo deviceCreate = {};
	deviceCreate.gpuList = gpuList;
	deviceCreate.isUsingSwapchain = true;
	deviceCreate.gpuEnabledFeatures = features;
	deviceCreate.graphicsQueueFlag = VULKRON_QUEUE_GRAPHICS_BIT;
	deviceCreate.graphicsQueueCount = 1;


	if (vulkronCreateDevice(&deviceCreate) != VULKRON_SUCCESS) {
		throw std::runtime_error("failed to create vulkron device!");
	}

	VulkronSwapchainCreateInfo swap = {};
	swap.width = &width;
	swap.height = &height;
	swap.vsync = false;

	if (vulkronCreateSwapchain(&swap) != VULKRON_SUCCESS) {
		throw std::runtime_error("failed to create vulkron swapchain!");
	}

	auto vertexShader = vulkronCreatePipelineShaderStage("file/path/to/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	auto fragShader = vulkronCreatePipelineShaderStage("file/path/to/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
	std::vector<VkPipelineShaderStageCreateInfo> shaderStageList = { vertexShader, fragShader};

	float blendConstants[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	
	auto bindingDescription = vulkronGetBindingDescription();
	auto attributeDescriptions = vulkronGetAttributeDescriptionNoTexture();
	auto colorBlendAttachment = vulkronColorBlendAttchementState();
	std::vector<VkPipelineColorBlendAttachmentState> blendAttachment = { colorBlendAttachment };

	VulkronGraphicsPipeline pipelineData = {};
	pipelineData.pShaderStage = shaderStageList.data();
	pipelineData.shaderStageCount = shaderStageList.size();
	pipelineData.pVertexInputState = vulkronVertexInputState(1, &bindingDescription, static_cast<uint32_t>(attributeDescriptions.size()), attributeDescriptions.data());
	pipelineData.pInputAssemblyState = vulkronInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);
	pipelineData.pRasterizationState = vulkronRasterizationState(VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE, 1.0f);
	pipelineData.pMultisampleState = vulkronMultisampleState(VK_SAMPLE_COUNT_1_BIT, VK_FALSE);
	pipelineData.pColorBlendState = vulkronColorBlendState(blendAttachment.data(), blendAttachment.size(), VK_FALSE, blendConstants);
	pipelineData.pDynamicState = vulkronDynamicState(static_cast<uint32_t>(dynamicStates.size()), dynamicStates.data());
	pipelineData.pPipelineLayoutInfo = vulkronPipelineLayoutInfo();

	VkRenderPass renderPass;
	VkPipeline pipeline2D;
	VkPipelineLayout pipelineLayout;

	VulkronGraphicsPipelineCreateInfo piplineCreate = {};
	piplineCreate.flag = VULKRON_DEFAULT_ATTACHMENT;
	piplineCreate.pRenderPass = &renderPass;
	piplineCreate.pPipeline = &pipeline2D;
	piplineCreate.pPipelineLayout = &pipelineLayout;
	piplineCreate.pPipelineData = &pipelineData;

	if (vulkronCreateGraphicsPipeline(&piplineCreate) != VULKRON_SUCCESS) {
		throw std::runtime_error("failed to create vulkron pipeline!");
	}

	// no buffers yet so can't send any objects
	std::vector<VulkronBaseObject> staticObjectsList;
	std::vector<VulkronBaseObject> dynamicObjectsList;

	VulkronGraphicsCommands commands = {};
	commands.staticObjectlist = staticObjectsList;
	commands.dynamicObjectsList = dynamicObjectsList;

	if (vulkronCreateRendererCommandBuffers(&commands) != VULKRON_SUCCESS) {
		throw std::runtime_error("failed to create vulkron command buffers!");
	}

	windowLoop();
	shutdownGlfw();
}

void updateViewPort() {
	glfwSwapBuffers(window);
	glClear(GL_COLOR_BUFFER_BIT);
}

void onGlfwWindowResized(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	updateViewPort();
}

void windowRefreshFun(GLFWwindow* window) {
	updateViewPort();
}

void beginGlfw() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = glfwCreateWindow(width, height, "Legacy Engine", nullptr, nullptr);

	if (nullptr == window) {
		LOG("Failed to create GLFW window\n");
		shutdownGlfw();
		return;
	}

	glfwMakeContextCurrent(window);
	//glfwSwapInterval(1);

	glfwSetWindowRefreshCallback(window, windowRefreshFun);

	if (!glfwVulkanSupported()) {
		LOG("GLFW: Vulkan Not Supported\n");
		return;
	}
}

void windowLoop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		vulkronDrawFrame();
		updateViewPort();
	}
}

void shutdownGlfw() {
	glfwDestroyWindow(window);
	glfwTerminate();
}
}

```

