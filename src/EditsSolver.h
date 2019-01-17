#ifndef EDITS_SOLVER_H
#define EDITS_SOLVER_H

#include "Common.h"

class EditsSolver
{
private:
    EditsSolver();
    virtual ~EditsSolver();

public:
    static DoubleArray solve(const DoubleArray& w, const DoubleArray& g, const FeatureVecArray& fv);
    static DoubleArray solve(const DoubleArray& w, const DoubleArray& g, const DoubleArray& mu, const FeatureVecArray& fv);
};

#endif // EDITS_SOLVER_H
