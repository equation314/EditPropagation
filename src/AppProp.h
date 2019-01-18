#ifndef APP_PROP_H
#define APP_PROP_H

#include "EditPropagation.h"

class AppProp : public EditPropagation
{
public:
    AppProp(const char* origImage, const char* userInput);
    virtual ~AppProp();

    cv::Mat getEditedImage(int outputId);

private:
};

#endif // APP_PROP_H
