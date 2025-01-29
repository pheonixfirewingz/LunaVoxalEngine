#include <platform/log.h>
#include <renderer/vulkan/buffer.h>


namespace LunaVoxelEngine::Renderer
{

Buffer::Buffer()
{
}

void *Buffer::map()
{
    return nullptr;
}

void Buffer::unmap()
{
}

Buffer::~Buffer()
{
}
} // namespace LunaVoxelEngine::Renderer