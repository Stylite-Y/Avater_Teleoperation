
#include "Tele_Controller.hpp"
#include <Configuration.h>
#include <assert.h>
#include <rt/rt_interface_lcm.h>

void Tele_Controller::updateUserParameter() {
  stiff_aim[0] = (float)userParameters.abad_stiff;
  stiff_aim[1] = (float)userParameters.hip_stiff;
  stiff_aim[2] = (float)userParameters.knee_stiff;
  damping_aim[0] = (float)userParameters.abad_damping;
  damping_aim[1] = (float)userParameters.hip_damping;
  damping_aim[2] = (float)userParameters.knee_damping;
}

/*
Lord control parameter here and do something initialize work
*/
void Tele_Controller::initializeController() {
  // ---------------
  // load parameter
  //#pragma region
  updateUserParameter();
  //#pragma endregion

  // ---------------
  // initialize
  //#pragma region
  BeltGear = (IS_SIM) ? 1.0f : 2.4025f;

  //#pragma endregion
}

/*
Update gamepad command
*/
void Tele_Controller::updateManualCommand() {
  updateUserParameter();
  leg_control_enable[3] =
      (_driverCommand->a)
          ? true
          : leg_control_enable[3]; // if a is pressed, enable motor
  leg_control_enable[3] =
      (_driverCommand->b)
          ? false
          : leg_control_enable[3]; // if b is pressed. disable motor
  // _legController->setEnabled(leg_control_enable);
    _legController->setEnabled(leg_control_enable[3]);

  // transform stiffness and damping to zeros
  if (_driverCommand->leftTriggerButton & leg_control_enable[3]) {
    for (int i = 0; i < 3; i++) {
      stiff_t[i] = stiff_t[i] * Transform_Filter_Ratio +
                   0.0f * (1.0f - Transform_Filter_Ratio);
      damping_t[i] = damping_t[i] * Transform_Filter_Ratio +
                     0.0f * (1.0f - Transform_Filter_Ratio);
    }
  }
  // transform stiffness and damping from current to aim value
  if (_driverCommand->rightTriggerButton & leg_control_enable[3]) {
    for (int i = 0; i < 3; i++) {
      stiff_t[i] = stiff_t[i] * Transform_Filter_Ratio +
                   stiff_aim[i] * (1.0f - Transform_Filter_Ratio);
      damping_t[i] = damping_t[i] * Transform_Filter_Ratio +
                     damping_aim[i] * (1.0f - Transform_Filter_Ratio);
    }
  }
}

/*
main control code
*/
void Tele_Controller::runController() {
  updateManualCommand(); // update command

  _mutex.lock();
  _legController->commands[3].kpJoint << stiff_t[0], 0.0f, 0.0f, 0.0f,
      stiff_t[1], 0.0f, 0.0f, 0.0f, stiff_t[2] / BeltGear;
  _legController->commands[3].kdJoint << damping_t[0], 0.0f, 0.0f, 0.0f,
      damping_t[1], 0.0f, 0.0f, 0.0f, damping_t[2] / BeltGear;
  _legController->commands[3].qDes
      << _legController->datas[2].q; // leg2's angle is aim of leg 3
  _mutex.unlock();
}

void Tele_Controller::updateVisualization() {
  // TODO: so far, do nothing
  ;
}