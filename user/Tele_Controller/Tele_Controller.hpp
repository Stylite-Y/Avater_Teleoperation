/*
Note at the very beginning
Suppose leg2 is the control side (by human)
leg3 is the implementation side
The relation between leg2 and leg3 is that:
leg2 is total controlled by human
leg2's angle is leg3's target
This is the simplist method for teleoperation, sofa, human can't feel the
feedback
*/
#include <RobotController.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <lcm/lcm-cpp.hpp>
#include <mutex>
#include <random>
#include <string>

#include "Tele_UserParameters.h"
#define Transform_Filter_Ratio 0.999

class Tele_Controller : public RobotController {
public:
  Tele_Controller() : RobotController() { ; };
  ~Tele_Controller() override = default;

  void initializeController() override;

  void runController() override;

  void updateVisualization() override;

  ControlParameters *getUserControlParameters() override {
    return &userParameters;
  }

private:
  void updateManualCommand();
  void updateUserParameter();
  Tele_UserParameters userParameters;
  // lcm::LCM _lcm; // lcm communication object
  std::mutex _mutex;

  float BeltGear = 1.0f;
  bool leg_control_enable[4] = {false, false, false,
                                false}; // if ture, motor will be enable;

  float stiff_t[3] = {0.0f, 0.0f, 0.0f}; // current stiffness
  float stiff_aim[3] = {0.0f, 0.0f,
                        0.0f}; // aim stiffness, setted by user parameter
  float damping_t[3] = {0.0f, 0.0f, 0.0f}; // current damping
  float damping_aim[3] = {0.0f, 0.0f,
                          0.0f}; // aim damping, setted by user parameter
};