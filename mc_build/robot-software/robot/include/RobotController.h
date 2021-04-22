#ifndef ROBOT_CONTROLLER_H
#define ROBOT_CONTROLLER_H

#include "Controllers/LegController.h"
#include "Dynamics/FloatingBaseModel.h"
#include <Controllers/StateEstimatorContainer.h>
#include <SimUtilities/VisualizationData.h>
#include "SimUtilities/GamepadCommand.h"
#include "Controllers/DesiredStateCommand.h"

class RobotController {
    friend class RobotRunner;

public:
    RobotController() {}

    virtual ~RobotController() {}

    virtual void initializeController() = 0;

/**
 * Called one time every control loop 
 */
    virtual void runController() = 0;

    virtual void updateVisualization() = 0;

    virtual ControlParameters *getUserControlParameters() = 0;

protected:
    Quadruped<float> *_quadruped;
    FloatingBaseModel<float> *_model;
    LegController<float> *_legController;
    StateEstimatorContainer<float> *_stateEstimator;
    StateEstimate<float> *_stateEstimate;
    GamepadCommand *_driverCommand;
    RobotControlParameters *_controlParameters;
    remote_controller *_remoteController;
    // DesiredStateCommand<float> *_desiredStateCommand = nullptr;

    // customer add part
    Vec4<double> *contact_force;    // wooden
    float* laser_distance;
    float* laser_yaw;

    VisualizationData *_visualizationData;
    RobotType _robotType;
};

#endif
