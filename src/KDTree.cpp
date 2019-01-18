#include <iostream>

#include "Config.h"
#include "EditsSolver.h"
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

    vector<pair<Point, NeighborCell>> all_corners;
    for (auto cell : m_cells)
    {
        for (int i = 0; i < 1 << DIM; i++)
            all_corners.push_back(make_pair(cell->cornerPoint(i), NeighborCell(cell, i)));
    }
    sort(all_corners.begin(), all_corners.end());

    for (int i = 0, j, n = all_corners.size(); i < n; i = j)
    {
        int s = 0;
        CornerPoint* corner = new CornerPoint(all_corners[i].first);
        for (j = i; j < n && all_corners[j].first == all_corners[i].first; j++)
        {
            auto neighbor_cell = all_corners[j].second;
            s += neighbor_cell.cell->r - neighbor_cell.cell->l;
            corner->addNeighborCell(neighbor_cell);
        }

        if (s)
            m_corners.push_back(corner);
        else
            delete corner;
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

DoubleArray KDTree::getClustersEditsImage(const DoubleArray& e) const
{
    assert(m_corners.size() == e.size());

    DoubleArray img(m_n, 0);
    for (int i = 0; i < m_corners.size(); i++)
    {
        auto corner = m_corners[i];
        for (auto j = corner->neighborCellBegin(); j != corner->neighborCellEnd(); j++)
        {
            auto cell = j->cell;
            for (int k = cell->l; k < cell->r; k++)
                img[m_points[k]->index()] += e[i];
        }
    }
    return img;
}

void KDTree::solveCornerEdits(const DoubleArray& userW, const DoubleArray& userG)
{
    DoubleArray w, g, mu;
    FeatureVecArray fvs;
    for (auto corner : m_corners)
    {
        double w_up = 0;
        double w_down = 0;
        double g_up = 0;
        for (auto i = corner->neighborCellBegin(); i != corner->neighborCellEnd(); i++)
        {
            auto cell = i->cell;
            int corner_index = i->corner_index;
            for (int j = cell->l; j < cell->r; j++)
            {
                double s = 1;
                for (int k = 0; k < DIM; k++)
                    if ((corner_index >> k) & 1)
                        s *= (m_points[j]->x[k] - cell->lower[k]) / cell->len(k);
                    else
                        s *= (cell->upper[k] - m_points[j]->x[k]) / cell->len(k);

                int index = m_points[j]->index();
                w_up += s * userW[index];
                w_down += s;
                g_up += s * userG[index] * userW[index];
            }
        }
        w.push_back(w_down == 0 ? 0 : w_up / w_down);
        g.push_back(w_up == 0 ? 0 : g_up / w_up);
        mu.push_back(w_down == 0 ? 1e-18 : w_down);
        fvs.push_back(FeatureVec(corner->x));
    }

    m_corner_edits = EditsSolver::solve(w, g, mu, fvs);
}

int KDTree::m_corner_index(const Point& p)
{
    CornerPoint corner(p);
    auto iter = lower_bound(m_corners.begin(), m_corners.end(), &corner, [](const CornerPoint* a, const CornerPoint* b) {
        return *a < *b;
    });
    if (iter == m_corners.end() || !(p == **iter))
        return -1;
    else
        return iter - m_corners.begin();
}

void KDTree::m_adjustTJunctions(Node* p)
{
    if (p->lc == nullptr || p->rc == nullptr)
        return;

    int k = p->k;
    for (int i = 0; i < 1 << DIM; i++)
        if (((i >> k) & 1) == 0)
        {
            int ia, ib, ic;
            Point pa = p->cornerPoint(i);
            Point pb = p->cornerPoint(i | (1 << k));
            Point pc = pa;
            pc.x[k] = (p->lower[k] + p->upper[k]) / 2;
            if ((ic = m_corner_index(pc)) > -1)
            {
                ia = m_corner_index(pa);
                ib = m_corner_index(pb);
                if (ia == -1 || ib == -1)
                    continue;
                assert(ia != -1 && ib != -1);

                int dim = DIM;
                for (int t = 0; t < DIM; t++)
                    if (pc.x[t] == m_root->lower[t] || pc.x[t] == m_root->upper[t])
                        dim--;
                if (m_corners[ic]->neighborCellsCount() < (1 << dim))
                    m_corner_edits[ic] = (m_corner_edits[ia] + m_corner_edits[ib]) / 2;
            }
        }
    m_adjustTJunctions(p->lc);
    m_adjustTJunctions(p->rc);
}

void KDTree::interpolation()
{
    m_adjustTJunctions(m_root);

    m_final_edits = DoubleArray(m_n);
    for (int t = 0; t < m_corners.size(); t++)
    {
        auto corner = m_corners[t];
        double e = m_corner_edits[t];
        for (auto i = corner->neighborCellBegin(); i != corner->neighborCellEnd(); i++)
        {
            auto cell = i->cell;
            int corner_index = i->corner_index;
            for (int j = cell->l; j < cell->r; j++)
            {
                double s = 1;
                for (int k = 0; k < DIM; k++)
                    if ((corner_index >> k) & 1)
                        s *= (m_points[j]->x[k] - cell->lower[k]) / cell->len(k);
                    else
                        s *= (cell->upper[k] - m_points[j]->x[k]) / cell->len(k);
                m_final_edits[m_points[j]->index()] += s * e;
            }
        }
    }
}
