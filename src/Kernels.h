#pragma once

#ifndef COVIDSIM_KERNELS_H_INCLUDED_
#    define COVIDSIM_KERNELS_H_INCLUDED_

#    include <cmath>
#    include <cstddef>
#    include <cstdio>

#    include "Param.h"

extern double *nKernel, *nKernelHR;

void
InitKernel(int, double);
double
numKernel(double);

inline double
ExpKernel(double);
inline double
PowerKernel(double);
inline double
PowerKernelB(double);
inline double
PowerKernelUS(double);
inline double
PowerExpKernel(double);
inline double
GaussianKernel(double);
inline double
StepKernel(double);

inline double
ExpKernel(double r2)
{
    return exp(-sqrt(r2) / P.KernelScale);
}

inline double
PowerKernel(double r2)
{
    double t;

    t = -P.KernelShape * log(sqrt(r2) / P.KernelScale + 1);

    return (t < -690) ? 0 : exp(t);
}

inline double
PowerKernelB(double r2)
{
    double t;

    t = 0.5 * P.KernelShape * log(r2 / (P.KernelScale * P.KernelScale));

    return (t > 690) ? 0 : (1 / (exp(t) + 1));
}

inline double
PowerKernelUS(double r2)
{
    double t;

    t = log(sqrt(r2) / P.KernelScale + 1);

    return (t < -690) ? 0
                      : (exp(-P.KernelShape * t) + P.KernelP3 * exp(-P.KernelP4 * t)) /
                            (1 + P.KernelP3);
}

inline double
GaussianKernel(double r2)
{
    return exp(-r2 / (P.KernelScale * P.KernelScale));
}

inline double
StepKernel(double r2)
{
    return (r2 > P.KernelScale * P.KernelScale) ? 0 : 1;
}

inline double
PowerExpKernel(double r2)
{
    double d, t;

    d = sqrt(r2);
    t = -P.KernelShape * log(d / P.KernelScale + 1);

    return (t < -690) ? 0 : exp(t - pow(d / P.KernelP3, P.KernelP4));
}

#endif  // COVIDSIM_KERNELS_H_INCLUDED_
