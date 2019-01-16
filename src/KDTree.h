#ifndef KD_TREE_H
#define KD_TREE_H

#include <cmath>
#include <vector>

#include "Config.h"

const int KD = 5;

typedef double VectorK[KD];

struct Point
{
    Point(bool is_user_edits = false)
        : is_user_edits(is_user_edits) {}
    Point(const double* _x, bool is_user_edits = false)
        : is_user_edits(is_user_edits)
    {
        for (int i = 0; i < KD; i++)
            x[i] = _x[i];
    }

    VectorK x;
    bool is_user_edits;
};

struct Node
{
    Node(int l, int r)
        : l(l), r(r), k(0), lc(nullptr), rc(nullptr)
    {
    }

    Point center() const
    {
        Point c;
        for (int i = 0; i < KD; i++)
            c.x[i] = (lower[i] + upper[i]) / 2;
        return c;
    }

    double size2() const
    {
        double s = 0;
        for (int i = 0; i < KD; i++)
            s += (upper[i] - lower[i]) * (upper[i] - lower[i]);
        return s;
    }

    bool is_leaf() const
    {
        return l >= r || sqrt(size2()) < Config::kd_tree_eta;
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
    std::vector<Point> m_points;

    void m_build(Node* p, int k);
    void m_destory(Node* p);
};

#endif // KD_TREE_H
