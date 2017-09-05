// QCGrids is a numerical integration library for quantum chemistry.
// Copyright (C) 2011-2017 The QCGrids developers
//
// This file is part of QCGrids.
//
// QCGrids is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// QCGrids is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>
// --



#include "qcgrids/scalarfns.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

#include <cmath>


namespace qcgrids {


double ScalarFunction::value(const double x) const {
  double result;
  calc(x, 0, &result);
  return result;
}


double ScalarFunction::value_inv(const double y) const {
  double result;
  calc_inv(y, 0, &result);
  return result;
}


double ScalarFunction::deriv(const double x) const {
  double result[2];
  calc(x, 1, result);
  return result[1];
}


double ScalarFunction::deriv_inv(const double y) const {
  double result[2];
  calc_inv(y, 1, result);
  return result[1];
}


double ScalarFunction::deriv2(const double x) const {
  double result[3];
  calc(x, 2, result);
  return result[2];
}


double ScalarFunction::deriv2_inv(const double y) const {
  double result[3];
  calc_inv(y, 2, result);
  return result[2];
}


void Exponential::calc(const double x, const int nderiv, double* const output) const {
  if (nderiv < 0) throw std::domain_error("nderiv cannot be negative.");
  output[0] = exp(exponent*x);
  if (nderiv > 0) output[1] = output[0]*exponent;
  if (nderiv > 1) output[2] = output[1]*exponent;
  if (nderiv > 2) throw std::domain_error("nderiv cannot be larger than two.");
}


void Exponential::calc_inv(const double y, const int nderiv, double* const output) const {
  if (nderiv < 0) throw std::domain_error("nderiv cannot be negative.");
  output[0] = log(y)/exponent;
  if (nderiv > 0) output[1] = 1.0/(y*exponent);
  if (nderiv > 1) output[2] = -output[1]/y;
  if (nderiv > 2) throw std::domain_error("nderiv cannot be larger than two.");
}


double Exponential::value(const double x) const {
  return exp(exponent*x);
}


double Exponential::value_inv(const double y) const {
  return log(y)/exponent;
}


double Exponential::deriv(const double x) const {
  return exponent*exp(exponent*x);
}


double Exponential::deriv_inv(const double y) const {
  return 1.0/(y*exponent);
}


double Exponential::deriv2(const double x) const {
  return exponent*exponent*exp(exponent*x);
}


double Exponential::deriv2_inv(const double y) const {
  return -1.0/(y*y*exponent);
}


void Spline::calc_inv(const double y, const int nderiv, double* const output) const {
  throw std::logic_error("calc_inv is not implemented for splines.");
}


void tridiagsym_solve(double* diag1, double* diag2, double* rhs, double* solution, size_t n) {
  double tmp1, tmp2;

  tmp1 = diag2[0];
  diag2[0] /= diag1[0];
  rhs[0] /= diag1[0];
  for (size_t i = 1; i < n; i++) {
    double denominator = (diag1[i] - diag2[i - 1]*tmp1);
    if (i < n-1) {
      tmp2 = diag2[i];
      diag2[i] /= denominator;
    }
    rhs[i] = (rhs[i] - rhs[i - 1]*tmp1)/denominator;
    tmp1 = tmp2;
  }

  solution[n - 1] = rhs[n - 1];
  for (int i = static_cast<int>(n) - 2; i >= 0; i--) {
    solution[i] = rhs[i] - diag2[i]*solution[i + 1];
  }
}


UniformCubicSpline::UniformCubicSpline(size_t npoint, const double* const _values,
    const double* const _derivs) : UniformCubicSpline(npoint) {
  std::copy(_values, _values + npoint, values);
  std::copy(_derivs, _derivs + npoint, derivs);
}


UniformCubicSpline::UniformCubicSpline(size_t npoint, const double* const _values)
    : UniformCubicSpline(npoint) {
  std::copy(_values, _values + npoint, values);
  fit_derivs();
}


UniformCubicSpline::UniformCubicSpline(const UniformCubicSpline &obj)
    : UniformCubicSpline(obj.npoint) {
  std::copy(obj.values, obj.values + npoint, values);
  std::copy(obj.derivs, obj.derivs + npoint, derivs);
}


UniformCubicSpline::~UniformCubicSpline() {
  delete[] values;
  delete[] derivs;
}


void UniformCubicSpline::calc(const double x, const int nderiv, double* const output) const {
  if (nderiv < 0) throw std::domain_error("nderiv cannot be negative.");
  if ((x < 0) || (x > npoint - 1)) {
    output[0] = 0.0;
    if (nderiv > 0) output[1] = 0.0;
    if (nderiv > 1) output[2] = 0.0;
  } else {
    size_t j = static_cast<size_t>(floor(x));
    double u = x - static_cast<double>(j);
    double z = values[j+1] - values[j];
    double c0 = values[j];
    double c1 = derivs[j];
    double c2 = 3.0*z - 2.0*derivs[j] - derivs[j+1];
    double c3 = -2.0*z + derivs[j] + derivs[j+1];
    output[0] = c0 + u*(c1 + u*(c2 + u*c3));
    if (nderiv > 0) output[1] = c1 + u*(2.0*c2 + u*3.0*c3);
    if (nderiv > 1) output[2] = 2.0*c2 + u*6.0*c3;
  }
  if (nderiv > 2) throw std::domain_error("nderiv cannot be larger than two.");
}


void UniformCubicSpline::fit_derivs() {
  std::unique_ptr<double[]> diag1(new double[npoint]);
  std::unique_ptr<double[]> diag2(new double[npoint - 1]);
  std::unique_ptr<double[]> rhs(new double[npoint]);

  // Setup the tri-diagonal system
  diag1[0] = 2.0;
  diag2[0] = 1.0;
  rhs[0] = 3.0*(values[1] - values[0]);
  for (size_t i = 1; i < npoint - 1; i++) {
    diag1[i] = 4.0;
    diag2[i] = 1.0;
    rhs[i] = 3.0*(values[i + 1] - values[i - 1]);
  }
  diag1[npoint-1] = 2.0;
  rhs[npoint-1] = 3.0*(values[npoint - 1] - values[npoint - 2]);

  tridiagsym_solve(diag1.get(), diag2.get(), rhs.get(), derivs, npoint);
}


}  // namespace qcgrids

// vim: textwidth=90 et ts=2 sw=2
