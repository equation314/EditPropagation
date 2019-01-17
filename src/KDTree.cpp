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

Point Node::center() const
{
    Point c;
    for (int i = 0; i < DIM; i++)
        c.x[i] = (lower[i] + upper[i]) / 2;
    return c;
}

double Node::size2() const
{
    double s = 0;
    for (int i = 0; i < DIM; i++)
        s += (upper[i] - lower[i]) * (upper[i] - lower[i]);
    return s;
}

Point Node::cornerPoint(int index) const
{
    Point c;
    for (int i = 0; i < DIM; i++)
        c.x[i] = ((index >> i) & 1) ? upper[i] : lower[i];
    return c;
}

KDTree::KDTree()
    : m_root(nullptr), m_nn_tree(nullptr)
{
}

KDTree::~KDTree()
{
    m_destory(m_root);
    if (m_nn_tree)
        delete m_nn_tree;
    for (auto p : m_points)
        delete p;
    for (auto c : m_corners)
        delete c;
}

void KDTree::m_build(Node* p, int k)
{
    p->k = k;
    Point center = p->center();
    if (p->l >= p->r || sqrt(p->size2()) - sqrt(m_nn_tree->nearestDist2(&center)) < Config::kd_tree_eta)
    {
        m_cells.push_back(p);
        return;
    }

    double mid = (p->lower[k] + p->upper[k]) / 2;
    int l = p->l, r = p->r;
    int i = l, j = r - 1;

    for (; i <= j;)
    {
        while (i <= j && m_points[i]->x[k] < mid)
            i++;
        while (i <= j && m_points[j]->x[k] > mid)
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
    {
        double mi = 1e9, ma = -1e9;
        for (auto p : m_points)
            mi = min(mi, p->x[i]), ma = max(ma, p->x[i]);
        m_root->lower[i] = mi, m_root->upper[i] = ma;
    }
    cout << m_n << endl;
    cout << *m_root << endl;

    m_nn_tree = new NearestNeighbor();
    for (auto p : m_points)
        if (p->isUserEdits()) m_nn_tree->insert(p);
    m_nn_tree->build();

    m_build(m_root, 0);
    m_c = m_cells.size();
    cout << m_c << endl;

    vector<pair<Point, const Node*>> all_corners;
    for (auto cell : m_cells)
    {
        for (int i = 0; i < 1 << DIM; i++)
            all_corners.push_back(make_pair(cell->cornerPoint(i), cell));
    }
    sort(all_corners.begin(), all_corners.end());

    for (int i = 0, j, n = all_corners.size(); i < n; i = j)
    {
        CornerPoint* corner = new CornerPoint(all_corners[i].first);
        for (j = i; j < n && all_corners[j].first == all_corners[i].first; j++)
            corner->addNeighborCell(all_corners[j].second);

        m_corners.push_back(corner);
    }
    cout << all_corners.size() << endl;
    cout << m_corners.size() << endl;
}

DoubleArray KDTree::getClustersImage() const
{
    DoubleArray img(m_n);
    for (auto cell : m_cells)
    {
        Point center = cell->center();
        for (int i = cell->l; i < cell->r; i++)
            img[m_points[i]->index()] = -sqrt(m_nn_tree->nearestDist2(&center));
    }

    return img;
}
