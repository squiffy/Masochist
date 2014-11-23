#include <stdio.h>
#include <stdlib.h>

int kas_info(int selector, void *value, size_t *size);

int main() {

    uint64_t slide = 0;
    size_t size = sizeof(slide);
    kas_info(0, &slide, &size);
    printf("Kernel slide: 0x%llx\n", slide);

}
