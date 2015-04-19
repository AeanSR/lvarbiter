#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <Windows.h>
#include <assert.h>
#include <vector>
#include <math.h>
#include <algorithm>
#include <functional>

BITMAPFILEHEADER head1;
BITMAPINFOHEADER head2;
RGBTRIPLE* buf1 = NULL;
RGBTRIPLE* buf2 = NULL;
char* closed = NULL;
#define img1(x,y) buf1[(y) * head2.biWidth + (x)]
#define img2(x,y) buf2[(y) * head2.biWidth + (x)]

int rec(size_t x, size_t y){
	if (img2(x, y).rgbtBlue < 255) return 0;
	if (closed[y * head2.biWidth + x]) return 0;
	int ret = 1;
	closed[y * head2.biWidth + x] = 1;
	if (x > 0)
		ret += rec(x-1, y);
	if (y > 0)
		ret += rec(x, y-1);
	if ((int)x + 1 < head2.biWidth)
		ret += rec(x+1, y);
	if ((int)y + 1 < head2.biHeight)
		ret += rec(x, y+1);
	return ret;
}

void rec2(size_t x, size_t y){
	if (img2(x, y).rgbtBlue < 255) return;
	if (!closed[y * head2.biWidth + x]) return;
	closed[y * head2.biWidth + x] = 0;
	if (x > 0)
		rec2(x-1, y);
	if (y > 0)
		rec2(x, y-1);
	if ((int)x + 1 < head2.biWidth)
		rec2(x+1, y);
	if ((int)y + 1 < head2.biHeight)
		rec2(x, y+1);
    img2(x,y).rgbtBlue = 0;
	return;
}

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
            img1(x,y).rgbtGreen = t1*t2/255;
            //img1(x,y).rgbtGreen = (int)img1(x,y).rgbtGreen * img1(x,y).rgbtGreen / 255;
        }

    for (int y = 0; y < head2.biHeight; y++)
        for (int x = 0; x < head2.biWidth; x++){
            BYTE filter[5];
            filter[0] = ( x ? img1(x-1,y).rgbtGreen : img1(x,y).rgbtGreen );
            filter[1] = ( img1(x,y).rgbtGreen );
            filter[2] = ( x == head2.biWidth - 1 ? img1(x,y).rgbtGreen : img1(x+1,y).rgbtGreen );
            filter[3] = ( y ? img1(x,y-1).rgbtGreen : img1(x,y).rgbtGreen );
            filter[4] = ( y == head2.biHeight - 1 ? img1(x,y).rgbtGreen : img1(x,y+1).rgbtGreen );
            std::sort(filter, filter+5, std::greater<BYTE>());
            img1(x,y).rgbtBlue = filter[2];
        }

    for (int y = 1; y < head2.biHeight - 1; y++ )
        for(int x = 1; x < head2.biWidth - 1; x++ ){
            int gx = (int)img1(x+1,y-1).rgbtBlue + 2*(int)img1(x+1,y).rgbtBlue + (int)img1(x+1,y+1).rgbtBlue - (int)img1(x-1,y-1).rgbtBlue - 2*(int)img1(x-1,y).rgbtBlue - (int)img1(x-1,y+1).rgbtBlue;
            int gy = (int)img1(x-1,y-1).rgbtBlue + 2*(int)img1(x,y-1).rgbtBlue + (int)img1(x+1,y-1).rgbtBlue - (int)img1(x-1,y+1).rgbtBlue - 2*(int)img1(x,y+1).rgbtBlue - (int)img1(x+1,y+1).rgbtBlue;
            img2(x,y).rgbtBlue = (unsigned char)(int)sqrt(gx * gx + gy * gy);
        }

    for (int y = 0; y < head2.biHeight; y++)
        for (int x = 0; x < head2.biWidth; x++)
            img2(x,y).rgbtBlue = img2(x,y).rgbtBlue > 40 ? 255 : 0;

    for (int i = 0; i < 3; i++)
    for (int y = 0; y < head2.biHeight; y++)
        for (int x = 0; x < head2.biWidth; x++){
            BYTE filter[5];
            filter[0] = ( x ? img2(x-1,y).rgbtBlue : img2(x,y).rgbtBlue );
            filter[1] = ( img2(x,y).rgbtBlue );
            filter[2] = ( x == head2.biWidth - 1 ? img2(x,y).rgbtBlue : img2(x+1,y).rgbtBlue );
            filter[3] = ( y ? img2(x,y-1).rgbtBlue : img2(x,y).rgbtBlue );
            filter[4] = ( y == head2.biHeight - 1 ? img2(x,y).rgbtBlue : img2(x,y+1).rgbtBlue );
            std::sort(filter, filter+5, std::greater<BYTE>());
            img2(x,y).rgbtBlue = filter[2];
        }

    closed = (char *)calloc(head2.biWidth * head2.biHeight, 1);
    for (int x = 0; x < head2.biWidth; x++){
		for (int y = 0; y < head2.biHeight; y++){
			if (img2(x,y).rgbtBlue < 255) continue;
			if (closed[y * head2.biWidth + x]) continue;
			int ret = rec(x,y);
            if (ret < 60) rec2(x,y);
		}
	}

    int mingreysum = head2.biWidth;
    for (int y = 1; y < head2.biHeight - 1; y++){
        int greysum = 0;
        for (int x = 1; x < head2.biWidth - 1; x++){
            greysum += img2(x,y).rgbtBlue;
        }
        greysum /= 255;
        for (int x = 0; x < head2.biWidth; x++){
            img2(x,y).rgbtBlue = img2(x,y).rgbtGreen = img2(x,y).rgbtRed = (x < greysum) ? 0 : 255;

        }
        mingreysum = greysum < mingreysum ? greysum : mingreysum;
    }
    for (int y = 0; y < head2.biHeight; y++){
        img2(mingreysum + 20, y) = (RGBTRIPLE){0,0,255};
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
