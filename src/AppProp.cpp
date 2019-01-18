#include "AppProp.h"
#include "EditsSolver.h"

AppProp::AppProp(const char* origImage, const char* userInput)
    : EditPropagation(origImage, userInput)
{
}

AppProp::~AppProp()
{
}

cv::Mat AppProp::getEditedImage(int outputId)
{
    DoubleArray e = EditsSolver::solve(m_user_w, m_user_g, m_fv);

    char fname[256];
    cv::Mat img = array2image(e, m_h, m_w, true);
    sprintf(fname, "%02d_app_prop_edits.png", outputId);
    cv::imwrite(fname, img);
    cv::imshow("edits", img);

    return applyEdits(e);
}
