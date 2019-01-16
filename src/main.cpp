#include <cstdlib>

#include "AppProp.h"
#include "AppPropKDTree.h"
#include "Config.h"

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

    Config::loadFrom("../config.json");

    srand(2333);
    // EditPropagation* ep = new AppProp(origin, input);
    EditPropagation* ep = new AppPropKDTree(origin, input);
    ep->work();

    return 0;
}
