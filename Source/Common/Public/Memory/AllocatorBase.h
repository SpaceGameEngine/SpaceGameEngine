#include "TypeDefinition.hpp"
#include "Platform.hpp"
#include "SGEString.h"

namespace SpaceGameEngine
{

#ifdef SGE_X86
#define SGE_MAX_MEMORY_SIZE UINT32_MAX
#elif defined(SGE_X64)
#define SGE_MAX_MEMORY_SIZE UINT64_MAX
#endif

	/*!
	@brief make the memory size or memory address aligned using the alignment
	*/
#define SGE_MEMORY_ALIGN(value, alignment) (((value) + ((alignment)-1)) & ~((alignment)-1))

	struct InvalidAlignmentError
	{
		inline static const TChar sm_pContent[] = SGE_TSTR("The alignment is invalid");
		/*!
		@note only the alignment which is 0 or 2^n can pass the judgment.
		*/
		static bool Judge(SizeType alignment);
	};

	/*!
	@brief get default alignment by giving memory size
	@note the default alignment depends on the allocated
	memory size,when the size >= 16,the alignment is 16,or it will be 4.
	*/
	SizeType GetDefaultAlignment(SizeType size);

}