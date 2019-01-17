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

struct NeighborCell
{
    const Node* cell;
    int corner_index;

    NeighborCell(const Node* cell, int corner_index)
        : cell(cell), corner_index(corner_index) {}

    bool operator<(const NeighborCell& cell) const { return corner_index < cell.corner_index; }
};

class CornerPoint : public Point
{
public:
    CornerPoint(const Point& p)
        : Point(p) {}

    int neighborCellsCount() const { return m_neighbor_cells.size(); }
    void addNeighborCell(const NeighborCell& cell) { m_neighbor_cells.push_back(cell); }

    std::vector<NeighborCell>::const_iterator neighborCellBegin() const { return m_neighbor_cells.begin(); }
    std::vector<NeighborCell>::const_iterator neighborCellEnd() const { return m_neighbor_cells.end(); }

private:
    std::vector<NeighborCell> m_neighbor_cells;
};

class KDTree
{
public:
    KDTree();
    virtual ~KDTree();

    void build();

    void insert(const PixelPoint* p) { m_points.push_back(p); }

    void solveCornerEdits(const DoubleArray& userW, const DoubleArray& userG);

    void interpolation();

    DoubleArray getFinalEdits() const { return m_final_edits; }
    DoubleArray getClustersImage() const;
    DoubleArray getClustersEditsImage(const DoubleArray& e) const;

    DoubleArray cornerEdits() const { return m_corner_edits; }

private:
    int m_n;
    Node* m_root;
    NearestNeighbor* m_nn_tree;
    std::vector<const PixelPoint*> m_points;
    std::vector<const Node*> m_cells;
    std::vector<const CornerPoint*> m_corners;

    DoubleArray m_corner_edits;
    DoubleArray m_final_edits;

    void m_build(Node* p, int k);
    void m_destory(Node* p);
    void m_adjustTJunctions(Node* p);
    int m_corner_index(const Point& p);
};

#endif // KD_TREE_H
