#include "math.h"
#include "string.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include <algorithm>

struct RGB {
	uint8_t r, g, b;
};

#define BI_RGB 0L

#pragma pack(1)

typedef struct BITMAPFILEHEADER {
	uint16_t Signature;
	uint32_t Size;
	uint32_t Reserved;
	uint32_t BitsOffset;
} BITMAPFILEHEADER;

#define BITMAP_FILEHEADER_SIZE 14

typedef struct {
	uint32_t HeaderSize;
	int32_t Width;
	int32_t Height;
	uint16_t Planes;
	uint16_t BitCount;
	uint32_t Compression;
	uint32_t SizeImage;
	int32_t PelsPerMeterX;
	int32_t PelsPerMeterY;
	uint32_t ClrUsed;
	uint32_t ClrImportant;
} BITMAPINFOHEADER;

#pragma pack()

class BmpWriter {
public:
	unsigned char *bitmapImage;
	int w;
	int h;

public:
	BmpWriter(int x, int y);
	void setSize(int x, int y);
	bool SaveImage(char* szPathName);
	void setPixel(int x, int y, RGB rgb);
};
