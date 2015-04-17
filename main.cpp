#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <Windows.h>
#include <assert.h>
#include <vector>
#include <math.h>

BITMAPFILEHEADER head1;
BITMAPINFOHEADER head2;
RGBTRIPLE* buf1 = NULL;
RGBTRIPLE* buf2 = NULL;

#define img1(x,y) buf1[(y) * head2.biWidth + (x)]
#define img2(x,y) buf2[(y) * head2.biWidth + (x)]

int main(int argc, char** argv){
	/* Read from file. */
	if (argc < 2){
		printf("nothing to recognize!.\r\n");
		return -1;
	}

	FILE* f = fopen(argv[1], "rb");

	fread(&head1, sizeof head1, 1, f);
	fread(&head2, sizeof head2, 1, f);
	assert(sizeof(RGBTRIPLE) == 3);
	buf1 = (RGBTRIPLE*)malloc(head2.biWidth * head2.biHeight * sizeof(RGBTRIPLE) + 12);
	buf2 = (RGBTRIPLE*)calloc(head2.biWidth * head2.biHeight, sizeof(RGBTRIPLE) + 12);
	fseek(f, head1.bfOffBits, SEEK_SET);
	RGBTRIPLE* mb = buf1;
	for (int i = 0; i < head2.biHeight; i++){
		fread(mb, sizeof(RGBTRIPLE), head2.biWidth, f);
		mb += head2.biWidth;
		fread(mb, 1, (sizeof(RGBTRIPLE) * head2.biWidth) % 4 ? 4 - ((sizeof(RGBTRIPLE) * head2.biWidth) % 4) : 0, f);
	}

	for (int y = 0; y < head2.biHeight; y++)
        for (int x = 0; x < head2.biWidth; x++){
            BYTE t1 = img1(x,y).rgbtRed > img1(x,y).rgbtBlue ? img1(x,y).rgbtRed - img1(x,y).rgbtBlue : 0;
            BYTE t2 = img1(x,y).rgbtGreen > img1(x,y).rgbtBlue ? img1(x,y).rgbtGreen - img1(x,y).rgbtBlue : 0;
            int ti = (int)(255 - t2) + (int)(255 - t1);
            if (ti > 510) ti = 510;
            img1(x,y).rgbtGreen = (510 - ti) / 2;
            img1(x,y).rgbtGreen = (int)img1(x,y).rgbtGreen * img1(x,y).rgbtGreen / 255;
        }

    for (int y = 1; y < head2.biHeight - 1; y++ )
        for(int x = 1; x < head2.biWidth - 1; x++ ){
            int gx = (int)img1(x+1,y-1).rgbtGreen + 2*(int)img1(x+1,y).rgbtGreen + (int)img1(x+1,y+1).rgbtGreen - (int)img1(x-1,y-1).rgbtGreen - 2*(int)img1(x-1,y).rgbtGreen - (int)img1(x-1,y+1).rgbtGreen;
            int gy = (int)img1(x-1,y-1).rgbtGreen + 2*(int)img1(x,y-1).rgbtGreen + (int)img1(x+1,y-1).rgbtGreen - (int)img1(x-1,y+1).rgbtGreen - 2*(int)img1(x,y+1).rgbtGreen - (int)img1(x+1,y+1).rgbtGreen;
            img2(x,y).rgbtBlue = img2(x,y).rgbtGreen = img2(x,y).rgbtRed = /*img1(x,y).rgbtGreen;/*/(unsigned char)(int)sqrt(gx * gx + gy * gy);
        }

	fclose(f);
	f = fopen("spec.bmp", "wb");
	fwrite(&head1, sizeof head1, 1, f);
	fwrite(&head2, sizeof head2, 1, f);
	void * p = calloc(head1.bfOffBits,1);
	fwrite(p, head1.bfOffBits, 1, f);
	fseek(f, head1.bfOffBits, SEEK_SET);
	mb = buf2;
	for (int i = 0; i < head2.biHeight; i++){
		fwrite(mb, sizeof(RGBTRIPLE), head2.biWidth, f);
		mb += head2.biWidth;
		fwrite(p, 1, (sizeof(RGBTRIPLE) * head2.biWidth) % 4 ? 4 - ((sizeof(RGBTRIPLE) * head2.biWidth) % 4) : 0, f);
	}
	fclose(f);

	return 0;
}
