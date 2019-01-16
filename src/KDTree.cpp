#include <iostream>

#include "KDTree.h"

using namespace std;

inline std::ostream& operator<<(std::ostream& out, const Point& p)
{
    out << '(';
    for (int i = 0; i < KD - 1; i++)
        out << p.x[i] << ", ";
    return out << p.x[KD - 1] << ')';
}

inline std::ostream& operator<<(std::ostream& out, const Node& node)
{
    out << node.l << ' ' << node.r << ' ' << node.k << ' ';
    for (int i = 0; i < KD; i++)
        out << '[' << node.lower[i] << ", " << node.upper[i] << "] ";
    return out << node.center() << ' ' << node.size2();
}

KDTree::KDTree()
    : m_root(nullptr)
{
}

KDTree::~KDTree()
{
    m_destory(m_root);
}

void KDTree::m_build(Node* p, int kk)
{
    int k = kk % KD;
    p->k = k;

    if (p->is_leaf())
    {
        m_c++;
        return;
    }

    double mid = (p->lower[k] + p->upper[k]) / 2;
    int l = p->l, r = p->r;
    int i = l, j = r - 1;

    for (; i <= j;)
    {
        while (i <= j && m_points[i].x[k] < mid)
            i++;
        while (i <= j && m_points[j].x[k] > mid)
            j--;
        if (i <= j)
        {
            swap(m_points[i], m_points[j]);
            i++, j--;
        }
    }

    Node *lc = new Node(l, i), *rc = new Node(i, r);
    for (int i = 0; i < KD; i++)
        if (i != k)
        {
            lc->lower[i] = rc->lower[i] = p->lower[i];
            lc->upper[i] = rc->upper[i] = p->upper[i];
        }
        else
        {
            lc->lower[i] = p->lower[i], lc->upper[i] = mid;
            rc->lower[i] = mid, rc->upper[i] = p->upper[i];
        }

    p->lc = lc, p->rc = rc;
    m_build(lc, (kk + 1));
    m_build(rc, (kk + 1));
}

void KDTree::m_destory(Node* p)
{
    if (p == nullptr)
        return;
    m_destory(p->lc);
    m_destory(p->rc);
    delete p;
}

void KDTree::build()
{
    m_n = m_points.size();
    m_c = 0;
    m_root = new Node(0, m_n);
    for (int i = 0; i < KD; i++)
        m_root->lower[i] = 0, m_root->upper[i] = 1;
    cout << m_n << endl;

    m_build(m_root, 0);
    cout << m_c << endl;
}
