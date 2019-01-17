#include <algorithm>
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Dense>

#include "Config.h"
#include "EditsSolver.h"

using namespace std;
using namespace Eigen;

VectorXd lower_rank_approximation(const MatrixXd& U, const VectorXd& w, const VectorXd& g, const VectorXd& mu, double lambda)
{
    int n = U.rows(), m = U.cols();
    MatrixXd U_T = U.transpose();
    MatrixXd A = U.block(0, 0, m, m);
    MatrixXd A_1 = A.inverse();

    VectorXd a = U * (A_1 * (U_T * (mu.asDiagonal() * w))); // U A^-1 U^T M W 1_n
    VectorXd b = U * (A_1 * (U_T * mu));                    // U A^-1 U^T M 1_n
    VectorXd d_1 = a / lambda / 2 + b;
    for (int i = 0; i < n; i++)
        d_1(i) = mu(i) / d_1(i);

    VectorXd c = U * (A_1 * (U_T * (mu.asDiagonal() * (w.asDiagonal() * g)))); // U A^-1 U^T M W g
    MatrixXd U_TD_1 = U_T * d_1.asDiagonal();                                  // U^T D^-1
    MatrixXd D_1U = d_1.asDiagonal() * U;                                      // D^-1 U
    MatrixXd mid = (U_TD_1 * U - A).inverse();                                 // (-A + U^T D^-1 U)^-1

    VectorXd e = d_1.asDiagonal() * c - D_1U * (mid * (U_TD_1 * c));
    for (int i = 0; i < n; i++)
        e(i) /= mu(i);
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
    DoubleArray mu(w.size(), 1);
    return solve(w, g, mu, fv);
}

DoubleArray EditsSolver::solve(const DoubleArray& w, const DoubleArray& g, const DoubleArray& mu, const FeatureVecArray& fv)
{
    int n = w.size(), m = Config::column_samples;
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
    VectorXd vmu(n);
    MatrixXd U(n, m);

    for (int i = 0; i < n; i++)
    {
        vw(i) = w[index[i]];
        vg(i) = g[index[i]];
        vmu(i) = mu[index[i]];
        for (int j = 0; j < m; j++)
            U(i, j) = fv[index[i]].affinity_with(fv[index[j]]);
    }

    VectorXd ve;
    if (Config::use_brute_force)
    {
        MatrixXd Z(n, n);
        for (int i = 0, t = 0; i < n; i++)
            for (int j = 0; j < n; j++, t++)
                Z(xedni[i], xedni[j]) = mu[i] * mu[j] * fv[i].affinity_with(fv[j]);
        ve = brute_force(Z, vw, vg, lambda);
    }
    else
        ve = lower_rank_approximation(U, vw, vg, vmu, lambda);

    DoubleArray e(n);
    for (int i = 0; i < n; i++)
        e[i] = max(min(ve(xedni[i]), 1.0), 0.0);
    return e;
}
