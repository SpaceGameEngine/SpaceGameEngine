#include "Memory/AllocatorBase.h"
#include "Error.h"

namespace SpaceGameEngine
{

	SizeType GetDefaultAlignment(SizeType size)
	{
		SGE_ASSERT(InvalidSizeError, size, 1, SGE_MAX_MEMORY_SIZE);
		return size >= 16 ? 16 : 4;
	}

	bool InvalidAlignmentError::Judge(SizeType alignment)
	{
		return (alignment & (alignment - 1)) != 0;
	}
}