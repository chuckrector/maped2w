#include <stdio.h>

int main(int argc, char* argv[]) {
        int sx = 320;
        int sy = 200;
        printf("rounded X = %d\n", ((sx+15)&~15));
        printf("rounded Y = %d\n", ((sy+15)&~15));
        return 0;
}
