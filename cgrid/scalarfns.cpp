// CGrid is a library for for molecular numerical integration.
// Copyright (C) 2011-2019 The CGrid Development Team
//
// This file is part of CGrid.
//
// CGrid is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// CGrid is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>
// --



#include "cgrid/scalarfns.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

#include <cmath>


namespace cgrid {


void ScalarFunction::calc_inv(const double, const int, double* const) const {
  throw std::logic_error("calc_inv is not implemented for splines.");
}


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


Exp::Exp(double slope, double offset) : ScalarFunction(true), slope_(slope), offset_(offset) {
  // Some checks to assure invertibility.
  if (slope == 0)
    throw std::domain_error("slope cannot be zero in Exp function.");
}


void Exp::calc(const double x, const int nderiv, double* const output) const {
  if (nderiv < 0)
    throw std::domain_error("nderiv cannot be negative.");
  output[0] = exp(slope_*x + offset_);
  if (nderiv > 0)
    output[1] = output[0]*slope_;
  if (nderiv > 1)
    output[2] = output[1]*slope_;
  if (nderiv > 2)
    throw std::domain_error("nderiv cannot be larger than two.");
}


void Exp::calc_inv(const double y, const int nderiv, double* const output) const {
  if (nderiv < 0)
    throw std::domain_error("nderiv cannot be negative.");
  output[0] = (log(y) - offset_)/slope_;
  if (nderiv > 0)
    output[1] = 1.0/(y*slope_);
  if (nderiv > 1)
    output[2] = -output[1]/y;
  if (nderiv > 2)
    throw std::domain_error("nderiv cannot be larger than two.");
}


Ln::Ln(double prefac, double alpha) : ScalarFunction(true), prefac_(prefac), alpha_(alpha) {
  // Some checks to assure invertibility.
  if (prefac == 0)
    throw std::domain_error("prefac cannot be zero in Ln function.");
  if (alpha == 0)
    throw std::domain_error("alpha cannot be zero in Ln function.");
}


void Ln::calc(const double x, const int nderiv, double* const output) const {
  if (nderiv < 0)
    throw std::domain_error("nderiv cannot be negative.");
  output[0] = prefac_*log(alpha_*x);
  if (nderiv > 0)
    output[1] = prefac_/x;
  if (nderiv > 1)
    output[2] = -output[1]/x;
  if (nderiv > 2)
    throw std::domain_error("nderiv cannot be larger than two.");
}


void Ln::calc_inv(const double y, const int nderiv, double* const output) const {
  if (nderiv < 0)
    throw std::domain_error("nderiv cannot be negative.");
  output[0] = exp(y/prefac_)/alpha_;
  if (nderiv > 0)
    output[1] = output[0]/prefac_;
  if (nderiv > 1)
    output[2] = output[1]/prefac_;
  if (nderiv > 2)
    throw std::domain_error("nderiv cannot be larger than two.");
}


Linear::Linear(double slope, double offset) : ScalarFunction(true), slope_(slope),
      offset_(offset) {
  // Some checks to assure invertibility.
  if (slope == 0)
    throw std::domain_error("slope cannot be zero in Linear function.");
}


void Linear::calc(const double x, const int nderiv, double* const output) const {
  if (nderiv < 0)
    throw std::domain_error("nderiv cannot be negative.");
  output[0] = slope_*x + offset_;
  if (nderiv > 0)
    output[1] = slope_;
  if (nderiv > 1)
    output[2] = 0.0;
  if (nderiv > 2)
    throw std::domain_error("nderiv cannot be larger than two.");
}


void Linear::calc_inv(const double y, const int nderiv, double* const output) const {
  if (nderiv < 0)
    throw std::domain_error("nderiv cannot be negative.");
  output[0] = (y - offset_)/slope_;
  if (nderiv > 0)
    output[1] = 1.0/slope_;
  if (nderiv > 1)
    output[2] = 0.0;
  if (nderiv > 2)
    throw std::domain_error("nderiv cannot be larger than two.");
}


void Identity::calc(const double x, const int nderiv, double* const output) const {
  if (nderiv < 0)
    throw std::domain_error("nderiv cannot be negative.");
  output[0] = x;
  if (nderiv > 0)
    output[1] = 1.0;
  if (nderiv > 1)
    output[2] = 0.0;
  if (nderiv > 2)
    throw std::domain_error("nderiv cannot be larger than two.");
}


void Identity::calc_inv(const double y, const int nderiv, double* const output) const {
  if (nderiv < 0)
    throw std::domain_error("nderiv cannot be negative.");
  output[0] = y;
  if (nderiv > 0)
    output[1] = 1.0;
  if (nderiv > 1)
    output[2] = 0.0;
  if (nderiv > 2)
    throw std::domain_error("nderiv cannot be larger than two.");
}


void Constant::calc(const double, const int nderiv, double* const output) const {
  if (nderiv < 0)
    throw std::domain_error("nderiv cannot be negative.");
  output[0] = offset_;
  if (nderiv > 0)
    output[1] = 0.0;
  if (nderiv > 1)
    output[2] = 0.0;
  if (nderiv > 2)
    throw std::domain_error("nderiv cannot be larger than two.");
}


Power::Power(double prefac, double power) : ScalarFunction(true), prefac_(prefac),
    power_(power) {
  // Some checks to assure invertibility.
  if (prefac == 0)
    throw std::domain_error("prefac cannot be zero in Power function.");
  if (power == 0)
    throw std::domain_error("The power cannot be zero in the Power function.");
}


void Power::calc(const double x, const int nderiv, double* const output) const {
  if (nderiv < 0)
    throw std::domain_error("nderiv cannot be negative.");
  output[0] = prefac_*pow(x, power_);
  if (nderiv > 0)
    output[1] = prefac_*power_*pow(x, power_ - 1);
  if (nderiv > 1) {
    // Avoid risk for division by zero;
    if (power_ == 1) {
      output[2] = 0.0;
    } else {
      output[2] = prefac_*power_*(power_-1)*pow(x, power_ - 2);
    }
  }
  if (nderiv > 2)
    throw std::domain_error("nderiv cannot be larger than two.");
}


void Power::calc_inv(const double y, const int nderiv, double* const output) const {
  if (nderiv < 0)
    throw std::domain_error("nderiv cannot be negative.");
  output[0] = pow(y/prefac_, 1.0/power_);
  if (nderiv > 0)
    output[1] = pow(y/prefac_, 1.0/power_ - 1.0)/(power_*prefac_);
  if (nderiv > 1) {
    // Avoid risk for division by zero;
    if (power_ == 1) {
      output[2] = 0.0;
    } else {
      output[2] = pow(y/prefac_, 1.0/power_ - 2.0)/(power_*prefac_*prefac_)*(1.0/power_ - 1.0);
    }
  }
  if (nderiv > 2)
    throw std::domain_error("nderiv cannot be larger than two.");
}


double Power::deriv2(const double x) const {
  if (power_ == 1) {
    return 0.0;
  } else {
    return prefac_*power_*(power_-1)*pow(x, power_ - 2);
  }
}


double Power::deriv2_inv(const double y) const {
  if (power_ == 1) {
    return 0.0;
  } else {
    return pow(y/prefac_, 1.0/power_ - 2.0)/(power_*prefac_*prefac_)*(1.0/power_ - 1.0);
  }
}


Rational::Rational(double prefac, double root) : ScalarFunction(true), prefac_(prefac),
    root_(root) {
  // Some checks to assure invertibility and to avoid division be zero.
  if (prefac == 0)
    throw std::domain_error("prefac cannot be zero in Rational function.");
  if (root == 0)
    throw std::domain_error("The root cannot be zero in Rational function.");
}


void Rational::calc(const double x, const int nderiv, double* const output) const {
  if (nderiv < 0)
    throw std::domain_error("nderiv cannot be negative.");
  double t = 1 - x/root_;
  output[0] = prefac_*x/t;
  if (nderiv > 0)
    output[1] = prefac_/(t*t);
  if (nderiv > 1)
    output[2] = 2*output[1]/(root_*t);
  if (nderiv > 2)
    throw std::domain_error("nderiv cannot be larger than two.");
}


void Rational::calc_inv(const double y, const int nderiv, double* const output) const {
  if (nderiv < 0)
    throw std::domain_error("nderiv cannot be negative.");
  double t = prefac_ + y/root_;
  output[0] = y/t;
  if (nderiv > 0)
    output[1] = prefac_/(t*t);
  if (nderiv > 1)
    output[2] = -2*output[1]/(root_*t);
  if (nderiv > 2)
    throw std::domain_error("nderiv cannot be larger than two.");
}


Spline::Spline(size_t npoint) : ScalarFunction(false), npoint_(npoint) {
  if (npoint < 2)
    throw std::domain_error("A spline should have at least two points.");
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


UniformCubicSpline::UniformCubicSpline(size_t npoint)
    : Spline(npoint), values_(new double[npoint]), derivs_(new double[npoint]) {
  std::fill(values_, values_ + npoint_, 0.0);
  std::fill(derivs_, derivs_ + npoint_, 0.0);
}


UniformCubicSpline::UniformCubicSpline(size_t npoint, const double* const values,
    const double* const derivs) : UniformCubicSpline(npoint) {
  std::copy(values, values + npoint_, values_);
  std::copy(derivs, derivs + npoint_, derivs_);
}


UniformCubicSpline::UniformCubicSpline(size_t npoint, const double* const values)
    : UniformCubicSpline(npoint) {
  std::copy(values, values + npoint_, values_);
  fit_derivs();
}


UniformCubicSpline::UniformCubicSpline(const UniformCubicSpline &obj)
    : UniformCubicSpline(obj.npoint()) {
  std::copy(obj.values(), obj.values() + npoint_, values_);
  std::copy(obj.derivs(), obj.derivs() + npoint_, derivs_);
}


UniformCubicSpline::~UniformCubicSpline() {
  delete[] values_;
  delete[] derivs_;
}


void UniformCubicSpline::calc(const double x, const int nderiv, double* const output) const {
  if (nderiv < 0)
    throw std::domain_error("nderiv cannot be negative.");
  if ((x < 0) || (x > static_cast<double>(npoint_ - 1))) {
    output[0] = 0.0;
    if (nderiv > 0)
      output[1] = 0.0;
    if (nderiv > 1)
      output[2] = 0.0;
  } else {
    size_t j = static_cast<size_t>(floor(x));
    // When x coincides with the last point, we need to avoid accessing data past the last
    // element of the values and derivs arrays.
    if (j == npoint_ - 1)
      j--;
    double u = x - static_cast<double>(j);
    double z = values_[j+1] - values_[j];
    double c0 = values_[j];
    double c1 = derivs_[j];
    double c2 = 3.0*z - 2.0*derivs_[j] - derivs_[j+1];
    double c3 = -2.0*z + derivs_[j] + derivs_[j+1];
    output[0] = c0 + u*(c1 + u*(c2 + u*c3));
    if (nderiv > 0)
      output[1] = c1 + u*(2.0*c2 + u*3.0*c3);
    if (nderiv > 1)
      output[2] = 2.0*c2 + u*6.0*c3;
  }
  if (nderiv > 2)
    throw std::domain_error("nderiv cannot be larger than two.");
}


void UniformCubicSpline::fit_derivs() {
  std::unique_ptr<double[]> diag1(new double[npoint_]);
  std::unique_ptr<double[]> diag2(new double[npoint_ - 1]);
  std::unique_ptr<double[]> rhs(new double[npoint_]);

  // Setup the tri-diagonal system
  diag1[0] = 2.0;
  diag2[0] = 1.0;
  rhs[0] = 3.0*(values_[1] - values_[0]);
  for (size_t i = 1; i < npoint_ - 1; i++) {
    diag1[i] = 4.0;
    diag2[i] = 1.0;
    rhs[i] = 3.0*(values_[i + 1] - values_[i - 1]);
  }
  diag1[npoint_ - 1] = 2.0;
  rhs[npoint_ - 1] = 3.0*(values_[npoint_ - 1] - values_[npoint_ - 2]);

  tridiagsym_solve(diag1.get(), diag2.get(), rhs.get(), derivs_, npoint_);
}


Composed::Composed(Spline* spline, ScalarFunction* const x_transform,
                   ScalarFunction* const y_transform, ScalarFunction* left_extra,
                   ScalarFunction* right_extra)
    : ScalarFunction(false),
      default_transform_(new Identity()), default_extra_(new Constant(0.0)),
      spline_(spline),
      x_transform_((x_transform == NULL) ? default_transform_ : x_transform),
      y_transform_((y_transform == NULL) ? default_transform_ : y_transform),
      left_extra_((left_extra == NULL) ? default_extra_ : left_extra),
      right_extra_((right_extra == NULL) ? default_extra_ : right_extra) {
  if (!x_transform_->invertible())
    throw std::logic_error("x_transform must be invertible.");
  if (!y_transform_->invertible())
    throw std::logic_error("y_transform must be invertible.");
}


Composed::Composed(Spline* spline, ScalarFunction* x_transform,
                   ScalarFunction* y_transform, ScalarFunction* left_extra,
                   ScalarFunction* right_extra, const double* values)
    : Composed(spline, x_transform, y_transform, left_extra, right_extra) {
  for (size_t i = 0; i < spline->npoint(); i++)
    spline_->values()[i] = y_transform_->value_inv(values[i]);
  spline->fit_derivs();
}


Composed::Composed(Spline* spline, ScalarFunction* x_transform,
                   ScalarFunction* y_transform, ScalarFunction* left_extra,
                   ScalarFunction* right_extra, const double* values,
                   const double* derivs)
    : Composed(spline, x_transform, y_transform, left_extra, right_extra) {
  for (size_t i = 0; i < spline->npoint(); i++) {
    double y_output[2];
    y_transform->calc_inv(values[i], 1, y_output);
    spline_->values()[i] = y_output[0];
    spline_->derivs()[i] = y_output[1] * derivs[i] * x_transform_->deriv(static_cast<double>(i));
  }
}


Composed::~Composed() {
  delete default_transform_;
  delete default_extra_;
}


void Composed::calc(const double x, const int nderiv, double* const output) const {
  if (x < x_transform_->value(spline_->left())) {
    left_extra_->calc(x, nderiv, output);
  } else if (x > x_transform_->value(spline_->right())) {
    right_extra_->calc(x, nderiv, output);
  } else {
    if (nderiv < 0)
      throw std::domain_error("nderiv cannot be negative.");
    if (nderiv > 2)
      throw std::domain_error("nderiv cannot be larger than two.");
    double tx[3];
    double s[3];
    double ty[3];
    x_transform_->calc_inv(x, nderiv, tx);
    spline_->calc(tx[0], nderiv, s);
    y_transform_->calc(s[0], nderiv, ty);
    output[0] = ty[0];
    if (nderiv > 0)
      output[1] = ty[1]*s[1]*tx[1];
    if (nderiv > 1)
      output[2] = ty[1]*s[1]*tx[2] + ty[1]*s[2]*tx[1]*tx[1] + ty[2]*s[1]*s[1]*tx[1]*tx[1];
  }
}


}  // namespace cgrid

// vim: textwidth=90 et ts=2 sw=2
