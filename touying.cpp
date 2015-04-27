#include <Windows.h>
#include <cstdio>
extern BITMAPFILEHEADER head1;
extern BITMAPINFOHEADER head2;
extern RGBTRIPLE* buf1;
extern RGBTRIPLE* buf2;
#define img1(x,y) buf1[(y) * head2.biWidth + (x)]
#define in(x,y) buf2[(y) * head2.biWidth + (x)].rgbtBlue
int count = 0;
int top[500];
int bottom[500];
int finleft;
int finright;

void line_slice() {
    int* out;
    int* outr;
    out = new int[head2.biWidth];
    outr = new int[head2.biHeight];

    for( int i = 0; i < head2.biWidth; i++ ) {
        out[i] = 0;
        for( int j = 0; j < head2.biHeight; j++ ) {
            out[i] += in( i, j );
        }
        out[i] /= 255;
    }

    for( int repeat = 0; repeat < 3; repeat ++ ) {
        int capac = 0;
        for( int i = 0; i < head2.biWidth; i++ ) {
            capac = out[i] = ( out[i] + capac ) / 2;
        }
        capac = 0;
        for( int i = head2.biWidth - 1; i >= 0; i-- ) {
            capac = out[i] = ( out[i] + capac ) / 2;
        }
    }

    int minout;
    minout = out[0];
    for( int k = 1; k < head2.biWidth; k++ ) {
        minout = minout < out[k] ? minout : out[k];
    }
    minout += 5;
    int right[1000] = {0};

    for( int d = head2.biWidth - 2; d > 0; d-- ) {
        if ( out[d] > minout ) {
            if ( out[d + 1] <= minout ) {
                right[count] = d;
                count++;
            }
        }
    }
    count = 0;
    int left[1000] = {0};
    for( int d = head2.biWidth - 2; d > 0; d-- ) {
        if ( out[d] <= minout ) {
            if ( out[d + 1] > minout ) {
                left[count] = d;
                count++;
            }
        }
    }

    printf( "column found: %d\n", count );
    FILE* f = fopen( "spec.bmp", "wb" );
    for( int i = 0; i < head2.biWidth; i++ ) {
        for( int j = 0; j < head2.biHeight; j++ ) {
            img1( i, j ).rgbtBlue = img1( i, j ).rgbtGreen = img1( i, j ).rgbtRed = out[i] > j ? 0 : 255;
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

    int width[1000], widthsum = 0;
    for( int w = 0; w < count; w++ ) {
        width[w] = right[w] - left[w];
        widthsum += width[w];
    }
    int widthaver = widthsum / 5;
    for( int i = 0; i < count; i++ ) {
        if ( width[i] < widthaver ) {
            count--;
            for( int j = i; j < count; j++ ) {
                width[j] = width[j + 1];
                left[j] = left[j + 1];
                right[j] = right[j + 1];

            }
        }
    }
    widthaver = widthsum / 10;
    for( int i = 0; i < count - 1; i++ ) {
        if ( left[i] - right[i + 1] > widthaver ) {
            count--;
            for( int j = i + 1; j < count; j++ ) {
                width[j] = width[j + 1];
                left[j] = left[j + 1];
                right[j] = right[j + 1];

            }
            i--;
        }
    }
    finleft = left[0];
    finright = right[0];
    for( int i = 1; i < count; i++ ) {
        finleft = finleft < left[i] ? finleft : left[i];
        finright = finright > right[i] ? finright : right[i];
    }

    for( int i = 0; i < head2.biHeight; i++ ) {
        outr[i] = 0;
        for( int j = finleft; j <= finright; j++ ) {
            outr[i] += in( j, i );
        }
        outr[i] /= 255;
    }

    for( int repeat = 0; repeat < 3; repeat ++ ) {
        int capac = 0;
        for( int i = 0; i < head2.biHeight; i++ ) {
            capac = outr[i] = ( outr[i] + capac ) / 2;
        }
        capac = 0;
        for( int i = head2.biHeight - 1; i >= 0; i-- ) {
            capac = outr[i] = ( outr[i] + capac ) / 2;
        }
    }

    int minoutr;
    minoutr = outr[0];
    for( int k = 1; k < head2.biHeight - 1; k++ ) {
        minoutr = minoutr < outr[k] ? minoutr : outr[k];
    }
    minoutr += 5;

    count = 0;
    for( int d = head2.biHeight - 2; d > 0; d-- ) {
        if ( outr[d] > minoutr ) {
            if ( outr[d + 1] <= minoutr ) {
                top[count] = d;
                count++;
            }
        }
    }
    count = 0;

    for( int d = head2.biHeight - 2; d > 0; d-- ) {
        if ( outr[d] <= minoutr ) {
            if ( outr[d + 1] > minoutr ) {
                bottom[count] = d;
                count++;
            }
        }
    }

    delete[] out;
    delete[] outr;
}
