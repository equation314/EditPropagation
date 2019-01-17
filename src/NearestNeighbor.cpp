#include "NearestNeighbor.h"

using namespace nanoflann;

NearestNeighbor::NearestNeighbor()
    : m_index(nullptr)
{
}

NearestNeighbor::~NearestNeighbor()
{
    if (m_index)
        delete m_index;
}

void NearestNeighbor::build()
{
    m_index = new my_kd_tree_t(DIM, *this, KDTreeSingleIndexAdaptorParams(10 /* max leaf */));
    m_index->buildIndex();
}

double NearestNeighbor::nearestDist2(const Point& query)
{
    size_t ret_index;
    double out_dist_sqr;
    KNNResultSet<double> resultSet(1);
    resultSet.init(&ret_index, &out_dist_sqr);
    m_index->findNeighbors(resultSet, query.x, nanoflann::SearchParams(10));
    return out_dist_sqr;
}
