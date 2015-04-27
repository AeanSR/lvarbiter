#include <Windows.h>
#include <cstdio>
extern BITMAPFILEHEADER head1;
extern BITMAPINFOHEADER head2;
extern RGBTRIPLE* buf1;
extern RGBTRIPLE* buf2;
#define img1(x,y) buf1[(y) * head2.biWidth + (x)]
int zishu = 0;
int zifur[256];
int ziful[256];
extern int outidx;

void zifufenge(int chang, int kuan, int* in) {
    int* zimu = new int[chang];

    for( int i = 0; i < chang; i++ ) {
        zimu[i] = 0;
        for( int j = 0; j < kuan; j++ ) {
            zimu[i] += in[i + j * chang];
        }
        zimu[i] /= 255;
    }
	for( int repeat = 0; repeat < 0; repeat ++ ) {
        int capac = 0;
        for( int i = 0; i < chang; i++ ) {
            capac = zimu[i] = ( zimu[i] + capac ) / 2;
        }
        capac = 0;
        for( int i = chang - 1; i >= 0; i-- ) {
            capac = zimu[i] = ( zimu[i] + capac ) / 2;
        }
    }

    int minzimu = zimu[0];
    for( int i = 0; i < chang; i++ ) {
        minzimu = minzimu < zimu[i] ? minzimu : zimu[i];

    }
    minzimu += 0;

    zishu = 0;
	if (zimu[0] > minzimu) ziful[zishu++] = 0;
    for( int i = 0; i < chang - 1; i++ ) {
        if ( zimu[i] <= minzimu ) {
            if ( zimu[i + 1] > minzimu ) {
                ziful[zishu] = i;
                zishu++;
            }
        }
    }
    zishu = 0;

    for( int i = 0; i < chang - 1; i++ ) {
        if( zimu[i] > minzimu ) {
            if( zimu[i + 1] <= minzimu ) {
                zifur[zishu] = i;
                zishu++;
            }
        }
    }
	if (zimu[chang - 1] > minzimu) zifur[zishu++] = chang - 1;

	char buf[256] = {0};
	sprintf(buf, "result\\line%d\\linespec.bmp", outidx + 1);
	FILE* f = fopen( buf, "wb" );
    for( int i = 0; i < head2.biWidth; i++ ) {
        for( int j = 0; j < head2.biHeight; j++ ) {
            img1( i, j ).rgbtBlue = img1( i, j ).rgbtGreen = img1( i, j ).rgbtRed = i < chang ?( zimu[i] > j ? 0 : 255) : 255;
        }
    }
    fwrite( &head1, sizeof head1, 1, f );
    fwrite( &head2, sizeof head2, 1, f );
    void * p = calloc( head1.bfOffBits, 1 );
    fwrite( p, head1.bfOffBits, 1, f );
    fseek( f, head1.bfOffBits, SEEK_SET );
    RGBTRIPLE* mb = buf1;
    for ( int i = 0; i < head2.biHeight; i++ ) {
        fwrite( mb, sizeof( RGBTRIPLE ), head2.biWidth, f );
        mb += head2.biWidth;
        fwrite( p, 1, ( sizeof( RGBTRIPLE ) * head2.biWidth ) % 4 ? 4 - ( ( sizeof( RGBTRIPLE ) * head2.biWidth ) % 4 ) : 0, f );
    }
    fclose( f );

    delete[] zimu;
}
