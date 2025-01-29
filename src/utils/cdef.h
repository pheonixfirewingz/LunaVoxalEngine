#ifndef CDEF_H
#    define CDEF_H
namespace LunaVoxelEngine
{
namespace Utils
{
#    if defined(ON_WINDOWS)
	using max_align_t = double;
	typedef unsigned __int64 size_t;
#        else
typedef unsigned long size_t;
#        endif
} // namespace Utils
} // namespace LunaVoxelEngine
#endif // CDEF_H