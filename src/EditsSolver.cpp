#include <algorithm>
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Dense>

#include "EditsSolver.h"

using namespace std;
using namespace Eigen;

VectorXd lower_rank_approximation(const MatrixXd& U, const VectorXd& w, const VectorXd& g, double lambda)
{
    int n = U.rows(), m = U.cols();
    MatrixXd U_T = U.transpose();
    MatrixXd A = U.block(0, 0, m, m);
    MatrixXd A_1 = A.inverse();

    VectorXd one_n = VectorXd::Ones(n);
    VectorXd a = U * (A_1 * (U_T * w));     // U A^-1 U^T W 1_n
    VectorXd b = U * (A_1 * (U_T * one_n)); // U A^-1 U^T 1_n
    VectorXd d_1 = a / lambda / 2 + b;
    for (int i = 0; i < n; i++)
        d_1(i) = 1 / d_1(i);

    VectorXd c = U * (A_1 * (U_T * (w.asDiagonal() * g))); // U A^-1 U^T W g
    MatrixXd U_TD_1 = U_T * d_1.asDiagonal();              // U^T D^-1
    MatrixXd D_1U = d_1.asDiagonal() * U;                  // D^-1 U
    MatrixXd mid = (U_TD_1 * U - A).inverse();             // (-A + U^T D^-1 U)^-1

    VectorXd e = d_1.asDiagonal() * c - D_1U * (mid * (U_TD_1 * c));
    e /= lambda * 2;

    return e;
}

VectorXd brute_force(const MatrixXd& Z, const VectorXd& w, const VectorXd& g, double lambda)
{
    int n = Z.rows();

    VectorXd one_n = VectorXd::Ones(n);
    VectorXd a = Z * w;
    VectorXd b = Z * one_n;
    VectorXd d = a / lambda / 2 + b;

    VectorXd c = Z * (w.asDiagonal() * g);
    VectorXd e = (d.asDiagonal().toDenseMatrix() - Z).inverse() * c;
    e /= lambda * 2;

    return e;
}

DoubleArray EditsSolver::solve(const DoubleArray& w, const DoubleArray& g, const FeatureVecArray& fv)
{
    int n = w.size(), m = COL_SAMPLES;
    double lambda = 0;
    for (auto i : w)
        lambda += i;
    lambda /= n;

    vector<int> index(n), xedni(n);
    for (int i = 0; i < n; i++)
        index[i] = i;
    random_shuffle(index.begin(), index.end());
    for (int i = 0; i < n; i++)
        xedni[index[i]] = i;

    VectorXd vw(n);
    VectorXd vg(n);
    MatrixXd U(n, m);

    for (int i = 0; i < n; i++)
    {
        vw(i) = w[index[i]];
        vg(i) = g[index[i]];
        for (int j = 0; j < m; j++)
            U(i, j) = fv[index[i]].affinity_with(fv[index[j]]);
    }
    VectorXd ve = lower_rank_approximation(U, vw, vg, lambda);

    DoubleArray e(n);
    for (int i = 0; i < n; i++)
        e[i] = ve(xedni[i]);
    return e;
}
