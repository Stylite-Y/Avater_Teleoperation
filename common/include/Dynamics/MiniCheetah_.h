/*! @file BlankPanther.h
 *  @brief Utility function to build a Blank Panther Quadruped object
 *
 * ref: https://github.com/mit-biomimetics/Cheetah-Software/blob/master/common/include/Dynamics/MiniCheetah.h
 * auther:Wooden Jin
 * Copyright@Xmech_ZJU
 * this script is a modeify of minicheetah.h file. the quadruped build in xmech refer to mit's minicheetah
 * but there is some small modification of the hardware, to use mit_controller, we write this header file
 */

#ifndef PROJECT_MINICHEETAH_H
#define PROJECT_MINICHEETAH_H

#include "FloatingBaseModel.h"
#include "Quadruped.h"

/*!
 * Generate a Quadruped model of Mini Cheetah
 */
template<typename T>
Quadruped<T> buildMiniCheetah() {
    Quadruped<T> cheetah;
    cheetah._robotType = RobotType::MINI_CHEETAH;

    cheetah._bodyMass = 3.3;
    cheetah._bodyLength = 0.212 * 2;
    cheetah._bodyWidth = 0.058 * 2;
    cheetah._bodyHeight = 0.046 * 2;
    cheetah._abadGearRatio = 6;
    cheetah._hipGearRatio = 6;
    // cheetah._kneeGearRatio = 9.375;
    cheetah._kneeGearRatio = 6;

    cheetah._abadLinkLength = 0.085;
    cheetah._hipLinkLength = 0.201;
    cheetah._kneeLinkLength = 0.19;
    // cheetah._kneeLinkLength = 0.21;
    cheetah._maxLegLength = 0.384;

    cheetah._motorTauMax = 3.f;
    cheetah._batteryV = 24;
    cheetah._motorKT = .05;  // this is flux linkage * pole pairs
    cheetah._motorR = 0.173;
    cheetah._jointDamping = .01;
    cheetah._jointDryFriction = .2;
    // cheetah._jointDamping = .00;
    // cheetah._jointDryFriction = .0;

    // rotor inertia if the rotor is oriented so it spins around the z-axis
    Mat3<T> rotorRotationalInertiaZ;
    rotorRotationalInertiaZ << 33, 0, 0, 0, 33, 0, 0, 0, 63;
    rotorRotationalInertiaZ = 1e-6 * rotorRotationalInertiaZ;

    Mat3<T> RY = coordinateRotation<T>(CoordinateAxis::Y, M_PI / 2);
    Mat3<T> RX = coordinateRotation<T>(CoordinateAxis::X, M_PI / 2);
    Mat3<T> rotorRotationalInertiaX =
            RY * rotorRotationalInertiaZ * RY.transpose();
    Mat3<T> rotorRotationalInertiaY =
            RX * rotorRotationalInertiaZ * RX.transpose();

    // spatial inertias
    Mat3<T> abadRotationalInertia;
    // abadRotationalInertia << 381, 58, 0.45, 58, 560, 0.95, 0.45, 0.95, 444;
    abadRotationalInertia << 488, -21, 0, -21, 739, 0, 0, 0, 391;
    abadRotationalInertia = abadRotationalInertia * 1e-6;
    // Vec3<T> abadCOM(0, 0.036, 0);  // LEFT
    // Vec3<T> abadCOM(0, 0.051, 0);
    Vec3<T> abadCOM(0, 0.0588, 0);
    SpatialInertia<T> abadInertia(0.54, abadCOM, abadRotationalInertia);

    Mat3<T> hipRotationalInertia;
    // hipRotationalInertia << 1983, 245, 13, 245, 2103, 1.5, 13, 1.5, 408;
    hipRotationalInertia << 1724, 0, 0, 0, 1907, -228, 0, -228, 468;
    hipRotationalInertia = hipRotationalInertia * 1e-6;
    // Vec3<T> hipCOM(0, 0.016, -0.02);
    Vec3<T> hipCOM(0, 0.018, -0.018);
    SpatialInertia<T> hipInertia(0.634, hipCOM, hipRotationalInertia);

    Mat3<T> kneeRotationalInertia, kneeRotationalInertiaRotated;
    // kneeRotationalInertiaRotated << 6, 0, 0, 0, 248, 0, 0, 0, 245;
    kneeRotationalInertiaRotated << 12, 0, 0, 0, 721, 0, 0, 0, 716;
    kneeRotationalInertiaRotated = kneeRotationalInertiaRotated * 1e-6;
    kneeRotationalInertia = RY * kneeRotationalInertiaRotated * RY.transpose();
    Vec3<T> kneeCOM(0, 0, -0.086);
    SpatialInertia<T> kneeInertia(0.117, kneeCOM, kneeRotationalInertia);

    Vec3<T> rotorCOM(0, 0, 0);
    SpatialInertia<T> rotorInertiaX(0.055, rotorCOM, rotorRotationalInertiaX);
    SpatialInertia<T> rotorInertiaY(0.055, rotorCOM, rotorRotationalInertiaY);

    Mat3<T> bodyRotationalInertia;
    // bodyRotationalInertia << 11253, 0, 0, 0, 36203, 0, 0, 0, 42673;
    bodyRotationalInertia << 13355, 0, 0, 0, 40234, 0, 0, 0, 48945;
    bodyRotationalInertia = bodyRotationalInertia * 1e-6;
    Vec3<T> bodyCOM(0, 0, 0);
    SpatialInertia<T> bodyInertia(cheetah._bodyMass, bodyCOM,
                                  bodyRotationalInertia);

    cheetah._abadInertia = abadInertia;
    cheetah._hipInertia = hipInertia;
    cheetah._kneeInertia = kneeInertia;
    cheetah._abadRotorInertia = rotorInertiaX;
    cheetah._hipRotorInertia = rotorInertiaY;
    cheetah._kneeRotorInertia = rotorInertiaY;
    cheetah._bodyInertia = bodyInertia;

    // locations
    cheetah._abadRotorLocation = Vec3<T>(0.1076, 0.048, 0);
    cheetah._abadLocation =
            Vec3<T>(cheetah._bodyLength, cheetah._bodyWidth, 0) * 0.5;
    cheetah._hipLocation = Vec3<T>(0, cheetah._abadLinkLength, 0);
    cheetah._hipRotorLocation = Vec3<T>(0, 0.04, 0);
    cheetah._kneeLocation = Vec3<T>(0, 0, -cheetah._hipLinkLength);
    cheetah._kneeRotorLocation = Vec3<T>(0, 0, 0);

    return cheetah;
}

#endif  // PROJECT_MINICHEETAH_H
