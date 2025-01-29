#ifndef VK_CMD_BUFFER_H
#define VK_CMD_BUFFER_H
#include <renderer/vulkan/buffer.h>
#include <renderer/vulkan/pipeline.h>
#include <renderer/vulkan/ivulkan.h>
namespace LunaVoxelEngine::Renderer
{

class [[nodiscard]] CommandBuffer final
{
  public:
    CommandBuffer(VkCommandPool commandPool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    // Prevent copying
    CommandBuffer(const CommandBuffer &) = delete;
    CommandBuffer &operator=(const CommandBuffer &) = delete;
    // Allow moving
    CommandBuffer(CommandBuffer &&other) noexcept
        : command_pool(other.command_pool)
        , command_buffer(other.command_buffer)
    {
        other.command_buffer = VK_NULL_HANDLE;
    }

    CommandBuffer &operator=(CommandBuffer &&other) noexcept
    {
        if (this != &other)
        {
            command_pool = other.command_pool;
            command_buffer = other.command_buffer;
            other.command_buffer = VK_NULL_HANDLE;
        }
        return *this;
    }
    ~CommandBuffer();
    // Core Command Buffer Operations
    void begin(const VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    void end();
    void reset(VkCommandBufferResetFlags flags);
    void executeCommands(uint32_t commandBufferCount, const VkCommandBuffer *pCommandBuffers);

    // Action Commands
    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
    void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset,
                     uint32_t firstInstance);
    void drawIndirect(Buffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride);
    void drawIndexedIndirect(Buffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride);
    void drawIndirectCount(Buffer buffer, VkDeviceSize offset, Buffer countBuffer, VkDeviceSize countBufferOffset,
                           uint32_t maxDrawCount, uint32_t stride);
    void drawIndexedIndirectCount(Buffer buffer, VkDeviceSize offset, Buffer countBuffer,
                                  VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride);
    void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);
    void dispatchIndirect(Buffer buffer, VkDeviceSize offset);
    void dispatchBase(uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ, uint32_t groupCountX,
                      uint32_t groupCountY, uint32_t groupCountZ);

    // Dynamic Rendering Core Commands
    void beginRendering(const VkRenderingInfo *pRenderingInfo);
    void endRendering();

    // Render Pass Commands
    void beginRenderPass(const VkRenderPassBeginInfo *pRenderPassBegin, VkSubpassContents contents);
    void nextSubpass(VkSubpassContents contents);
    void endRenderPass();
    void beginRenderPass2(const VkRenderPassBeginInfo *pRenderPassBegin, const VkSubpassBeginInfo *pSubpassBeginInfo);
    void nextSubpass2(const VkSubpassBeginInfo *pSubpassBeginInfo, const VkSubpassEndInfo *pSubpassEndInfo);
    void endRenderPass2(const VkSubpassEndInfo *pSubpassEndInfo);

    // Dynamic Rendering State Commands
    void setRenderingAttachmentLocations(const VkRenderingAttachmentLocationInfoKHR *pLocationInfo);
    void setRenderingInputAttachmentIndices(const VkRenderingInputAttachmentIndexInfoKHR *pIndexInfo);

    // Dynamic State Commands
    void setViewport(uint32_t firstViewport, uint32_t viewportCount, const VkViewport *pViewports);
    void setScissor(uint32_t firstScissor, uint32_t scissorCount, const VkRect2D *pScissors);
    void setLineWidth(float lineWidth);
    void setDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor);
    void setBlendConstants(const float blendConstants[4]);
    void setDepthBounds(float minDepthBounds, float maxDepthBounds);
    void setStencilCompareMask(VkStencilFaceFlags faceMask, uint32_t compareMask);
    void setStencilWriteMask(VkStencilFaceFlags faceMask, uint32_t writeMask);
    void setStencilReference(VkStencilFaceFlags faceMask, uint32_t reference);
    void setViewportWithCount(uint32_t viewportCount, const VkViewport *pViewports);
    void setScissorWithCount(uint32_t scissorCount, const VkRect2D *pScissors);
    void setDepthTestEnable(VkBool32 depthTestEnable);
    void setDepthWriteEnable(VkBool32 depthWriteEnable);
    void setDepthCompareOp(VkCompareOp depthCompareOp);
    void setDepthBoundsTestEnable(VkBool32 depthBoundsTestEnable);
    void setStencilTestEnable(VkBool32 stencilTestEnable);
    void setStencilOp(VkStencilFaceFlags faceMask, VkStencilOp failOp, VkStencilOp passOp, VkStencilOp depthFailOp,
                      VkCompareOp compareOp);
    void setRasterizerDiscardEnable(VkBool32 rasterizerDiscardEnable);
    void setDepthBiasEnable(VkBool32 depthBiasEnable);
    void setPrimitiveRestartEnable(VkBool32 primitiveRestartEnable);
    void setCullMode(VkCullModeFlags cullMode);
    void setFrontFace(VkFrontFace frontFace);
    void setPrimitiveTopology(VkPrimitiveTopology primitiveTopology);

    // Resource Binding Commands
    void bindPipeline(VkPipelineBindPoint pipelineBindPoint, const Pipeline* pipeline);
    void bindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet,
                            uint32_t descriptorSetCount, const VkDescriptorSet *pDescriptorSets,
                            uint32_t dynamicOffsetCount, const uint32_t *pDynamicOffsets);
    void bindVertexBuffer(uint32_t firstBinding, const Buffer &buffer, VkDeviceSize offset);
    void bindIndexBuffer(Buffer buffer, VkDeviceSize offset, VkIndexType indexType);
    void pushConstants(VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size,
                       const void *pValues);

    // Memory Barriers
    void pipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                         VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount,
                         const VkMemoryBarrier *pMemoryBarriers, uint32_t bufferMemoryBarrierCount,
                         const VkBufferMemoryBarrier *pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount,
                         const VkImageMemoryBarrier *pImageMemoryBarriers);
    void pipelineBarrier2(const VkDependencyInfo *pDependencyInfo);

    // Query Commands
    void beginQuery(VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags);
    void endQuery(VkQueryPool queryPool, uint32_t query);
    void resetQueryPool(VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount);
    void writeTimestamp(VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query);
    void writeTimestamp2(VkPipelineStageFlags2 stage, VkQueryPool queryPool, uint32_t query);

    // Clear Commands
    void clearAttachments(uint32_t attachmentCount, const VkClearAttachment *pAttachments, uint32_t rectCount,
                          const VkClearRect *pRects);
    void clearColorImage(VkImage image, VkImageLayout imageLayout, const VkClearColorValue *pColor, uint32_t rangeCount,
                         const VkImageSubresourceRange *pRanges);
    void clearDepthStencilImage(VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue *pDepthStencil,
                                uint32_t rangeCount, const VkImageSubresourceRange *pRanges);

    // Copy Commands
    void copyBuffer(Buffer srcBuffer, Buffer dstBuffer, uint32_t regionCount, const VkBufferCopy *pRegions);
    void copyBuffer2(const VkCopyBufferInfo2 *pCopyBufferInfo);
    void copyImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout,
                   uint32_t regionCount, const VkImageCopy *pRegions);
    void copyImage2(const VkCopyImageInfo2 *pCopyImageInfo);
    void copyBufferToImage(Buffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount,
                           const VkBufferImageCopy *pRegions);
    void copyBufferToImage2(const VkCopyBufferToImageInfo2 *pCopyBufferToImageInfo);
    void copyImageToBuffer(VkImage srcImage, VkImageLayout srcImageLayout, Buffer dstBuffer, uint32_t regionCount,
                           const VkBufferImageCopy *pRegions);
    void copyImageToBuffer2(const VkCopyImageToBufferInfo2 *pCopyImageToBufferInfo);

    // Debug Commands
    void beginDebugUtilsLabelEXT(const VkDebugUtilsLabelEXT *pLabelInfo);
    void endDebugUtilsLabelEXT();
    void insertDebugUtilsLabelEXT(const VkDebugUtilsLabelEXT *pLabelInfo);

    VkCommandBuffer handle() const { return command_buffer; }

  private:
    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;
};
} // namespace LunaVoxelEngine::Renderer
#endif