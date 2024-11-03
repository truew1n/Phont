#include <iostream>

#include "Converter.h"
#include "File.h"
#include "Deque.h"
#include "Hashmap.h"

#include "Tag.h"

typedef struct STable {
	uint32_t Checksum;
	uint32_t Offset;
	uint32_t Length;
} STable;

typedef struct SBoundingBox {
	int16_t XMinimum;
	int16_t YMinimum;
	int16_t XMaximum;
	int16_t YMaximum;
} SBoundingBox;

int main(void)
{
	CFile *File = CFile::Open("Assets\\Fonts\\CascadiaMono.ttf", "rb");

	if (!File) return 1;
	
	File->Seek(4, ESeekOrigin::Current);
	
	uint16_t NumberOfTables;
	File->Read(&NumberOfTables, sizeof(NumberOfTables), 1);

	NumberOfTables = NConverter::SwapEndian(NumberOfTables);

	printf("Number Of Tables: %i\n", NumberOfTables);

	File->Seek(6, ESeekOrigin::Current);

	uint32_t Tag = 0;
	STable Table = {0};
	THashMap<uint32_t, STable> HashMap;
	for (uint32_t I = 0; I < NumberOfTables; ++I) {
		File->Read(&Tag, sizeof(Tag), 1);
		File->Read(&Table, sizeof(Table), 1);

		Table.Checksum = NConverter::SwapEndian(Table.Checksum);
		Table.Offset = NConverter::SwapEndian(Table.Offset);
		Table.Length = NConverter::SwapEndian(Table.Length);

		HashMap.Insert(Tag, Table);
	}

	STable GlyfOffsetTable = HashMap[GLYF_TABLE_B];
	std::cout << "glyf Pointer Table:" << std::endl;
	std::cout << "\tChecksum: " << GlyfOffsetTable.Checksum << std::endl;
	std::cout << "\tOffset: " << GlyfOffsetTable.Offset << std::endl;
	std::cout << "\tLength: " << GlyfOffsetTable.Length << std::endl;

	File->Seek(GlyfOffsetTable.Offset, ESeekOrigin::Set);

	int16_t NumberOfContours = 0;
	File->Read(&NumberOfContours, sizeof(NumberOfContours), 1);
	std::cout << NConverter::SwapEndian(NumberOfContours) << std::endl;

	SBoundingBox BoundingBox = {0};
	File->Read(&BoundingBox, sizeof(BoundingBox), 1);
	printf("%i\n", NConverter::SwapEndian(BoundingBox.XMinimum));
	printf("%i\n", NConverter::SwapEndian(BoundingBox.YMinimum));
	printf("%i\n", NConverter::SwapEndian(BoundingBox.XMaximum));
	printf("%i\n", NConverter::SwapEndian(BoundingBox.YMaximum));

	delete File;

	return 0;
}