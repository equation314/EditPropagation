#include "AppPropKDTree.h"
#include "KDTree.h"

AppPropKDTree::AppPropKDTree(const char* origImage, const char* userInput, bool isVideo)
    : EditPropagation(origImage, userInput, isVideo)
{
}

AppPropKDTree::~AppPropKDTree()
{
}

FrameArray AppPropKDTree::getEditedImage(int outputId)
{
    KDTree* tree = new KDTree();
    for (int i = 0; i < m_n; i++)
    {
        double x[DIM] = {m_fv[i].f[0], m_fv[i].f[1], m_fv[i].f[2], m_fv[i].x[0], m_fv[i].x[1], m_fv[i].t};
        tree->insert(new PixelPoint(x, i, m_user_w[i] == 1));
    }
    m_fv.clear();

    tree->build();
    tree->solveCornerEdits(m_user_w, m_user_g);
    m_user_w.clear(), m_user_g.clear();
    tree->interpolation();

    char fname[256];
    DoubleArray arr = tree->getClustersImage();
    FrameArray img = array2image(arr, m_h, m_w);
    sprintf(fname, "%02d_kd_clusters", outputId);
    saveFrameArray(fname, img);

    // arr = tree->getClustersEditsImage(tree->cornerEdits());
    // img = array2image(arr, m_h, m_w);
    // sprintf(fname, "%02d_kd_edit_clusters", outputId);
    // saveFrameArray(fname, img);

    arr = tree->getFinalEdits();
    img = array2image(arr, m_h, m_w, true);
    sprintf(fname, "%02d_kd_edits", outputId);
    saveFrameArray(fname, img);

    delete tree;

    return applyEdits(arr);
}
