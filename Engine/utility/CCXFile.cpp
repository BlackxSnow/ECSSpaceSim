#include "CCXFile.h"

#include <bx/file.h>

#include "ConsoleLogging.h"

bx::AllocatorI* GetDefaultAllocator();

bx::AllocatorI* _Allocator = GetDefaultAllocator();
bx::FileReaderI* _Reader;

bx::FileReaderI* CCX::GetFileReader()
{
	if (_Reader != nullptr)
	{
		return _Reader;
	}

	_Reader = BX_NEW(GetDefaultAllocator(), bx::FileReader);
	return _Reader;
}

bx::AllocatorI* GetDefaultAllocator()
{
	static bx::DefaultAllocator allocator;
	return &allocator;
}

const bgfx::Memory* CCX::LoadMemory(bx::FileReaderI* reader, std::string filePath)
{
	if (bx::open(reader, filePath.c_str()))
	{
		uint32_t size = (uint32_t)bx::getSize(reader);
		const bgfx::Memory* memory = bgfx::alloc(size + 1);
		bx::read(reader, memory->data, size, bx::ErrorAssert{});
		bx::close(reader);
		memory->data[memory->size - 1] = '\0';
		return memory;
	}

	LogError("Failed to load " + filePath, false);
	return nullptr;
}