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
    tree->interpolation();

    DoubleArray arr = tree->getClustersImage();
    cv::Mat img = array2image(arr, m_h, m_w);
    cv::imwrite("01_clusters.png", img);
    cv::imshow("clusters", img);

    arr = tree->getClustersEditsImage(tree->cornerEdits());
    img = array2image(arr, m_h, m_w);
    cv::imwrite("01_edit_clusters.png", img);
    cv::imshow("edit_clusters", img);

    arr = tree->getFinalEdits();
    img = array2image(arr, m_h, m_w);
    cv::imwrite("01_edits.png", img);
    cv::imshow("edits", img);

    img = apply_edits(arr);
    cv::imwrite("01_final.png", img);
    cv::imshow("final", img);

    cv::waitKey(0);

    delete tree;
}
