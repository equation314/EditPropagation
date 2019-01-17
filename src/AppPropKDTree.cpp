#include "AppPropKDTree.h"
#include "KDTree.h"

AppPropKDTree::AppPropKDTree(const char* origImage, const char* userInput)
    : EditPropagation(origImage, userInput)
{
}

AppPropKDTree::~AppPropKDTree()
{
}

void AppPropKDTree::work()
{
    KDTree* tree = new KDTree();
    for (int i = 0; i < m_n; i++)
    {
        double x[5] = {m_fv[i].f[0], m_fv[i].f[1], m_fv[i].f[2], m_fv[i].x[0], m_fv[i].x[1]};
        tree->insert(new PixelPoint(x, i, m_user_w[i] == 1));
    }

    tree->build();
    tree->solveCornerEdits(m_user_w, m_user_g);

    DoubleArray arr = tree->getClustersEditsImage(tree->cornerEdits());
    // DoubleArray arr = tree->getClustersImage();
    cv::Mat img = array2image(arr, m_h, m_w);
    cv::imwrite("clusters.png", img);
    cv::imshow("clusters", img);
    cv::waitKey(0);

    delete tree;
}
