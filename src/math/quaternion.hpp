//
// Copyright (c) 2016 CNRS
//
// This file is part of Pinocchio
// Pinocchio is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
//
// Pinocchio is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Lesser Public License for more details. You should have
// received a copy of the GNU Lesser General Public License along with
// Pinocchio If not, see
// <http://www.gnu.org/licenses/>.

#ifndef __math_quaternion_hpp__
#define __math_quaternion_hpp__

#include <boost/math/constants/constants.hpp>

namespace se3
{
  ///
  /// \brief Compute the minimal angle between q1 and q2.
  ///
  /// \param[in] q1 input quaternion.
  /// \param[in] q2 input quaternion.
  ///
  /// \return angle between the two quaternions
  ///
  template <typename D>
  typename D::Scalar angleBetweenQuaternions(const Eigen::QuaternionBase<D> & q1,
                                             const Eigen::QuaternionBase<D> & q2)
  {
    typedef typename D::Scalar Scalar;
    const Scalar innerprod = q1.dot(q2);
    Scalar theta = acos(innerprod);
    const Scalar PIs = boost::math::constants::pi<Scalar>();
    
    if (innerprod < 0)
      return PIs - theta;
    
    return theta;
  }
  
  ///
  /// \brief Check if two quaternions define the same rotations.
  /// \note Two quaternions define the same rotation iff q1 == q2 OR q1 == -q2.
  ///
  /// \param[in] q1 input quaternion.
  /// \param[in] q2 input quaternion.
  ///
  /// \return Return true if the two input quaternions define the same rotation.
  ///
  template <typename Derived, typename otherDerived>
  bool defineSameRotation(const Eigen::QuaternionBase<Derived> & q1,
                          const Eigen::QuaternionBase<otherDerived> & q2)
  {
    return (q1.coeffs().isApprox(q2.coeffs()) || q1.coeffs().isApprox(-q2.coeffs()) );
  }

}
#endif //#ifndef __math_quaternion_hpp__
