#ifndef PIPELINE_H
#define PIPELINE_H
#include <renderer/vulkan/ivulkan.h>
#include <utils/vector.h>
namespace LunaVoxelEngine::Renderer
{
enum class PipelineType
{
    GRAPHICS,
    COMPUTE,
    RAY_TRACING
};

class [[nodiscard]] GraphicsPipelineBuilder final
{
    Utils::Vector<VkPipelineShaderStageCreateInfo> shader_stages;
    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    VkPipelineInputAssemblyStateCreateInfo input_assembly{};
    VkPipelineViewportStateCreateInfo viewport_state{};
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    VkPipelineMultisampleStateCreateInfo multi_sampling{};
    VkPipelineDepthStencilStateCreateInfo depth_stencil{};
    VkPipelineColorBlendStateCreateInfo color_blending{};
    VkPipelineDynamicStateCreateInfo dynamic_state{};
    VkPipelineRenderingCreateInfo rendering_info{};
    VkPipelineLayout layout = VK_NULL_HANDLE;

  public:
    GraphicsPipelineBuilder()
    {
        static const VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT,
                                                       VK_DYNAMIC_STATE_SCISSOR,
                                                       VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE,
                                                       VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY,
                                                       VK_DYNAMIC_STATE_DEPTH_BIAS,
                                                       VK_DYNAMIC_STATE_BLEND_CONSTANTS};
        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = 6;
        dynamic_state.pDynamicStates = dynamic_states;

        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.scissorCount = 1;

        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.lineWidth = 1.0f;

        multi_sampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multi_sampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth_stencil.depthTestEnable = VK_TRUE;
        depth_stencil.depthWriteEnable = VK_TRUE;
        depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;

        color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.attachmentCount = 1;

        rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    }

    GraphicsPipelineBuilder &addShader(VkShaderStageFlagBits stage, VkShaderModule shader)
    {
        VkPipelineShaderStageCreateInfo shader_stage_info{};
        shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stage_info.stage = stage;
        shader_stage_info.module = shader;
        shader_stage_info.pName = "main";
        shader_stages.push_back(shader_stage_info);
        return *this;
    }

    GraphicsPipelineBuilder &setLayout(VkPipelineLayout layout_in)
    {
        layout = layout_in;
        return *this;
    }

    GraphicsPipelineBuilder &setRenderingInfo(VkPipelineRenderingCreateInfo &rendering_info_in)
    {
        rendering_info = rendering_info_in;
        return *this;
    }

    GraphicsPipelineBuilder &setVertexInputInfo(VkPipelineVertexInputStateCreateInfo &vertex_input_info_in)
    {
        vertex_input_info = vertex_input_info_in;
        return *this;
    }

    GraphicsPipelineBuilder &setInputAssembly(VkPrimitiveTopology topology, VkBool32 primitive_restart_enable)
    {
        VkPipelineInputAssemblyStateCreateInfo input_assembly_in{};
        input_assembly_in.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly_in.topology = topology;
        input_assembly_in.primitiveRestartEnable = primitive_restart_enable;
        input_assembly = input_assembly_in;
        return *this;
    }

    GraphicsPipelineBuilder &setViewportState(VkPipelineViewportStateCreateInfo &viewport_state_in)
    {
        viewport_state = viewport_state_in;
        return *this;
    }

    GraphicsPipelineBuilder &setRasterizer(VkPipelineRasterizationStateCreateInfo &rasterizer_in)
    {
        rasterizer = rasterizer_in;
        return *this;
    }

    GraphicsPipelineBuilder &setMultisampling(VkPipelineMultisampleStateCreateInfo &multi_sampling_in)
    {
        multi_sampling = multi_sampling_in;
        return *this;
    }

    GraphicsPipelineBuilder &setDepthStencil(VkPipelineDepthStencilStateCreateInfo &depth_stencil_in)
    {
        depth_stencil = depth_stencil_in;
        return *this;
    }

    GraphicsPipelineBuilder &setColorBlending(VkPipelineColorBlendStateCreateInfo &color_blending_in)
    {
        color_blending = color_blending_in;
        return *this;
    }

    GraphicsPipelineBuilder &setDynamicState(VkPipelineDynamicStateCreateInfo &dynamic_state_in)
    {
        dynamic_state = dynamic_state_in;
        return *this;
    }

    VkGraphicsPipelineCreateInfo build() const noexcept
    {
        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.stageCount = static_cast<uint32_t>(shader_stages.size());
        pipeline_info.pStages = shader_stages.data();
        pipeline_info.pVertexInputState = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer;
        pipeline_info.pMultisampleState = &multi_sampling;
        pipeline_info.pDepthStencilState = &depth_stencil;
        pipeline_info.pColorBlendState = &color_blending;
        pipeline_info.pDynamicState = &dynamic_state;
        pipeline_info.layout = layout;
        pipeline_info.pNext = &rendering_info;
        return pipeline_info;
    }
};

class [[nodiscard]] Pipeline final
{
  public:
    Pipeline(const GraphicsPipelineBuilder *graphics);
    Pipeline(const VkComputePipelineCreateInfo &compute);
    Pipeline(const VkRayTracingPipelineCreateInfoKHR &ray_tracing);
    ~Pipeline();

    [[nodiscard]] VkPipeline handle() const noexcept
    {
        return pipeline;
    }
    [[nodiscard]] PipelineType getType() const noexcept
    {
        return type;
    }

  private:
    VkPipeline pipeline;
    PipelineType type;
};
} // namespace LunaVoxelEngine::Renderer
#endif