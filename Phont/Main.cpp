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

typedef struct SGlyphFlags {
	uint8_t OnCurve;
	uint8_t XShortVector;
	uint8_t YShortVector;
	uint8_t Repeat;
	uint8_t PositiveXShortVector;
	uint8_t PositiveYShortVector;
	uint8_t ReservedA;
	uint8_t ReservedB;
} SGlyphFlags;


SGlyphFlags ExtractSimpleGlyphFlags(uint32_t Flags)
{
	SGlyphFlags GlyphFlags = {0};
	GlyphFlags.OnCurve = (Flags & 0x01) >> 0;
	GlyphFlags.XShortVector = (Flags & 0x02) >> 1;
	GlyphFlags.YShortVector = (Flags & 0x04) >> 2;
	GlyphFlags.Repeat = (Flags & 0x08) >> 3;
	GlyphFlags.PositiveXShortVector = (Flags & 0x10) >> 4;
	GlyphFlags.PositiveYShortVector = (Flags & 0x20) >> 5;
	GlyphFlags.ReservedA = (Flags & 0x40) >> 6;
	GlyphFlags.ReservedB = (Flags & 0x80) >> 7;
	return GlyphFlags;
}

void PrintSimpleGlyphFlags(uint8_t Flags)
{
	printf(
		"On Curve: %i\n"
		"X-Short Vector: %i\n"
		"Y-Short Vector: %i\n"
		"Repeat: %i\n"
		"Positive X-Short Vector: %i\n"
		"Positive Y-Short Vector: %i\n"
		"Reserved: %i\n"
		"Reserved: %i\n",
		(Flags & 0x01) >> 0,
		(Flags & 0x02) >> 1,
		(Flags & 0x04) >> 2,
		(Flags & 0x08) >> 3,
		(Flags & 0x10) >> 4,
		(Flags & 0x20) >> 5,
		(Flags & 0x40) >> 6,
		(Flags & 0x80) >> 7
	);
}
void PrintSimpleGlyphFlags(SGlyphFlags &GlypFlags)
{
	printf(
		"On Curve: %i\n"
		"X-Short Vector: %i\n"
		"Y-Short Vector: %i\n"
		"Repeat: %i\n"
		"Positive X-Short Vector: %i\n"
		"Positive Y-Short Vector: %i\n"
		"Reserved: %i\n"
		"Reserved: %i\n",
		GlypFlags.OnCurve,
		GlypFlags.XShortVector,
		GlypFlags.YShortVector,
		GlypFlags.Repeat,
		GlypFlags.PositiveXShortVector,
		GlypFlags.PositiveYShortVector,
		GlypFlags.ReservedA,
		GlypFlags.ReservedB
	);
}


void ReadSimpleGlyph(CFile *File, int16_t NumberOfContours)
{
	uint16_t *ContourEndPoints = (uint16_t *) malloc(sizeof(uint16_t) * NumberOfContours);

	File->Read(ContourEndPoints, sizeof(uint16_t), NumberOfContours);

	uint16_t MaxIndex = NConverter::SwapEndian(ContourEndPoints[0]);
	printf("Contour Index: 0 - %i\n", MaxIndex);
	for (int16_t i = 1; i < NumberOfContours; ++i) {
		uint16_t CurrentIndex = NConverter::SwapEndian(ContourEndPoints[i]);
		printf(
			"Contour Index: %i - %i\n",
			NConverter::SwapEndian(ContourEndPoints[i - 1]),
			CurrentIndex
		);
		if (MaxIndex < CurrentIndex) {
			MaxIndex = CurrentIndex;
		}
	}
	MaxIndex++;
	printf("Max Index Count: %i\n", MaxIndex);

	uint16_t InstructionLength = 0;
	File->Read(&InstructionLength, sizeof(InstructionLength), 1);

	uint8_t *Instructions = (uint8_t *) malloc(sizeof(uint16_t) * InstructionLength);

	File->Read(Instructions, sizeof(uint8_t), InstructionLength); 

	uint8_t Flags = 0;
	File->Read(&Flags, sizeof(Flags), 1);

	SGlyphFlags GlyphFlags = ExtractSimpleGlyphFlags(Flags);
	PrintSimpleGlyphFlags(GlyphFlags);

	

	free(ContourEndPoints);
	free(Instructions);
}

void ReadCompoundGlyph(CFile *File, int16_t NumberOfContours)
{

}

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
	printf(
		"glyf Pointer Table:\n"
		"\tChecksum: %i\n"
		"\tOffset: %i\n"
		"\tLength: %i\n",
		GlyfOffsetTable.Checksum,
		GlyfOffsetTable.Offset,
		GlyfOffsetTable.Length
	);

	File->Seek(GlyfOffsetTable.Offset, ESeekOrigin::Set);

	int16_t NumberOfContours = 0;
	File->Read(&NumberOfContours, sizeof(NumberOfContours), 1);
	NumberOfContours = NConverter::SwapEndian(NumberOfContours);
	printf("Number of Contours: %i\n", NumberOfContours);

	SBoundingBox BoundingBox = {0};
	File->Read(&BoundingBox, sizeof(BoundingBox), 1);
	printf(
		"X Minimum: %i\n"
		"Y Minimum: %i\n"
		"X Maximum: %i\n"
		"Y Maximum: %i\n",
		NConverter::SwapEndian(BoundingBox.XMinimum),
		NConverter::SwapEndian(BoundingBox.YMinimum),
		NConverter::SwapEndian(BoundingBox.XMaximum),
		NConverter::SwapEndian(BoundingBox.YMaximum)

	);

	if (NumberOfContours >= 0) {
		ReadSimpleGlyph(File, NumberOfContours);
	} else {
		ReadCompoundGlyph(File, NumberOfContours);
	}

	delete File;

	return 0;
}