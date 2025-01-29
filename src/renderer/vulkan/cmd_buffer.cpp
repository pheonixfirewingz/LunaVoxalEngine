#include <platform/log.h>
#include <renderer/vulkan/cmd_buffer.h>
namespace LunaVoxelEngine::Renderer
{
CommandBuffer::CommandBuffer(VkCommandPool commandPool, VkCommandBufferLevel level)
{
}
CommandBuffer::~CommandBuffer()
{
}
void CommandBuffer::begin(const VkCommandBufferUsageFlags usage)
{
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = usage;

    vkBeginCommandBuffer(command_buffer, &begin_info);
}
void CommandBuffer::end()
{
    vkEndCommandBuffer(command_buffer);
}
void CommandBuffer::reset(VkCommandBufferResetFlags flags)
{
    vkResetCommandBuffer(command_buffer, flags);
}
void CommandBuffer::executeCommands(uint32_t commandBufferCount, const VkCommandBuffer *pCommandBuffers)
{
    vkCmdExecuteCommands(command_buffer, commandBufferCount, pCommandBuffers);
}
void CommandBuffer::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    vkCmdDraw(command_buffer, vertexCount, instanceCount, firstVertex, firstInstance);
}
void CommandBuffer::drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset,
                                uint32_t firstInstance)
{
    vkCmdDrawIndexed(command_buffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}
void CommandBuffer::drawIndirect(Buffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
    vkCmdDrawIndirect(command_buffer, buffer.buffer, offset, drawCount, stride);
}
void CommandBuffer::drawIndexedIndirect(Buffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
    vkCmdDrawIndexedIndirect(command_buffer, buffer.buffer, offset, drawCount, stride);
}
void CommandBuffer::drawIndirectCount(Buffer buffer, VkDeviceSize offset, Buffer countBuffer,
                                      VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride)
{
    vkCmdDrawIndirectCount(command_buffer, buffer.buffer, offset, countBuffer.buffer, countBufferOffset, maxDrawCount,
                           stride);
}
void CommandBuffer::drawIndexedIndirectCount(Buffer buffer, VkDeviceSize offset, Buffer countBuffer,
                                             VkDeviceSize countBufferOffset, uint32_t maxDrawCount, uint32_t stride)
{
    vkCmdDrawIndexedIndirectCount(command_buffer, buffer.buffer, offset, countBuffer.buffer, countBufferOffset,
                                  maxDrawCount, stride);
}
void CommandBuffer::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
    vkCmdDispatch(command_buffer, groupCountX, groupCountY, groupCountZ);
}
void CommandBuffer::dispatchIndirect(Buffer buffer, VkDeviceSize offset)
{
    vkCmdDispatchIndirect(command_buffer, buffer.buffer, offset);
}
void CommandBuffer::dispatchBase(uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ, uint32_t groupCountX,
                                 uint32_t groupCountY, uint32_t groupCountZ)
{
    vkCmdDispatchBase(command_buffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ);
}
void CommandBuffer::beginRendering(const VkRenderingInfo *pRenderingInfo)
{
    vkCmdBeginRendering(command_buffer, pRenderingInfo);
}
void CommandBuffer::endRendering()
{
    vkCmdEndRendering(command_buffer);
}
void CommandBuffer::beginRenderPass(const VkRenderPassBeginInfo *pRenderPassBegin, VkSubpassContents contents)
{
    vkCmdBeginRenderPass(command_buffer, pRenderPassBegin, contents);
}
void CommandBuffer::nextSubpass(VkSubpassContents contents)
{
    vkCmdNextSubpass(command_buffer, contents);
}
void CommandBuffer::endRenderPass()
{
    vkCmdEndRenderPass(command_buffer);
}
void CommandBuffer::beginRenderPass2(const VkRenderPassBeginInfo *pRenderPassBegin,
                                     const VkSubpassBeginInfo *pSubpassBeginInfo)
{
    vkCmdBeginRenderPass2(command_buffer, pRenderPassBegin, pSubpassBeginInfo);
}
void CommandBuffer::nextSubpass2(const VkSubpassBeginInfo *pSubpassBeginInfo, const VkSubpassEndInfo *pSubpassEndInfo)
{
    vkCmdNextSubpass2(command_buffer, pSubpassBeginInfo, pSubpassEndInfo);
}
void CommandBuffer::endRenderPass2(const VkSubpassEndInfo *pSubpassEndInfo)
{
    vkCmdEndRenderPass2(command_buffer, pSubpassEndInfo);
}
void CommandBuffer::setRenderingAttachmentLocations(const VkRenderingAttachmentLocationInfoKHR *pLocationInfo)
{
    vkCmdSetRenderingAttachmentLocations(command_buffer, pLocationInfo);
}
void CommandBuffer::setRenderingInputAttachmentIndices(const VkRenderingInputAttachmentIndexInfoKHR *pIndexInfo)
{
    vkCmdSetRenderingInputAttachmentIndices(command_buffer, pIndexInfo);
}
void CommandBuffer::setViewport(uint32_t firstViewport, uint32_t viewportCount, const VkViewport *pViewports)
{
    vkCmdSetViewport(command_buffer, firstViewport, viewportCount, pViewports);
}
void CommandBuffer::setScissor(uint32_t firstScissor, uint32_t scissorCount, const VkRect2D *pScissors)
{
    vkCmdSetScissor(command_buffer, firstScissor, scissorCount, pScissors);
}
void CommandBuffer::setLineWidth(float lineWidth)
{
    vkCmdSetLineWidth(command_buffer, lineWidth);
}
void CommandBuffer::setDepthBias(float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
    vkCmdSetDepthBias(command_buffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}
void CommandBuffer::setBlendConstants(const float blendConstants[4])
{
    vkCmdSetBlendConstants(command_buffer, blendConstants);
}
void CommandBuffer::setDepthBounds(float minDepthBounds, float maxDepthBounds)
{
    vkCmdSetDepthBounds(command_buffer, minDepthBounds, maxDepthBounds);
}
void CommandBuffer::setStencilCompareMask(VkStencilFaceFlags faceMask, uint32_t compareMask)
{
    vkCmdSetStencilCompareMask(command_buffer, faceMask, compareMask);
}
void CommandBuffer::setStencilWriteMask(VkStencilFaceFlags faceMask, uint32_t writeMask)
{
    vkCmdSetStencilWriteMask(command_buffer, faceMask, writeMask);
}
void CommandBuffer::setStencilReference(VkStencilFaceFlags faceMask, uint32_t reference)
{
    vkCmdSetStencilReference(command_buffer, faceMask, reference);
}
void CommandBuffer::setViewportWithCount(uint32_t viewportCount, const VkViewport *pViewports)
{
    vkCmdSetViewportWithCount(command_buffer, viewportCount, pViewports);
}
void CommandBuffer::setScissorWithCount(uint32_t scissorCount, const VkRect2D *pScissors)
{
    vkCmdSetScissorWithCount(command_buffer, scissorCount, pScissors);
}
void CommandBuffer::setDepthTestEnable(VkBool32 depthTestEnable)
{
    vkCmdSetDepthTestEnable(command_buffer, depthTestEnable);
}
void CommandBuffer::setDepthWriteEnable(VkBool32 depthWriteEnable)
{
    vkCmdSetDepthWriteEnable(command_buffer, depthWriteEnable);
}
void CommandBuffer::setDepthCompareOp(VkCompareOp depthCompareOp)
{
    vkCmdSetDepthCompareOp(command_buffer, depthCompareOp);
}
void CommandBuffer::setDepthBoundsTestEnable(VkBool32 depthBoundsTestEnable)
{
    vkCmdSetDepthBoundsTestEnable(command_buffer, depthBoundsTestEnable);
}
void CommandBuffer::setStencilTestEnable(VkBool32 stencilTestEnable)
{
    vkCmdSetStencilTestEnable(command_buffer, stencilTestEnable);
}
void CommandBuffer::setStencilOp(VkStencilFaceFlags faceMask, VkStencilOp failOp, VkStencilOp passOp,
                                 VkStencilOp depthFailOp, VkCompareOp compareOp)
{
    vkCmdSetStencilOp(command_buffer, faceMask, failOp, passOp, depthFailOp, compareOp);
}
void CommandBuffer::setRasterizerDiscardEnable(VkBool32 rasterizerDiscardEnable)
{
    vkCmdSetRasterizerDiscardEnable(command_buffer, rasterizerDiscardEnable);
}
void CommandBuffer::setDepthBiasEnable(VkBool32 depthBiasEnable)
{
    vkCmdSetDepthBiasEnable(command_buffer, depthBiasEnable);
}
void CommandBuffer::setPrimitiveRestartEnable(VkBool32 primitiveRestartEnable)
{
    vkCmdSetPrimitiveRestartEnable(command_buffer, primitiveRestartEnable);
}
void CommandBuffer::setCullMode(VkCullModeFlags cullMode)
{
    vkCmdSetCullMode(command_buffer, cullMode);
}
void CommandBuffer::setFrontFace(VkFrontFace frontFace)
{
    vkCmdSetFrontFace(command_buffer, frontFace);
}
void CommandBuffer::setPrimitiveTopology(VkPrimitiveTopology primitiveTopology)
{
    vkCmdSetPrimitiveTopology(command_buffer, primitiveTopology);
}
void CommandBuffer::bindPipeline(VkPipelineBindPoint pipelineBindPoint, const Pipeline* pipeline)
{
    vkCmdBindPipeline(command_buffer, pipelineBindPoint, pipeline->handle());
}
void CommandBuffer::bindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout,
                                       uint32_t firstSet, uint32_t descriptorSetCount,
                                       const VkDescriptorSet *pDescriptorSets, uint32_t dynamicOffsetCount,
                                       const uint32_t *pDynamicOffsets)
{
    vkCmdBindDescriptorSets(command_buffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets,
                            dynamicOffsetCount, pDynamicOffsets);
}
void CommandBuffer::bindVertexBuffer(uint32_t firstBinding, const Buffer &buffer, VkDeviceSize offset)
{
    VkBuffer buffers[] = {buffer.buffer};
    VkDeviceSize offsets[] = {offset};
    vkCmdBindVertexBuffers(command_buffer, firstBinding, 1, buffers, offsets);
}
void CommandBuffer::bindIndexBuffer(Buffer buffer, VkDeviceSize offset, VkIndexType indexType)
{
    vkCmdBindIndexBuffer(command_buffer, buffer.buffer, offset, indexType);
}
void CommandBuffer::pushConstants(VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset,
                                  uint32_t size, const void *pValues)
{
    vkCmdPushConstants(command_buffer, layout, stageFlags, offset, size, pValues);
}
void CommandBuffer::pipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                                    VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount,
                                    const VkMemoryBarrier *pMemoryBarriers, uint32_t bufferMemoryBarrierCount,
                                    const VkBufferMemoryBarrier *pBufferMemoryBarriers,
                                    uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier *pImageMemoryBarriers)
{
    vkCmdPipelineBarrier(command_buffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount,
                         pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount,
                         pImageMemoryBarriers);
}
void CommandBuffer::pipelineBarrier2(const VkDependencyInfo *pDependencyInfo)
{
    vkCmdPipelineBarrier2(command_buffer, pDependencyInfo);
}
void CommandBuffer::beginQuery(VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags)
{
    vkCmdBeginQuery(command_buffer, queryPool, query, flags);
}
void CommandBuffer::endQuery(VkQueryPool queryPool, uint32_t query)
{
    vkCmdEndQuery(command_buffer, queryPool, query);
}
void CommandBuffer::resetQueryPool(VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount)
{
    vkCmdResetQueryPool(command_buffer, queryPool, firstQuery, queryCount);
}
void CommandBuffer::writeTimestamp(VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query)
{
    vkCmdWriteTimestamp(command_buffer, pipelineStage, queryPool, query);
}
void CommandBuffer::writeTimestamp2(VkPipelineStageFlags2 stage, VkQueryPool queryPool, uint32_t query)
{
    vkCmdWriteTimestamp2(command_buffer, stage, queryPool, query);
}
void CommandBuffer::clearAttachments(uint32_t attachmentCount, const VkClearAttachment *pAttachments,
                                     uint32_t rectCount, const VkClearRect *pRects)
{
    vkCmdClearAttachments(command_buffer, attachmentCount, pAttachments, rectCount, pRects);
}
void CommandBuffer::clearColorImage(VkImage image, VkImageLayout imageLayout, const VkClearColorValue *pColor,
                                    uint32_t rangeCount, const VkImageSubresourceRange *pRanges)
{
    vkCmdClearColorImage(command_buffer, image, imageLayout, pColor, rangeCount, pRanges);
}
void CommandBuffer::clearDepthStencilImage(VkImage image, VkImageLayout imageLayout,
                                           const VkClearDepthStencilValue *pDepthStencil, uint32_t rangeCount,
                                           const VkImageSubresourceRange *pRanges)
{
    vkCmdClearDepthStencilImage(command_buffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
}
void CommandBuffer::copyBuffer(Buffer srcBuffer, Buffer dstBuffer, uint32_t regionCount, const VkBufferCopy *pRegions)
{
    vkCmdCopyBuffer(command_buffer, srcBuffer.buffer, dstBuffer.buffer, regionCount, pRegions);
}
void CommandBuffer::copyBuffer2(const VkCopyBufferInfo2 *pCopyBufferInfo)
{
    vkCmdCopyBuffer2(command_buffer, pCopyBufferInfo);
}
void CommandBuffer::copyImage(VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage,
                              VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy *pRegions)
{
    vkCmdCopyImage(command_buffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
}
void CommandBuffer::copyImage2(const VkCopyImageInfo2 *pCopyImageInfo)
{
    vkCmdCopyImage2(command_buffer, pCopyImageInfo);
}
void CommandBuffer::copyBufferToImage(Buffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout,
                                      uint32_t regionCount, const VkBufferImageCopy *pRegions)
{
    vkCmdCopyBufferToImage(command_buffer, srcBuffer.buffer, dstImage, dstImageLayout, regionCount, pRegions);
}
void CommandBuffer::copyBufferToImage2(const VkCopyBufferToImageInfo2 *pCopyBufferToImageInfo)
{
    vkCmdCopyBufferToImage2(command_buffer, pCopyBufferToImageInfo);
}
void CommandBuffer::copyImageToBuffer(VkImage srcImage, VkImageLayout srcImageLayout, Buffer dstBuffer,
                                      uint32_t regionCount, const VkBufferImageCopy *pRegions)
{
    vkCmdCopyImageToBuffer(command_buffer, srcImage, srcImageLayout, dstBuffer.buffer, regionCount, pRegions);
}
void CommandBuffer::copyImageToBuffer2(const VkCopyImageToBufferInfo2 *pCopyImageToBufferInfo)
{
    vkCmdCopyImageToBuffer2(command_buffer, pCopyImageToBufferInfo);
}
void CommandBuffer::beginDebugUtilsLabelEXT(const VkDebugUtilsLabelEXT *pLabelInfo)
{
    vkCmdBeginDebugUtilsLabelEXT(command_buffer, pLabelInfo);
}
void CommandBuffer::endDebugUtilsLabelEXT()
{
    vkCmdEndDebugUtilsLabelEXT(command_buffer);
}
void CommandBuffer::insertDebugUtilsLabelEXT(const VkDebugUtilsLabelEXT *pLabelInfo)
{
    vkCmdInsertDebugUtilsLabelEXT(command_buffer, pLabelInfo);
}
} // namespace LunaVoxelEngine::Renderer