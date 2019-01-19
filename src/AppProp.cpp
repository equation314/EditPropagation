#include "AppProp.h"
#include "EditsSolver.h"

AppProp::AppProp(const char* origImage, const char* userInput, bool isVideo)
    : EditPropagation(origImage, userInput, isVideo)
{
}

AppProp::~AppProp()
{
}

FrameArray AppProp::getEditedImage(int outputId)
{
    DoubleArray e = EditsSolver::solve(m_user_w, m_user_g, m_fv);

    char fname[256];
    FrameArray edits = array2image(e, m_h, m_w, true);
    sprintf(fname, "%02d_app_prop_edits", outputId);
    saveFrameArray(fname, edits);

    return applyEdits(e);
}
