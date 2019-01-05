#ifndef EDITS_SOLVER_H
#define EDITS_SOLVER_H

#include "EditPropagation.h"

class EditsSolver
{
private:
    EditsSolver();
    ~EditsSolver();

public:
    static constexpr int COL_SAMPLES = 100;

    static DoubleArray solve(const DoubleArray& w, const DoubleArray& g, const FeatureVecArray& fv);
};

#endif // EDITS_SOLVER_H