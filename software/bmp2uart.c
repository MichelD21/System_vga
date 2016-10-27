#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BITMAP_FILE_HEADER_SIZE 14 // Bytes

/* Bitmap file info */
struct BitmapFileHeader {
    unsigned char signature[2];
    unsigned int fileSize;
    unsigned char reserved[4];
    unsigned int dataOffset;    // Points the first image pixel
};     

/* Bitmap information header */
struct DIBHeader {
    unsigned int headerSize;
    unsigned int width;
    unsigned int height;
    unsigned short int planes;
    unsigned short int bitCount;  // Bits per pixel
    unsigned int compression;
    unsigned int imageSize;     // in pixels (width * height)
    unsigned int XpixelsPerM;   // Horizontal resolution: Pixels/meter
    unsigned int YpixelsPerM;   // Vertical resolution: Pixels/meter
    unsigned int colorsUsed;
    unsigned int colorsImportant;
};

void ReadBitmapFileHeader(struct BitmapFileHeader *bmpFileHeader, FILE *bmpFile) {
        
    fseek(bmpFile, 0, SEEK_SET);    // Seeks the file begin
    
    fread(&bmpFileHeader->signature, 1, sizeof(bmpFileHeader->signature), bmpFile);
    fread(&bmpFileHeader->fileSize, 1, sizeof(bmpFileHeader->fileSize), bmpFile);
    fread(&bmpFileHeader->reserved, 1, sizeof(bmpFileHeader->reserved), bmpFile);
    fread(&bmpFileHeader->dataOffset, 1, sizeof(bmpFileHeader->dataOffset), bmpFile);
    
}

void ReadDibHeader(struct DIBHeader *imageHeader, FILE *bmpFile) {
    
    fseek(bmpFile, BITMAP_FILE_HEADER_SIZE, SEEK_SET); // Seeks the begin of DIB Header
    
    fread(&imageHeader->headerSize, 1, sizeof(imageHeader->headerSize), bmpFile);
    fread(&imageHeader->width, 1, sizeof(imageHeader->width), bmpFile);
    fread(&imageHeader->height, 1, sizeof(imageHeader->height), bmpFile);
    fread(&imageHeader->planes, 1, sizeof(imageHeader->planes), bmpFile);
    fread(&imageHeader->bitCount, 1, sizeof(imageHeader->bitCount), bmpFile);
    fread(&imageHeader->compression, 1, sizeof(imageHeader->compression), bmpFile);
    fread(&imageHeader->imageSize, 1, sizeof(imageHeader->imageSize), bmpFile);
    fread(&imageHeader->XpixelsPerM, 1, sizeof(imageHeader->XpixelsPerM), bmpFile);
    fread(&imageHeader->YpixelsPerM, 1, sizeof(imageHeader->YpixelsPerM), bmpFile);
    fread(&imageHeader->colorsUsed, 1, sizeof(imageHeader->colorsUsed), bmpFile);
    fread(&imageHeader->colorsImportant, 1, sizeof(imageHeader->colorsImportant), bmpFile);
}



int main(int argc, char *argv[]) {

    FILE *bmpFile;
	FILE *txtFile;
    unsigned char *image;
    int x, y, rowSize;
        
    struct BitmapFileHeader bmpFileHeader;
    struct DIBHeader imageHeader;
	
	txtFile = fopen(argv[2], "wb+");
	
	char bytes[5];
    
    if (argc < 3) {
        printf("Usage: %s <file_name>.bmp <file_name>.bin\n", argv[0]);
        return -1;
    }

    bmpFile = fopen(argv[1], "rb");
    if (bmpFile == NULL) {
        printf("Can't open %s\n", argv[1]);
        return 1;
    }
	
	ReadDibHeader(&imageHeader,bmpFile);
	
    /* Allocates memory to store the image */
    image = (unsigned char *)malloc(imageHeader.imageSize); 
    
    /* Seeks the begin of image on bmp file */
    fseek(bmpFile, bmpFileHeader.dataOffset, SEEK_SET);
	   
    /* Reads the image */
    fread(image, 1, imageHeader.imageSize, bmpFile);  
    fclose(bmpFile);
	
	rowSize = 4*floor(((imageHeader.bitCount*imageHeader.width)+31)/32);
	
	//fprintf(txtFile,"%x", imageHeader.height*imageHeader.width + 5);
	
	//fprintf(txtFile,"%X",3);													// HEADER
	//fprintf(txtFile,"%X",0);													// LINE
	//fprintf(txtFile,"%X",0xFF);													// COLUMN
	//fprintf(txtFile,"\x", imageHeader.height);							// HEIGHT
	//fprintf(txtFile,"%x", imageHeader.width);								// WIDTH
	
	bytes[0] = 0x12;
	bytes[1] = 0x34;
	bytes[2] = 0xab;
	bytes[3] = 0x54;
	bytes[4] = 0x78;
	fwrite(bytes,1,sizeof(bytes),txtFile);
	
    /*** Extracts the image pixels ***/
	
	for(y=imageHeader.height-1; y>=0; y--) {
	
		for(x=0; x<rowSize; x++) {
			fprintf(txtFile,"%x",image[x + (y*rowSize)]);
		}
	}
	
	fclose(txtFile);
    
    return 0;
}
