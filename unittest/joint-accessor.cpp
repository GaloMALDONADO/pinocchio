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

#include "pinocchio/multibody/model.hpp"
#include "pinocchio/algorithm/jacobian.hpp"
#include "pinocchio/algorithm/operational-frames.hpp"
#include "pinocchio/algorithm/rnea.hpp"
#include "pinocchio/algorithm/joint-configuration.hpp"
#include "pinocchio/algorithm/compute-all-terms.hpp"
#include "pinocchio/spatial/act-on-set.hpp"
#include "pinocchio/multibody/parser/sample-models.hpp"
#include "pinocchio/tools/timer.hpp"

#include "pinocchio/multibody/joint/joint-accessor.hpp"

#include <iostream>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE JointAccessorTest
#include <boost/test/unit_test.hpp>
#include <boost/utility/binary.hpp>

template <typename T>
void test_joint_methods (T & jmodel, typename T::JointData & jdata)
{
    Eigen::VectorXd q1(Eigen::VectorXd::Random (jmodel.nq()));
    Eigen::VectorXd q1_dot(Eigen::VectorXd::Random (jmodel.nv()));
    Eigen::VectorXd q2(Eigen::VectorXd::Random (jmodel.nq()));
    double u = 0.3;
    se3::Inertia::Matrix6 Ia(se3::Inertia::Random().matrix());
    bool update_I = false;

    jmodel.calc(jdata, q1, q1_dot); // To be removed when cherry-picked the fix of calc visitor
    jmodel.calc_aba(jdata, Ia, update_I); // To be removed when cherry-picked the fix of calc_aba visitor

    se3::JointModelVariant jmodelvariant(jmodel);
    se3::JointDataVariant jdatavariant(jdata);

    se3::JointModelAccessor jma(jmodelvariant);
    se3::JointDataAccessor jda(jdatavariant);

    // calc_first_order(jmodelvariant, jdatavariant, q1, q1_dot); // To be added instead of line 134
    // jma.calc(jda, q1, q1_dot);                                 // or test with this one
    // jma.calc_aba(jda, Ia, update_I) 

    std::string error_prefix("Joint Model Accessor on " + T::shortname());
    BOOST_CHECK_MESSAGE(nq(jmodelvariant) == jma.nq() ,std::string(error_prefix + " - nq "));
    BOOST_CHECK_MESSAGE(nv(jmodelvariant) == jma.nv() ,std::string(error_prefix + " - nv "));

    BOOST_CHECK_MESSAGE(idx_q(jmodelvariant) == jma.idx_q() ,std::string(error_prefix + " - Idx_q "));
    BOOST_CHECK_MESSAGE(idx_v(jmodelvariant) == jma.idx_v() ,std::string(error_prefix + " - Idx_v "));
    BOOST_CHECK_MESSAGE(id(jmodelvariant) == jma.id() ,std::string(error_prefix + " - JointId "));

    BOOST_CHECK_MESSAGE(integrate(jmodelvariant,q1,q1_dot).isApprox(jma.integrate(q1,q1_dot)) ,std::string(error_prefix + " - integrate "));
    BOOST_CHECK_MESSAGE(interpolate(jmodelvariant,q1,q2,u).isApprox(jma.interpolate(q1,q2,u)) ,std::string(error_prefix + " - interpolate "));
    BOOST_CHECK_MESSAGE(randomConfiguration(jmodelvariant, -1 * Eigen::VectorXd::Ones(nq(jmodelvariant)),
                                                Eigen::VectorXd::Ones(nq(jmodelvariant))).size()
                                    == jma.randomConfiguration(-1 * Eigen::VectorXd::Ones(jma.nq()),
                                                              Eigen::VectorXd::Ones(jma.nq())).size()
                        ,std::string(error_prefix + " - RandomConfiguration dimensions "));
    BOOST_CHECK_MESSAGE(difference(jmodelvariant,q1,q2).isApprox(jma.difference(q1,q2)) ,std::string(error_prefix + " - difference "));
    BOOST_CHECK_MESSAGE(distance(jmodelvariant,q1,q2) == jma.distance(q1,q2) ,std::string(error_prefix + " - distance "));


    BOOST_CHECK_MESSAGE((jda.S().matrix()).isApprox((constraint_xd(jdatavariant).matrix())),std::string(error_prefix + " - ConstraintXd "));
    BOOST_CHECK_MESSAGE((jda.M()) == (joint_transform(jdatavariant)),std::string(error_prefix + " - Joint transforms ")); // ==  or isApprox ?
    BOOST_CHECK_MESSAGE((jda.v()) == (motion(jdatavariant)),std::string(error_prefix + " - Joint motions "));
    BOOST_CHECK_MESSAGE((jda.c()) == (bias(jdatavariant)),std::string(error_prefix + " - Joint bias "));
    
    BOOST_CHECK_MESSAGE((jda.U()).isApprox((u_inertia(jdatavariant))),std::string(error_prefix + " - Joint U inertia matrix decomposition "));
    BOOST_CHECK_MESSAGE((jda.Dinv()).isApprox((dinv_inertia(jdatavariant))),std::string(error_prefix + " - Joint DInv inertia matrix decomposition "));
    BOOST_CHECK_MESSAGE((jda.UDinv()).isApprox((udinv_inertia(jdatavariant))),std::string(error_prefix + " - Joint UDInv inertia matrix decomposition "));
}

struct TestJointAccessor{

  template <typename T>
  void operator()(const T t) const
  {
    T jmodel;
    jmodel.setIndexes(0,0,0);
    typename T::JointData jdata = jmodel.createData();

    test_joint_methods(jmodel, jdata);    
  }

  template <int NQ, int NV>
  void operator()(const se3::JointModelDense<NQ,NV> & ) const
  {
    // Not yet correctly implemented, test has no meaning for the moment
  }

  void operator()(const se3::JointModelRevoluteUnaligned & ) const
  {
    se3::JointModelRevoluteUnaligned jmodel(1.5, 1., 0.);
    jmodel.setIndexes(0,0,0);
    se3::JointModelRevoluteUnaligned::JointData jdata = jmodel.createData();

    test_joint_methods(jmodel, jdata);
  }

  void operator()(const se3::JointModelPrismaticUnaligned & ) const
  {
    se3::JointModelPrismaticUnaligned jmodel(1.5, 1., 0.);
    jmodel.setIndexes(0,0,0);
    se3::JointModelPrismaticUnaligned::JointData jdata = jmodel.createData();

    test_joint_methods(jmodel, jdata);
  }

};


BOOST_AUTO_TEST_SUITE ( JointAccessorTest)

BOOST_AUTO_TEST_CASE ( test_all_joints )
{
  using namespace Eigen;
  using namespace se3;

  Model model;
  buildModels::humanoidSimple(model);
  se3::Data data(model);


  boost::mpl::for_each<JointModelVariant::types>(TestJointAccessor());


}
BOOST_AUTO_TEST_SUITE_END ()

