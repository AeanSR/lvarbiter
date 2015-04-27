int zishu = 0;
int zifur[256];
int ziful[256];

void zifufenge(int chang, int kuan, int* in) {
    int* zimu = new int[chang];

    for( int i = 0; i < chang; i++ ) {
        zimu[i] = 0;
        for( int j = 0; j < kuan; j++ ) {
            zimu[i] += in[i + j * chang];
        }
        zimu[i] /= 255;
    }
    int minzimu = zimu[0];
    for( int i = 0; i < chang; i++ ) {
        minzimu = minzimu < zimu[i] ? minzimu : zimu[i];

    }
    minzimu += 3;

    zishu = 0;
    for( int i = 0; i < chang - 2; i++ ) {
        if ( zimu[i] <= minzimu ) {
            if ( zimu[i + 1] > minzimu ) {
                ziful[zishu] = i;
                zishu++;
            }
        }
    }
    zishu = 0;

    for( int i = 1; i < chang - 2; i++ ) {
        if( zimu[i] > minzimu ) {
            if( zimu[i - 1] <= minzimu ) {
                zifur[zishu] = i;
                zishu++;
            }
        }
    }
    delete[] zimu;
}
