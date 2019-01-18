#include "AppPropKDTree.h"
#include "KDTree.h"

AppPropKDTree::AppPropKDTree(const char* origImage, const char* userInput)
    : EditPropagation(origImage, userInput)
{
}

AppPropKDTree::~AppPropKDTree()
{
}

cv::Mat AppPropKDTree::getEditedImage(int outputId)
{
    KDTree* tree = new KDTree();
    for (int i = 0; i < m_n; i++)
    {
        double x[5] = {m_fv[i].f[0], m_fv[i].f[1], m_fv[i].f[2], m_fv[i].x[0], m_fv[i].x[1]};
        tree->insert(new PixelPoint(x, i, m_user_w[i] == 1));
    }

    tree->build();
    tree->solveCornerEdits(m_user_w, m_user_g);
    tree->interpolation();

    char fname[256];
    DoubleArray arr = tree->getClustersImage();
    cv::Mat img = array2image(arr, m_h, m_w);
    sprintf(fname, "%02d_kd_clusters.png", outputId);
    cv::imwrite(fname, img);
    cv::imshow("clusters", img);

    arr = tree->getClustersEditsImage(tree->cornerEdits());
    img = array2image(arr, m_h, m_w);
    sprintf(fname, "%02d_kd_edit_clusters.png", outputId);
    cv::imwrite(fname, img);
    cv::imshow("edit_clusters", img);

    arr = tree->getFinalEdits();
    img = array2image(arr, m_h, m_w, true);
    sprintf(fname, "%02d_kd_edits.png", outputId);
    cv::imwrite(fname, img);
    cv::imshow("edits", img);

    delete tree;

    return applyEdits(arr);
}
