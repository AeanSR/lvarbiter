#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <Windows.h>
#include <assert.h>
#include <vector>
#include <math.h>
#include <algorithm>
#include <functional>
int outidx;

BITMAPFILEHEADER head1;
BITMAPINFOHEADER head2;
RGBTRIPLE* orig = NULL;
RGBTRIPLE* buf1 = NULL;
RGBTRIPLE* buf2 = NULL;
RGBTRIPLE* buf3 = NULL;
char* closed = NULL;
#define ori(x,y) orig[(y) * head2.biWidth + (x)]
#define img1(x,y) buf1[(y) * head2.biWidth + (x)]
#define img2(x,y) buf2[(y) * head2.biWidth + (x)]
void line_slice();
extern int count;
extern int top[500];
extern int bottom[500];
extern int finleft;
extern int finright;

void zifufenge( int chang, int kuan, int* in );
void give_me_a_face(int left, int right, int bottom, int top);
extern int zishu;
extern int zifur[256];
extern int ziful[256];

int rec( size_t x, size_t y ) {
    if ( img2( x, y ).rgbtBlue < 255 ) return 0;
    if ( closed[y * head2.biWidth + x] ) return 0;
    int ret = 1;
    closed[y * head2.biWidth + x] = 1;
    if ( x > 0 )
        ret += rec( x - 1, y );
    if ( y > 0 )
        ret += rec( x, y - 1 );
    if ( ( int )x + 1 < head2.biWidth )
        ret += rec( x + 1, y );
    if ( ( int )y + 1 < head2.biHeight )
        ret += rec( x, y + 1 );
    return ret;
}

void rec2( size_t x, size_t y ) {
    if ( img2( x, y ).rgbtBlue < 255 ) return;
    if ( !closed[y * head2.biWidth + x] ) return;
    closed[y * head2.biWidth + x] = 0;
    if ( x > 0 )
        rec2( x - 1, y );
    if ( y > 0 )
        rec2( x, y - 1 );
    if ( ( int )x + 1 < head2.biWidth )
        rec2( x + 1, y );
    if ( ( int )y + 1 < head2.biHeight )
        rec2( x, y + 1 );
    img2( x, y ).rgbtBlue = 0;
    return;
}

int main( int argc, char** argv ) {
    /* Read from file. */
    if ( argc < 2 ) {
        printf( "nothing to recognize!.\r\n" );
        return -1;
    }

    FILE* f = fopen( argv[1], "rb" );

    fread( &head1, sizeof head1, 1, f );
    fread( &head2, sizeof head2, 1, f );
    assert( sizeof( RGBTRIPLE ) == 3 );
    orig = ( RGBTRIPLE* )malloc( head2.biWidth * head2.biHeight * sizeof( RGBTRIPLE ) + 12 );
    buf1 = ( RGBTRIPLE* )malloc( head2.biWidth * head2.biHeight * sizeof( RGBTRIPLE ) + 12 );
    buf2 = ( RGBTRIPLE* )calloc( head2.biWidth * head2.biHeight, sizeof( RGBTRIPLE ) + 12 );
    buf3 = ( RGBTRIPLE* )calloc( head2.biWidth * head2.biHeight, sizeof( RGBTRIPLE ) + 12 );
    fseek( f, head1.bfOffBits, SEEK_SET );
    RGBTRIPLE* mb = buf1;
    for ( int i = 0; i < head2.biHeight; i++ ) {
        fread( mb, sizeof( RGBTRIPLE ), head2.biWidth, f );
        mb += head2.biWidth;
        fread( mb, 1, ( sizeof( RGBTRIPLE ) * head2.biWidth ) % 4 ? 4 - ( ( sizeof( RGBTRIPLE ) * head2.biWidth ) % 4 ) : 0, f );
    }
    memcpy( orig, buf1, head2.biWidth * head2.biHeight * sizeof( RGBTRIPLE ) + 12 );
    fclose( f );

    for ( int y = 0; y < head2.biHeight; y++ )
        for ( int x = 0; x < head2.biWidth; x++ ) {
            BYTE t1 = img1( x, y ).rgbtRed > img1( x, y ).rgbtBlue ? img1( x, y ).rgbtRed - img1( x, y ).rgbtBlue : 0;
            BYTE t2 = img1( x, y ).rgbtGreen > img1( x, y ).rgbtBlue ? img1( x, y ).rgbtGreen - img1( x, y ).rgbtBlue : 0;
            img1( x, y ).rgbtGreen = t1 * t2 / 255;
            //img1(x,y).rgbtGreen = (int)img1(x,y).rgbtGreen * img1(x,y).rgbtGreen / 255;
        }

    for ( int y = 0; y < head2.biHeight; y++ )
        for ( int x = 0; x < head2.biWidth; x++ ) {
            BYTE filter[5];
            filter[0] = ( x ? img1( x - 1, y ).rgbtGreen : img1( x, y ).rgbtGreen );
            filter[1] = ( img1( x, y ).rgbtGreen );
            filter[2] = ( x == head2.biWidth - 1 ? img1( x, y ).rgbtGreen : img1( x + 1, y ).rgbtGreen );
            filter[3] = ( y ? img1( x, y - 1 ).rgbtGreen : img1( x, y ).rgbtGreen );
            filter[4] = ( y == head2.biHeight - 1 ? img1( x, y ).rgbtGreen : img1( x, y + 1 ).rgbtGreen );
            std::sort( filter, filter + 5, std::greater<BYTE>() );
            img1( x, y ).rgbtBlue = filter[2];
        }

    for ( int y = 1; y < head2.biHeight - 1; y++ )
        for( int x = 1; x < head2.biWidth - 1; x++ ) {
            int gx = ( int )img1( x + 1, y - 1 ).rgbtBlue + 2 * ( int )img1( x + 1, y ).rgbtBlue + ( int )img1( x + 1, y + 1 ).rgbtBlue - ( int )img1( x - 1, y - 1 ).rgbtBlue - 2 * ( int )img1( x - 1, y ).rgbtBlue - ( int )img1( x - 1, y + 1 ).rgbtBlue;
            int gy = ( int )img1( x - 1, y - 1 ).rgbtBlue + 2 * ( int )img1( x, y - 1 ).rgbtBlue + ( int )img1( x + 1, y - 1 ).rgbtBlue - ( int )img1( x - 1, y + 1 ).rgbtBlue - 2 * ( int )img1( x, y + 1 ).rgbtBlue - ( int )img1( x + 1, y + 1 ).rgbtBlue;
            img2( x, y ).rgbtBlue = ( unsigned char )( int )sqrt( gx * gx + gy * gy );
        }

    for ( int y = 0; y < head2.biHeight; y++ )
        for ( int x = 0; x < head2.biWidth; x++ )
            img2( x, y ).rgbtBlue = img2( x, y ).rgbtBlue > 40 ? 255 : 0;

    for ( int i = 0; i < 3; i++ )
        for ( int y = 0; y < head2.biHeight; y++ )
            for ( int x = 0; x < head2.biWidth; x++ ) {
                BYTE filter[5];
                filter[0] = ( x ? img2( x - 1, y ).rgbtBlue : img2( x, y ).rgbtBlue );
                filter[1] = ( img2( x, y ).rgbtBlue );
                filter[2] = ( x == head2.biWidth - 1 ? img2( x, y ).rgbtBlue : img2( x + 1, y ).rgbtBlue );
                filter[3] = ( y ? img2( x, y - 1 ).rgbtBlue : img2( x, y ).rgbtBlue );
                filter[4] = ( y == head2.biHeight - 1 ? img2( x, y ).rgbtBlue : img2( x, y + 1 ).rgbtBlue );
                std::sort( filter, filter + 5, std::greater<BYTE>() );
                img2( x, y ).rgbtBlue = filter[2];
            }

    closed = ( char * )calloc( head2.biWidth * head2.biHeight, 1 );
    for ( int x = 0; x < head2.biWidth; x++ ) {
        for ( int y = 0; y < head2.biHeight; y++ ) {
            if ( img2( x, y ).rgbtBlue < 255 ) continue;
            if ( closed[y * head2.biWidth + x] ) continue;
            int ret = rec( x, y );
            if ( ret < 60 ) rec2( x, y );
        }
    }
    /*
        int mingreysum = head2.biWidth;
        int* greysum = new int[head2.biHeight];
        for (int y = 1; y < head2.biHeight - 1; y++){
            greysum[y] = 0;
            for (int x = 1; x < head2.biWidth - 1; x++){
                greysum[y] += img2(x,y).rgbtBlue;
            }
            greysum[y] /= 255;
            mingreysum = greysum[y] < mingreysum ? greysum[y] : mingreysum;
        }

        int top = 0;
        int bottom = 0;
        for (int y = head2.biHeight - 2; y; y--){
            if (!top && greysum[y] > mingreysum + 20) top = y;
            else if (top && !bottom && greysum[y] < mingreysum + 20){bottom = y; break;}
        }

        delete[] greysum;
        greysum = new int[head2.biWidth];
        mingreysum = head2.biHeight;
        for (int x = 1; x < head2.biWidth - 1; x++){
            greysum[x] = 0;
            for (int y = bottom; y < top; y++){
                greysum[x] += img2(x,y).rgbtBlue;
            }
            greysum[x] /= 255;
            mingreysum = greysum[x] < mingreysum ? greysum[x] : mingreysum;
        }

        int left = 0;
        int right = 0;
        for (int x = 1; x < head2.biWidth - 1; x++){
            if (!left && greysum[x] > mingreysum + 5) left = x;
            if (left && greysum[x] > mingreysum + 5) right = x;
        }
        right++;
    */
    line_slice();
    printf( "lines found: %d\n", count );
    printf( "finwidth: %d-%d\n", finleft, finright );
    system("rd /S /Q result");
    system("md result");
    outidx = count;
    while( outidx-- ) {
        BITMAPFILEHEADER nhead1 = head1;
        BITMAPINFOHEADER nhead2 = head2;
        nhead2.biWidth = finright - finleft;
        nhead2.biHeight = top[outidx] - bottom[outidx];
        size_t rowsize = ( 24 * nhead2.biWidth + 31 ) / 32;
        rowsize *= 4;
        nhead2.biSizeImage = rowsize * nhead2.biHeight;
        nhead1.bfSize = 54 + nhead2.biSizeImage;


        char buf[256] = {0};
        sprintf( buf, "md result\\line%d", outidx+1 );
        system(buf);
        sprintf( buf, "result\\line%d\\line%d.bmp", outidx + 1, outidx+1 );
        FILE* f = fopen( buf, "wb" );
        fwrite( &nhead1, sizeof( nhead1 ), 1, f );
        fwrite( &nhead2, sizeof( nhead2 ), 1, f );
        void * p = calloc( head1.bfOffBits, 1 );
        fwrite( p, head1.bfOffBits, 1, f );
        fseek( f, head1.bfOffBits, SEEK_SET );
        for ( int i = 0; i < nhead2.biHeight; i++ ) {
            fwrite( &ori( finleft, bottom[outidx] + i ), sizeof( RGBTRIPLE ), nhead2.biWidth, f );
            fwrite( p, 1, ( sizeof( RGBTRIPLE ) * nhead2.biWidth ) % 4 ? 4 - ( ( sizeof( RGBTRIPLE ) * nhead2.biWidth ) % 4 ) : 0, f );
        }
        fclose( f );
    }

	for ( int y = 0; y < head2.biHeight; y++ )
        for ( int x = 0; x < head2.biWidth; x++ ) {
            BYTE t1 = ori( x, y ).rgbtRed > ori( x, y ).rgbtBlue ? ori( x, y ).rgbtRed - ori( x, y ).rgbtBlue : 0;
            BYTE t2 = ori( x, y ).rgbtGreen > ori( x, y ).rgbtBlue ? ori( x, y ).rgbtGreen - ori( x, y ).rgbtBlue : 0;
            img1( x, y ).rgbtGreen = t1 * t2 / 255;
            //img1(x,y).rgbtGreen = (int)img1(x,y).rgbtGreen * img1(x,y).rgbtGreen / 255;
        }

    for ( int y = 0; y < head2.biHeight; y++ )
        for ( int x = 0; x < head2.biWidth; x++ ) {
            BYTE filter[5];
            filter[0] = ( x ? img1( x - 1, y ).rgbtGreen : img1( x, y ).rgbtGreen );
            filter[1] = ( img1( x, y ).rgbtGreen );
            filter[2] = ( x == head2.biWidth - 1 ? img1( x, y ).rgbtGreen : img1( x + 1, y ).rgbtGreen );
            filter[3] = ( y ? img1( x, y - 1 ).rgbtGreen : img1( x, y ).rgbtGreen );
            filter[4] = ( y == head2.biHeight - 1 ? img1( x, y ).rgbtGreen : img1( x, y + 1 ).rgbtGreen );
            std::sort( filter, filter + 5, std::greater<BYTE>() );
            img1( x, y ).rgbtBlue = filter[2];
        }

	for ( int y = 0; y < head2.biHeight; y++ )
        for ( int x = 0; x < head2.biWidth; x++ )
			img2(x, y).rgbtBlue = img1(x, y).rgbtBlue > 15 ? 255 : 0;

    memcpy( buf3, buf2, head2.biWidth * head2.biHeight * sizeof( RGBTRIPLE ) + 12 );

	for ( int y = 0; y < head2.biHeight; y++ )
        for ( int x = 5; x < head2.biWidth - 6; x++ ){
			if (img2(x - 2, y).rgbtBlue == 255 && img2(x - 1, y).rgbtBlue == 255 && img2(x, y).rgbtBlue == 0){
				int dist = 1;
				for (; dist < 6; dist++){
					if (img2(x + dist, y).rgbtBlue == 255 && img2(x + dist + 1, y).rgbtBlue == 255) break;
				}
				if (dist >= 6) continue;
				while (dist--){
					img2(x + dist, y).rgbtBlue = 255;
				}
				img2(x,y).rgbtBlue = 255;
			}
		}
	for ( int y = 5; y < head2.biHeight - 6; y++ )
        for ( int x = 5; x < head2.biWidth - 6; x++ ){
			if (img2(x - 2, y - 2).rgbtBlue == 255 && img2(x - 1, y - 1).rgbtBlue == 255 && img2(x, y).rgbtBlue == 0){
				int dist = 1;
				for (; dist < 6; dist++){
					if (img2(x + dist, y + dist).rgbtBlue == 255 && img2(x + dist + 1, y + dist + 1).rgbtBlue == 255) break;
				}
				if (dist >= 6) continue;
				while (dist--){
					img2(x + dist, y + dist).rgbtBlue = 255;
				}
				img2(x,y).rgbtBlue = 255;
			}
		}
	for ( int y = 5; y < head2.biHeight - 6; y++ )
        for ( int x = 5; x < head2.biWidth - 6; x++ ){
			if (img2(x - 2, y + 2).rgbtBlue == 255 && img2(x - 1, y + 1).rgbtBlue == 255 && img2(x, y).rgbtBlue == 0){
				int dist = 1;
				for (; dist < 6; dist++){
					if (img2(x + dist, y - dist).rgbtBlue == 255 && img2(x + dist + 1, y - dist - 1).rgbtBlue == 255) break;
				}
				if (dist >= 6) continue;
				while (dist--){
					img2(x + dist, y - dist).rgbtBlue = 255;
				}
				img2(x,y).rgbtBlue = 255;
			}
		}

	{	FILE* f = fopen("spec.bmp", "wb");
	for (int i = 0; i < head2.biWidth; i++) {
		for (int j = 0; j < head2.biHeight; j++) {
			img2(i, j).rgbtBlue = img2(i, j).rgbtGreen = img2(i, j).rgbtRed = img2(i, j).rgbtBlue;
		}
	}
	fwrite(&head1, sizeof head1, 1, f);
	fwrite(&head2, sizeof head2, 1, f);
	void * p = calloc(head1.bfOffBits, 1);
	fwrite(p, head1.bfOffBits, 1, f);
	fseek(f, head1.bfOffBits, SEEK_SET);
	RGBTRIPLE* mb = buf2;
	for (int i = 0; i < head2.biHeight; i++) {
		fwrite(mb, sizeof(RGBTRIPLE), head2.biWidth, f);
		mb += head2.biWidth;
		fwrite(p, 1, (sizeof(RGBTRIPLE) * head2.biWidth) % 4 ? 4 - ((sizeof(RGBTRIPLE) * head2.biWidth) % 4) : 0, f);
	}
	fclose(f);
	}

    outidx = count;
    while( outidx-- ) {
        int chang = finright - finleft;
        int kuan = top[outidx] - bottom[outidx];
        int* in = new int[chang * kuan];
        for ( int x = finleft; x < finright; x++ ) {
            for( int y = bottom[outidx]; y < top[outidx]; y++ ) {
                in[(x-finleft) + (y-bottom[outidx]) * chang] = img2( x, y ).rgbtBlue;
            }
        }
        zifufenge( chang, kuan, in );
        printf( "characters found: %d\n", zishu );
        int chridx = zishu;
        while( chridx-- ) {
            BITMAPFILEHEADER nhead1 = head1;
            BITMAPINFOHEADER nhead2 = head2;
            nhead2.biWidth = zifur[chridx] - ziful[chridx];
            nhead2.biHeight = top[outidx] - bottom[outidx];
            size_t rowsize = ( 24 * nhead2.biWidth + 31 ) / 32;
            rowsize *= 4;
            nhead2.biSizeImage = rowsize * nhead2.biHeight;
            nhead1.bfSize = 54 + nhead2.biSizeImage;


            char buf[256] = {0};
            sprintf( buf, "result\\line%d\\chr%d.bmp", outidx + 1, chridx + 1 );
            FILE* f = fopen( buf, "wb" );
            fwrite( &nhead1, sizeof( nhead1 ), 1, f );
            fwrite( &nhead2, sizeof( nhead2 ), 1, f );
            void * p = calloc( head1.bfOffBits, 1 );
            fwrite( p, head1.bfOffBits, 1, f );
            fseek( f, head1.bfOffBits, SEEK_SET );
            for ( int i = 0; i < nhead2.biHeight; i++ ) {
                fwrite( &ori( finleft + ziful[chridx], bottom[outidx] + i ), sizeof( RGBTRIPLE ), nhead2.biWidth, f );
                fwrite( p, 1, ( sizeof( RGBTRIPLE ) * nhead2.biWidth ) % 4 ? 4 - ( ( sizeof( RGBTRIPLE ) * nhead2.biWidth ) % 4 ) : 0, f );
            }
            fclose( f );
        }
        delete[] in;
    }

    give_me_a_face(finleft, finright, bottom[0], top[0]);
    return 0;
}
