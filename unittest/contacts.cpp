//
// Copyright (c) 2017 CNRS
//
// This file is part of PinInvDyn
// PinInvDyn is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
// PinInvDyn is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Lesser Public License for more details. You should have
// received a copy of the GNU Lesser General Public License along with
// PinInvDyn If not, see
// <http://www.gnu.org/licenses/>.
//

#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/utility/binary.hpp>

#include <pinocchio/algorithm/joint-configuration.hpp>

#include <pininvdyn/contacts/contact-6d.hpp>

#define HRP2_PKG_DIR "/home/adelpret/devel/sot_hydro/install/share"

using namespace pininvdyn;
using namespace pininvdyn::trajectories;
using namespace pininvdyn::math;
using namespace pininvdyn::contacts;
using namespace std;

BOOST_AUTO_TEST_SUITE ( BOOST_TEST_MODULE )

#define REQUIRE_FINITE(A) BOOST_REQUIRE_MESSAGE(is_finite(A), #A<<": "<<A)

BOOST_AUTO_TEST_CASE ( test_contact_6d )
{
  const double lx = 0.07;
  const double ly = 0.12;
  const double lz = 0.105;
  const double mu = 0.3;
  const double fMin = 10.0;
  const std::string frameName = "RLEG_JOINT5";

  vector<string> package_dirs;
  package_dirs.push_back(HRP2_PKG_DIR);
  string urdfFileName = package_dirs[0] + "/hrp2_14_description/urdf/hrp2_14_reduced.urdf";
  RobotWrapper robot(urdfFileName,
                     package_dirs,
                     se3::JointModelFreeFlyer(),
                     false);

  BOOST_REQUIRE(robot.model().existFrame(frameName));

  Vector3 contactNormal = Vector3::UnitZ();
  Matrix3x contactPoints(3,4);
  contactPoints << -lx, -lx, +lx, +lx,
                   -ly, +ly, -ly, +ly,
                    lz,  lz,  lz,  lz;
  Contact6d contact("contact6d", robot, frameName,
                    contactPoints, contactNormal,
                    mu, fMin, 1e-3);

  BOOST_CHECK(contact.n_motion()==6);
  BOOST_CHECK(contact.n_force()==12);

  Vector Kp = Vector::Ones(6);
  Vector Kd = 2*Vector::Ones(6);
  contact.Kp(Kp);
  contact.Kd(Kd);
  BOOST_CHECK(contact.Kp().isApprox(Kp));
  BOOST_CHECK(contact.Kd().isApprox(Kd));

  Vector q = robot.model().neutralConfiguration;
  Vector v = Vector::Zero(robot.nv());
  se3::Data data(robot.model());
  robot.computeAllTerms(data, q, v);

  se3::SE3 H_ref = robot.position(data, robot.model().getJointId(frameName));
  contact.setReference(H_ref);

  double t = 0.0;
  contact.computeMotionTask(t, q, v, data);

  const ConstraintInequality & forceIneq = contact.computeForceTask(t, q, v, data);
  Vector3 f3;
  Vector f(12);
  f3 << 0.0, 0.0, 100.0;
  f << f3, f3, f3, f3;
  BOOST_CHECK(forceIneq.checkConstraint(f));
  BOOST_CHECK(((forceIneq.matrix()*f).array() <= forceIneq.upperBound().array()).all());
  BOOST_CHECK(((forceIneq.matrix()*f).array() >= forceIneq.lowerBound().array()).all());
  f(0) = f(2)*mu*1.1;
  BOOST_CHECK(forceIneq.checkConstraint(f)==false);

  const Matrix & forceGenMat = contact.getForceGeneratorMatrix();
  BOOST_CHECK(forceGenMat.rows()==6 && forceGenMat.cols()==12);

  contact.computeForceRegularizationTask(t, q, v, data);
}

BOOST_AUTO_TEST_SUITE_END ()