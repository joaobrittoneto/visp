/****************************************************************************
 *
 * $Id: vpRobotCamera.cpp,v 1.9 2007-04-27 16:40:15 fspindle Exp $
 *
 * Copyright (C) 1998-2006 Inria. All rights reserved.
 *
 * This software was developed at:
 * IRISA/INRIA Rennes
 * Projet Lagadic
 * Campus Universitaire de Beaulieu
 * 35042 Rennes Cedex
 * http://www.irisa.fr/lagadic
 *
 * This file is part of the ViSP toolkit.
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech AS of Norway and appearing in the file
 * LICENSE included in the packaging of this file.
 *
 * Licensees holding valid ViSP Professional Edition licenses may
 * use this file in accordance with the ViSP Commercial License
 * Agreement provided with the Software.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Contact visp@irisa.fr if any conditions of this licensing are
 * not clear to you.
 *
 * Description:
 * Defines the simplest robot : a free flying camera.
 *
 * Authors:
 * Eric Marchand
 *
 *****************************************************************************/


/*!
  \file vpRobotCamera.cpp
  \brief class that defines the simplest robot : a free flying camera
*/

#include <visp/vpHomogeneousMatrix.h>
#include <visp/vpRobotCamera.h>
#include <visp/vpRobotException.h>
#include <visp/vpDebug.h>
#include <visp/vpExponentialMap.h>

//! basic initialization
void vpRobotCamera::init()
{
  eJe.resize(6,6) ;
  eJe.setIdentity() ;
}

//! constructor
vpRobotCamera::vpRobotCamera()
{
  init() ;
}


//! constructor
vpRobotCamera::~vpRobotCamera()
{
}

/*

AT LEAST ONE OF THESE TWO FUNCTIONS HAS TO BE IMPLEMENTED

get_eJe
get_fJe

*/

//! get the robot Jacobian expressed in the end-effector frame
//! for that simple robot the Jacobian is the identity
void
vpRobotCamera::get_eJe(vpMatrix &_eJe)
{
  _eJe = eJe ;
}

//! get the robot Jacobian expressed in the robot reference frame
void
vpRobotCamera::get_fJe(vpMatrix & /* _fJe */)
{
  std::cout << "Not implemented ! " << std::endl;
}


/*

AT LEAST ONE OF THESE TWO FUNCTIONS HAS TO BE IMPLEMENTED

sendCameraVelocity
sendArticularVelocity


*/

//! send to the controller a velocity expressed in the camera frame
//! we use the exponential map to update the camera location
void
vpRobotCamera::setCameraVelocity(const vpColVector &v)
{
  cMo = vpExponentialMap::direct(v).inverse()*cMo ;
}

//! send to the controller a velocity expressed in the articular frame
void
vpRobotCamera::setArticularVelocity(const vpColVector &qdot)
{
  cMo = vpExponentialMap::direct(qdot).inverse()*cMo ;
}

//! send to the controller a velocity (frame as to ve specified)
void
vpRobotCamera::setVelocity(const vpRobot::ControlFrameType frame,
			   const vpColVector &vel)
{
  switch (frame)
  {
  case vpRobot::REFERENCE_FRAME:
    break ;
  case vpRobot::CAMERA_FRAME:
    setCameraVelocity(vel) ;
    break ;
  case vpRobot::ARTICULAR_FRAME:
    setArticularVelocity(vel) ;
    break ;
  case vpRobot::MIXT_FRAME:
    vpERROR_TRACE ("Cannot set a velocity in the mixt frame: "
		 "functionality not implemented");
    throw vpRobotException (vpRobotException::wrongStateError,
			    "Cannot get a velocity in the reference frame:"
			    "functionality not implemented");

    break ;
  }
}


/*

THESE FUNCTIONS ARE NOT MENDATORY BUT ARE USUALLY USEFUL

*/

//! get a position expressed in the robot reference frame
//! we consider that the "robot" reference frame is the world reference
//! so we return cMo (or at least the corresponding vpPoseVector)
void
vpRobotCamera::getPosition(vpColVector & /*cpo*/)
{
  std::cout << "Not implemented ! " << std::endl;
  //  cpo.buildFrom(cMo) ;
}
//! get a position expressed in the robot reference frame
void
vpRobotCamera::getPosition(vpHomogeneousMatrix &_cMo) const
{
  _cMo = cMo ;
}
//! get a position expressed in the robot reference frame
void
vpRobotCamera::setPosition(const vpHomogeneousMatrix &_cMo)
{
   cMo = _cMo ;
}

//! get a position expressed in the articular frame
void
vpRobotCamera::getArticularPosition(vpColVector &/* q */) const
{
  std::cout << "Not implemented ! " << std::endl;
}

//! get a displacement (frame as to ve specified)
void
vpRobotCamera::getPosition(const vpRobot::ControlFrameType /* frame */,
			   vpColVector & /* q */)
{
  std::cout << "Not implemented ! " << std::endl;
}

//! get a displacement expressed in the camera frame
void
vpRobotCamera::getCameraDisplacement(vpColVector & /* v */)
{
  std::cout << "Not implemented ! " << std::endl;
}

//! get a displacement expressed  in the articular frame
void
vpRobotCamera::getArticularDisplacement(vpColVector & /* qdot */)
{
  std::cout << "Not implemented ! " << std::endl;
}

//! get a displacement (frame as to ve specified)
void
vpRobotCamera::getDisplacement(const vpRobot::ControlFrameType /* frame */,
			       vpColVector &/* q */)
{
  std::cout << "Not implemented ! " << std::endl;
}


/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
