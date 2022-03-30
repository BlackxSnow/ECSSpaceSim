#pragma once

#include <bx/readerwriter.h>
#include <bgfx/bgfx.h>
#include <string>

namespace CCX
{
	bx::FileReaderI* GetFileReader();
	const bgfx::Memory* LoadMemory(bx::FileReaderI* reader, std::string filePath);
}