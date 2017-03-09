//
// Copyright (c) 2017 CNRS
//
// This file is part of PinInvDyn
// pinocchio is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
// pinocchio is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Lesser Public License for more details. You should have
// received a copy of the GNU Lesser General Public License along with
// pinocchio If not, see
// <http://www.gnu.org/licenses/>.
//

#ifndef __invdyn_task_base_hpp__
#define __invdyn_task_base_hpp__

#include <pininvdyn/robot-wrapper.hpp>
#include <pininvdyn/math/utils.hpp>
#include <pininvdyn/math/constraint-base.hpp>

namespace pininvdyn
{
  namespace tasks
  {
    
    ///
    /// \brief Base template of a Task.
    /// Each class is defined according to a constant model of a robot.
    ///
    class TaskBase
    {
    public:
      typedef pininvdyn::RobotWrapper RobotWrapper;
      typedef pininvdyn::math::ConstraintBase ConstraintBase;
      typedef pininvdyn::math::ConstRefVector ConstRefVector;
      typedef se3::Data Data;

      TaskBase(const std::string & name,
               RobotWrapper & robot);

      const std::string & name() const;

      void name(const std::string & name);
      
      /// \brief Return the dimension of the task.
      /// \info should be overloaded in the child class.
      virtual int dim() const = 0;

      virtual const ConstraintBase & compute(const double t,
                                             ConstRefVector q,
                                             ConstRefVector v,
                                             Data & data) = 0;
      
    protected:
      
      /// \brief Reference on the robot model.
      RobotWrapper & m_robot;
      std::string m_name;
    };
    
  }
}

#endif // ifndef __invdyn_task_base_hpp__