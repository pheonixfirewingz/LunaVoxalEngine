#include <renderer/vulkan/pipeline.h>
extern VkAllocationCallbacks callbacks;
namespace LunaVoxalEngine::Renderer
{
Pipeline::Pipeline(const GraphicsPipelineBuilder* builder)
    : type(PipelineType::GRAPHICS)
{
    VkGraphicsPipelineCreateInfo graphics = builder->build();
    vkCreateGraphicsPipelines(volkGetLoadedDevice(), VK_NULL_HANDLE, 1, &graphics, &callbacks, &pipeline);
}

Pipeline::Pipeline(const VkComputePipelineCreateInfo &compute)
    : type(PipelineType::COMPUTE)
{
    vkCreateComputePipelines(volkGetLoadedDevice(), VK_NULL_HANDLE, 1, &compute, &callbacks, &pipeline);
}

Pipeline::Pipeline(const VkRayTracingPipelineCreateInfoKHR &ray_tracing)
    : type(PipelineType::RAY_TRACING)
{
    vkCreateRayTracingPipelinesKHR(volkGetLoadedDevice(), VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &ray_tracing, &callbacks,
                                   &pipeline);
}

Pipeline::~Pipeline()
{
    vkDestroyPipeline(volkGetLoadedDevice(), pipeline, nullptr);
}
} // namespace LunaVoxalEngine::Renderer