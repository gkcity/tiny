#include <stdio.h>

int main(int argc, char *argv[])
{

    void * ptr_array[256];

    size_t i;

    int idx;

    printf( "Size of umm_heap is %i\n", sizeof(umm_heap)       );
    printf( "Size of header   is %i\n", sizeof(umm_heap[0])    );
    printf( "Size of nblock   is %i\n", sizeof(umm_heap[0].header.used.next) );
    printf( "Size of pblock   is %i\n", sizeof(umm_heap[0].header.used.prev) );
    printf( "Size of nfree    is %i\n", sizeof(umm_heap[0].body.free.next)   );
    printf( "Size of pfree    is %i\n", sizeof(umm_heap[0].body.free.prev)   );

    memset( umm_heap, 0, sizeof(umm_heap) );

    umm_info( NULL, 1 );

    for( idx=0; idx<256; ++idx )
        ptr_array[idx] = (void *)NULL;

    for( idx=0; idx<6553500; ++idx ) {
        i = rand()%256;

        switch( rand() % 16 ) {

            case  0:
            case  1:
            case  2:
            case  3:
            case  4:
            case  5:
            case  6: ptr_array[i] = umm_realloc(ptr_array[i], 0);
                break;
            case  7:
            case  8: ptr_array[i] = umm_realloc(ptr_array[i], rand()%40 );
                break;

            case  9:
            case 10:
            case 11:
            case 12: ptr_array[i] = umm_realloc(ptr_array[i], rand()%100 );
                break;

            case 13:
            case 14: ptr_array[i] = umm_realloc(ptr_array[i], rand()%200 );
                break;

            default: ptr_array[i] = umm_realloc(ptr_array[i], rand()%400 );
                break;
        }

    }

    umm_info( NULL, 1 );
}
