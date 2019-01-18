#include <cstdlib>

#include "AppProp.h"
#include "AppPropKDTree.h"
#include "Config.h"

void usage(char* self)
{
    printf("Usage %s <original_image> <user_input> <algorithm> [<output_id>]\n", self);
}

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        usage(argv[0]);
        return 0;
    }

    char* origin = argv[1];
    char* input = argv[2];
    char* algo = argv[3];
    int output_id = 0;
    if (argc > 4)
        output_id = argv[4][0] - 48;

    Config::loadFrom("../config.json");

    srand(2333);
    EditPropagation* ep;

    switch (algo[0])
    {
    case '0':
        ep = new AppProp(origin, input);
        break;
    case '1':
        ep = new AppPropKDTree(origin, input);
        break;
    default:
        usage(argv[0]);
        return 0;
    }

    char fname[256];
    cv::Mat img = ep->getEditedImage(output_id);
    sprintf(fname, "%02d_%s_final.png", output_id, algo[0] == '0' ? "app_prop" : "kd");
    cv::imwrite(fname, img);
    cv::imshow("final", img);
    cv::waitKey(0);

    delete ep;

    return 0;
}
