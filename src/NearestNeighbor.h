#ifndef NEAREST_NEIGHBOR_H
#define NEAREST_NEIGHBOR_H

#include <nanoflann.hpp>

#include "Common.h"

class NearestNeighbor
{
public:
    NearestNeighbor();
    virtual ~NearestNeighbor();

    void insert(const Point* p) { m_pts.push_back(p); }

    void build();

    double nearestDist2(const Point* query);

    // Must return the number of data points
    inline size_t kdtree_get_point_count() const { return m_pts.size(); }

    // Returns the dim'th component of the idx'th point in the class:
    // Since this is inlined and the "dim" argument is typically an immediate value, the
    //  "if/else's" are actually solved at compile time.
    inline double kdtree_get_pt(const size_t idx, const size_t dim) const
    {
        return m_pts[idx]->x[dim];
    }

    // Optional bounding-box computation: return false to default to a standard bbox computation loop.
    //   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
    //   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
    template <class BBOX>
    bool kdtree_get_bbox(BBOX& /* bb */) const
    {
        return false;
    }

private:
    typedef nanoflann::KDTreeSingleIndexAdaptor<
        nanoflann::L2_Simple_Adaptor<double, NearestNeighbor>,
        NearestNeighbor,
        DIM>
        my_kd_tree_t;

    my_kd_tree_t* m_index;
    std::vector<const Point*> m_pts;
};

#endif // NEAREST_NEIGHBOR_H
