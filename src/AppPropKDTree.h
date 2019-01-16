#ifndef APP_PROP_KD_TREE_H
#define APP_PROP_KD_TREE_H

#include "EditPropagation.h"

class AppPropKDTree : public EditPropagation
{
public:
    AppPropKDTree(const char* origImage, const char* userInput);
    virtual ~AppPropKDTree();

    void work();

private:
};

#endif // APP_PROP_KD_TREE_H
