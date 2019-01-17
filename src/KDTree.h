#ifndef KD_TREE_H
#define KD_TREE_H

#include <cmath>
#include <vector>

#include "Common.h"

class NearestNeighbor;

struct Node
{
    Node(int l, int r)
        : l(l), r(r), k(0), lc(nullptr), rc(nullptr)
    {
    }

    Point center() const
    {
        Point c;
        for (int i = 0; i < DIM; i++)
            c.x[i] = (lower[i] + upper[i]) / 2;
        return c;
    }

    double size2() const
    {
        double s = 0;
        for (int i = 0; i < DIM; i++)
            s += (upper[i] - lower[i]) * (upper[i] - lower[i]);
        return s;
    }

    int l, r, k;
    Node *lc, *rc;
    VectorK lower, upper;
};

class KDTree
{
public:
    KDTree();
    virtual ~KDTree();

    void build();

    void insert(const Point& p) { m_points.push_back(p); }

private:
    int m_n, m_c;
    Node* m_root;
    NearestNeighbor* m_nn_tree;
    std::vector<Point> m_points;
    std::vector<Node*> m_cells;

    void m_build(Node* p, int k);
    void m_destory(Node* p);
};

#endif // KD_TREE_H
