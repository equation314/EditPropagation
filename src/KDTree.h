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

    Point center() const;

    double size2() const;

    Point cornerPoint(int index) const;

    int l, r, k;
    Node *lc, *rc;
    VectorK lower, upper;
};

class CornerPoint : public Point
{
public:
    CornerPoint(const Point& p)
        : Point(p) {}

    int neighborCellsCount() const { return m_neighbor_cells.size(); }
    void addNeighborCell(const Node* cell) { m_neighbor_cells.push_back(cell); }

private:
    std::vector<const Node*> m_neighbor_cells;
};

class KDTree
{
public:
    KDTree();
    virtual ~KDTree();

    void build();

    void insert(const PixelPoint* p) { m_points.push_back(p); }

    DoubleArray getClustersImage() const;

private:
    int m_n, m_c;
    Node* m_root;
    NearestNeighbor* m_nn_tree;
    std::vector<const PixelPoint*> m_points;
    std::vector<const Node*> m_cells;
    std::vector<const CornerPoint*> m_corners;

    void m_build(Node* p, int k);
    void m_destory(Node* p);
};

#endif // KD_TREE_H
