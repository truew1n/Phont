#include "File.h"

CFile::CFile()
{
	File = nullptr;
	BInitialized = false;
}

CFile *CFile::Open(const char *Filepath, const char *Mode)
{
	CFile *NewFile = new CFile();

	fopen_s(&NewFile->File, Filepath, Mode);

	if (!NewFile->File) {
		std::cerr << "CFile: Failed to open a file: " << Filepath << std::endl;
	}

	NewFile->BInitialized = true;

	return NewFile;
}

bool CFile::Read(void *Buffer, size_t ElementSize, size_t ElementCount)
{
	if (!BInitialized) return false;
	
	fread(Buffer, ElementSize, ElementCount, File);

	return true;
}

bool CFile::Write(void *Buffer, size_t ElementSize, size_t ElementCount)
{
	if (!BInitialized) return false;

	fwrite(Buffer, ElementSize, ElementCount, File);

	return true;
}

bool CFile::Seek(int32_t Offset, ESeekOrigin Origin)
{
	if (!BInitialized) return false;

	fseek(File, Offset, (int32_t) Origin);

	return true;
}

uint64_t CFile::Tell()
{
	if (!BInitialized) return false;

	return ftell(File);
}

CFile::~CFile()
{
	if (BInitialized) {
		fclose(File);
	}
}