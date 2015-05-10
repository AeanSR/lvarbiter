#include <Windows.h>
#include <cmath>
#include <cstdio>
#include <algorithm>
extern BITMAPFILEHEADER head1;
extern BITMAPINFOHEADER head2;
extern RGBTRIPLE* buf1;
extern RGBTRIPLE* buf3;
#define in(x,y) buf3[(y) * head2.biWidth + (x)].rgbtBlue
#define img1(x,y) buf1[(y) * head2.biWidth + (x)]
int give_me_a_face(int left, int right, int bottom, int top, int line){
    int* proj = new int[right-left];
    for(int x = left; x < right; x++){
        proj[x-left] = 0;
        for(int y = bottom; y < top; y++){
            proj[x-left] += in(x,y);
        }
        proj[x-left] /= 255;
    }
	for( int repeat = 0; repeat < 3; repeat ++ ) {
        int capac = 0;
        for( int i = 0; i < right-left; i++ ) {
            capac = proj[i] = ( proj[i] + capac ) / 2;
        }
        capac = 0;
        for( int i = right-left - 1; i >= 0; i-- ) {
            capac = proj[i] = ( proj[i] + capac ) / 2;
        }
    }
    int maximas[500];
    int count = 0;
    int state = 0;
    for(int x = 1; x < right-left; x++){
        if(proj[x-1] < proj[x]){
            state = 1;
        } else if(proj[x-1] == proj[x]){
        } else if(proj[x-1] > proj[x]){
            if(state == 1)
                maximas[count++] = x-1;
            state = -1;
        }
    }

    int max11[11] = {0};
    for(int i = 0; i < count; i++){
        for(int j = 0; j < 11; j++){
            if(proj[maximas[i]] > proj[max11[j]]){
                for(int k = 10; k > j; k--){
                    max11[k] = max11[k-1];
                }
                max11[j] = maximas[i];
                break;
            }
        }
    }

    char buf[256] = {0};
	sprintf(buf, "result\\zhaofucksky.bmp");
	FILE* f = fopen( buf, "wb" );
    for( int i = 0; i < head2.biWidth; i++ ) {
        for( int j = 0; j < head2.biHeight; j++ ) {
            img1( i, j ).rgbtBlue = img1( i, j ).rgbtGreen = img1( i, j ).rgbtRed = in(i,j);
        }
    }
    for(int i = 0; i < 11; i++){
        for(int j = 0; j < head2.biHeight; j++){
            img1(max11[i]+left, j) = (RGBTRIPLE){0,0,255};
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

    std::sort(&max11[0],&max11[0] + 11);

    int face[9];
    int stdface[][9] ={
        { 1729,2396,2851,5470,6157,6622,7067,7745,9332, },
        { 1236,2036,2763,3236,4945,5818,6254,7490,8836, },
        { 689,1039,2949,3231,3717,5706,6135,8757,9401, },
        { 1571,1995,2169,2306,4002,4301,5548,5847,8615, },
        { 964,3446,4255,4936,6609,7092,8113,9021,9517, },
        { 802,2105,2857,4035,4461,5914,6541,8320,9147, },
    };
    f = fopen("result\\zhaofucksky.txt","wb");
    for(int i = 1; i < 10; i++){
        fprintf(f, "%d,", (face[i-1] = (max11[i]-max11[0])*10000/(max11[10]-max11[0])));
    }
    fclose(f);

    int dev = 0;

    for(int i = 0; i < 9; i++){
        dev+=(face[i]-stdface[line][i])*(face[i]-stdface[line][i]);
    }
    dev/=9;
    dev=sqrt(dev);
    dev=sqrt(dev);
    dev=100-dev;
    printf("Similarity %d%%\n", dev);

    delete[] proj;
    return dev;
}
