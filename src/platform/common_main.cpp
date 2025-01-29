#include <platform/main.h>
#include <utils/new.h>

namespace LunaVoxelEngine::Platform
{
bool Runtime::Init(Utils::Vector<Utils::String> args) noexcept
{
    window = new Window(1280, 720, "LunaVoxelEngine");
    window->show();
    device = new Renderer::Device(true);
    swap_chain = new Renderer::SwapChain(device, window->getVulkanLink());
    command_buffer = new Renderer::CommandBuffer(nullptr);
    queue = new Renderer::Queue(device->get_graphics_family_index());
    Renderer::GraphicsPipelineBuilder builder;
    pipeline = new Renderer::Pipeline(&builder);
    return false;
}

bool Runtime::IsRunning() const noexcept
{
    return !window->shouldClose();
}

void Runtime::Update() noexcept {
    // Poll window events
    window->pollEvents();

    // Synchronization objects
    VkSemaphore image_available_semaphore;
    VkSemaphore render_finished_semaphore;
    VkFence in_flight_fence;

    //VkSemaphoreCreateInfo semaphore_info{};
    //semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    //VkFenceCreateInfo fence_info{};
    //fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    //fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    // Wait for previous frame
    //device->waitForFences(1, &in_flight_fence, VK_TRUE, UINT64_MAX);
    //device->resetFences(1, &in_flight_fence);
    uint32_t image_index = 0;
    swap_chain->acquireNextImage(image_available_semaphore, nullptr, &image_index);
    auto swap_chain_extent = swap_chain->getExtent();
    
    // Setup color attachment
    VkRenderingAttachmentInfo color_attachment{};
    color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    color_attachment.imageView = swap_chain->getImageViews()[image_index];
    color_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.clearValue = {{0.0f, 0.0f, 0.0f, 1.0f}};

    // Record commands
    command_buffer->begin();

    // Image layout transition: Undefined → Color Attachment Optimal
    VkImageMemoryBarrier2 image_memory_barrier{};
    image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    image_memory_barrier.image = swap_chain->getImages()[image_index];
    image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_memory_barrier.subresourceRange.baseMipLevel = 0;
    image_memory_barrier.subresourceRange.levelCount = 1;
    image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    image_memory_barrier.subresourceRange.layerCount = 1;
    image_memory_barrier.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
    image_memory_barrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    image_memory_barrier.srcAccessMask = 0;
    image_memory_barrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

    VkDependencyInfo dependency_info{};
    dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependency_info.imageMemoryBarrierCount = 1;
    dependency_info.pImageMemoryBarriers = &image_memory_barrier;

    command_buffer->pipelineBarrier2(&dependency_info);

    // Begin dynamic rendering
    VkRenderingInfo rendering_info{};
    rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    rendering_info.renderArea.offset = {0, 0};
    rendering_info.renderArea.extent = swap_chain_extent;
    rendering_info.layerCount = 1;
    rendering_info.viewMask = 0;
    rendering_info.colorAttachmentCount = 1;
    rendering_info.pColorAttachments = &color_attachment;

    command_buffer->beginRendering(&rendering_info);
    command_buffer->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    // Set viewport and scissor
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swap_chain_extent.width);
    viewport.height = static_cast<float>(swap_chain_extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    command_buffer->setViewport(0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swap_chain_extent;
    command_buffer->setScissor(0, 1, &scissor);

    // Draw call
    command_buffer->draw(3, 1, 0, 0);
    command_buffer->endRendering();

    // Image layout transition: Color Attachment Optimal → Present
    image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    image_memory_barrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    image_memory_barrier.dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
    image_memory_barrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    image_memory_barrier.dstAccessMask = 0;

    command_buffer->pipelineBarrier2(&dependency_info);
    command_buffer->end();

    // Submit command buffer
    VkSubmitInfo2 submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;

    VkCommandBufferSubmitInfo cmd_submit_info{};
    cmd_submit_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    cmd_submit_info.commandBuffer = command_buffer->handle();

    VkSemaphoreSubmitInfo wait_semaphore_info{};
    wait_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    wait_semaphore_info.semaphore = image_available_semaphore;
    wait_semaphore_info.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSemaphoreSubmitInfo signal_semaphore_info{};
    signal_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    signal_semaphore_info.semaphore = render_finished_semaphore;
    signal_semaphore_info.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

    submit_info.commandBufferInfoCount = 1;
    submit_info.pCommandBufferInfos = &cmd_submit_info;
    submit_info.waitSemaphoreInfoCount = 1;
    submit_info.pWaitSemaphoreInfos = &wait_semaphore_info;
    submit_info.signalSemaphoreInfoCount = 1;
    submit_info.pSignalSemaphoreInfos = &signal_semaphore_info;

    queue->submit2(submit_info, in_flight_fence);

    swap_chain->present(queue, image_index, render_finished_semaphore);

    // Cleanup
    command_buffer->reset(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
}

void Runtime::Shutdown() noexcept
{
    vkDeviceWaitIdle(volkGetLoadedDevice());
    delete command_buffer;
    delete queue;
    delete swap_chain;
    delete device;
    delete window;
}
} // namespace LunaVoxelEngine::Platform