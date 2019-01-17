#include <iostream>

#include "Config.h"
#include "KDTree.h"
#include "NearestNeighbor.h"

using namespace std;

inline std::ostream& operator<<(std::ostream& out, const Point& p)
{
    out << '(';
    for (int i = 0; i < DIM - 1; i++)
        out << p.x[i] << ", ";
    return out << p.x[DIM - 1] << ')';
}

inline std::ostream& operator<<(std::ostream& out, const Node& node)
{
    out << node.l << ' ' << node.r << ' ' << node.k << ' ';
    for (int i = 0; i < DIM; i++)
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

void KDTree::m_build(Node* p, int k)
{
    p->k = k;
    if (p->l >= p->r || sqrt(p->size2()) - sqrt(m_nn_tree->nearestDist2(p->center().x)) < Config::kd_tree_eta)
    {
        m_cells.push_back(p);
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
    for (int i = 0; i < DIM; i++)
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
    m_build(lc, (k + 1) % DIM);
    m_build(rc, (k + 1) % DIM);
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
    m_root = new Node(0, m_n);
    m_cells.clear();
    for (int i = 0; i < DIM; i++)
        m_root->lower[i] = 0, m_root->upper[i] = 1;
    cout << m_n << endl;

    m_nn_tree = new NearestNeighbor();
    for (auto p : m_points)
        if (p.is_user_edits) m_nn_tree->insert(p);
    m_nn_tree->build();

    m_build(m_root, 0);
    m_c = m_cells.size();
    cout << m_c << endl;

    delete m_nn_tree;
}
