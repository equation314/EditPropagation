#include "AppProp.h"
#include "EditsSolver.h"

AppProp::AppProp(const char* origImage, const char* userInput)
    : EditPropagation(origImage, userInput)
{
}

AppProp::~AppProp()
{
}

void AppProp::work()
{
    DoubleArray e = EditsSolver::solve(m_user_w, m_user_g, m_fv);

    cv::Mat img = array2image(e, m_h, m_w);
    cv::imshow("edits", img);
    cv::waitKey(0);
}
