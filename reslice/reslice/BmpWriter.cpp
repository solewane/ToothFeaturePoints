#include "BmpWriter.h"

BmpWriter::BmpWriter(int x, int y) {
	setSize(x, y);
	bitmapImage = (unsigned char*)malloc(w * h * 3);
	memset(bitmapImage, 0, w * h * 3);
}

bool BmpWriter::SaveImage(char* szPathName) {
	BITMAPINFOHEADER BMIH;
	BITMAPFILEHEADER bmfh;

	FILE *pFile = fopen(szPathName, "wb");

	if (pFile == NULL) {
		return false;
	}

	BMIH.HeaderSize = sizeof(BITMAPINFOHEADER);
	BMIH.Width = w;
	BMIH.Height = h;
	BMIH.Planes = 1;
	BMIH.BitCount = 24;
	BMIH.Compression = BI_RGB;
	BMIH.SizeImage = w * h * 3;

	int nBitsOffset = sizeof(BITMAPFILEHEADER) + BMIH.HeaderSize;
	long lImageSize = BMIH.SizeImage;
	long lFileSize = nBitsOffset + lImageSize;
	bmfh.Signature = 'B' + ('M' << 8);
	bmfh.BitsOffset = nBitsOffset;
	bmfh.Size = lFileSize;
	bmfh.Reserved = 0;

	unsigned int nWrittenFileHeaderSize = fwrite(&bmfh, 1, sizeof(BITMAPFILEHEADER), pFile);
	unsigned int nWrittenInfoHeaderSize = fwrite(&BMIH, 1, sizeof(BITMAPINFOHEADER), pFile);
	unsigned int nWrittenDIBDataSize = fwrite(bitmapImage, 1, lImageSize, pFile);

	fclose(pFile);

	return true;
}

void BmpWriter::setPixel(int x, int y, RGB rgb) {
	int ptr = ((x + (y * w)) * 3);

	bitmapImage[ptr] = rgb.b;
	bitmapImage[ptr + 1] = rgb.g;
	bitmapImage[ptr + 2] = rgb.r;
}

void BmpWriter::setSize(int x, int y) {
	w = x;
	h = y;
}
