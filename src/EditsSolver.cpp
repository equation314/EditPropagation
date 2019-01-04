#include <algorithm>
#include <iostream>
#include <Eigen/Core>

#include "EditsSolver.h"

using namespace std;
using namespace Eigen;

VectorXd lower_rank_approximation(const MatrixXd& U, const VectorXd& W, const VectorXd& g)
{
    return W;
}

DoubleArray EditsSolver::solve(const DoubleArray& w, const DoubleArray& g, const FeatureVecArray& fv)
{
    int n = w.size(), m = COL_SAMPLES;
    double lambda = 0;
    for (auto i : w)
        lambda += i;
    lambda /= n;

    vector<int> index(n);
    for (int i = 0; i < n; i++)
        index[i] = i;
    random_shuffle(index.begin(), index.end());

    VectorXd vW(n);
    VectorXd vg(n);
    MatrixXd U(n, m);

    for (int i = 0; i < n; i++)
    {
        vW(i) = w[index[i]];
        vg(i) = g[index[i]];
    }
    VectorXd ve = (U, vW, vg);

    DoubleArray e(n);
    for (int i = 0; i < n; i++)
        e[index[i]] = ve(i) / lambda / 2;
    return e;
}
