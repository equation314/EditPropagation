#include <cstdio>
#include <cstdlib>

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

    puts(origin);
    puts(input);

    return 0;
}
