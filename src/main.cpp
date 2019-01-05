#include <cstdio>
#include <cstdlib>

#include "AppProp.h"

void usage(char* self)
{
    printf("Usage %s <original_image> <user_input>\n", self);
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        usage(argv[0]);
        return 0;
    }

    char* origin = argv[1];
    char* input = argv[2];

    srand(2333);
    EditPropagation* ep = new AppProp(origin, input);
    ep->work();

    return 0;
}
