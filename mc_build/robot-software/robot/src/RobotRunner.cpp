#include "RobotRunner.h"
#include "Controllers/ContactEstimator.h"
#include "Controllers/OrientationEstimator.h"
#include "Dynamics/Cheetah3.h"
#include "Dynamics/MiniCheetah.h"
#include "Controllers/DesiredStateCommand.h"

#include <Utilities/Utilities_print.h>
#include <ParamHandler.hpp>
#include <Utilities/Timer.h>
#include <unistd.h>
#include <Configuration.h>

#include <Controllers/PositionVelocityEstimator.h>
#include "rt/rt_interface_lcm.h"

RobotRunner::RobotRunner(RobotController *robot_ctrl,
                         PeriodicTaskManager *manager,
                         float period, std::string name) :
        PeriodicTask(manager, period, name),
        _lcm(getLcmUrl(255)), _contactLcm(getLcmUrl(255)) {

    _robot_ctrl = robot_ctrl;
}

/**
 * Initializes the robot model, state estimator, leg controller,
 * robot data, and any control logic specific data.
 */
void RobotRunner::init() {
    printf("[RobotRunner] initialize\n");
    memset(spiCommand, 0, sizeof(SpiCommand));  // avoid random initial spi command
    memset(spiData, 0, sizeof(SpiData));
    // Init the robot leg
    _legController = new LegController<float>(_quadruped);
    _legController->zeroCommand();
    _legController->zeroCommand();

    // Build the appropriate Quadruped object
    if (robotType == RobotType::MINI_CHEETAH) {
        _quadruped = buildMiniCheetah<float>();
    } else {
        _quadruped = buildCheetah3<float>();
    }

    // Initialize the model and robot data
    _model = _quadruped.buildModel();
    _jpos_initializer = new JPosInitializer<float>(3.);

    // Always initialize the leg controller and state entimator

    _stateEstimator = new StateEstimatorContainer<float>(
            cheaterState, vectorNavData, _legController->datas,
            &_stateEstimate, controlParameters);
    initializeStateEstimator(false);

    //memset(&rc_control, 0, sizeof(rc_control_settings));
    memset(&remoteController, 0, sizeof(remote_controller));
    // Initialize the DesiredStateCommand object
//    _desiredStateCommand =
//            new DesiredStateCommand<float>(driverCommand,
//                                           &rc_control,
//                                           controlParameters,
//                                           &_stateEstimate,
//                                           controlParameters->controller_dt);

    // Controller initializations
    _robot_ctrl->_model = &_model;
    _robot_ctrl->_quadruped = &_quadruped;
    _robot_ctrl->_legController = _legController;
    _robot_ctrl->_stateEstimator = _stateEstimator;
    _robot_ctrl->_stateEstimate = &_stateEstimate;
    _robot_ctrl->_visualizationData = visualizationData;
    _robot_ctrl->_robotType = robotType;
    _robot_ctrl->_driverCommand = driverCommand;
    _robot_ctrl->_controlParameters = controlParameters;
    //_robot_ctrl->_desiredStateCommand = _desiredStateCommand;
    _robot_ctrl->_remoteController = &remoteController;

    _robot_ctrl->contact_force = contact_force;  // copy the contact force to the controller
    _robot_ctrl->laser_distance = _laser_distance;
    _robot_ctrl->laser_yaw = _laser_yaw;

    _robot_ctrl->initializeController();
}

/**
 * Runs the overall robot control system by calling each of the major components
 * to run each of their respective steps.
 */
void RobotRunner::run() {
    // Run the state estimator step
    _stateEstimator->run(cheetahMainVisualization);
    cheetahMainVisualization->p = _stateEstimate.position;
    visualizationData->clear();

    // Update the data from the robot
    setupStep();

    static int count_ini(0);  // 静态变量无论申明多少次，整个程序都只有一份
    ++count_ini;
    if (count_ini < 10) {
        _legController->setEnabled(false);
    } else if (20 < count_ini && count_ini < 30) {
        _legController->setEnabled(false);
    } else if (40 < count_ini && count_ini < 50) {
        _legController->setEnabled(false);
    } else {
        // _legController->setEnabled(true);

        // Controller
        if (!_jpos_initializer->IsInitialized(_legController)) {
            Mat3<float> kpMat;
            Mat3<float> kdMat;
            kpMat << 0, 0, 0, 0, 0, 0, 0, 0, 0;
            kdMat << 0.0, 0, 0, 0, 0.0, 0, 0, 0, 0.0;

            for (auto &command : _legController->commands) {
                command.kpJoint = kpMat;
                command.kdJoint = kdMat;
            }
        } else {
            // Run Control
            // std::cout<<"robot runner contact force:"<<contact_force->transpose()<<std::endl;
            _robot_ctrl->runController();

            // Update Visualization
            _robot_ctrl->updateVisualization();
        }
    }



    // Visualization (will make this into a separate function later)
    for (int leg = 0; leg < 4; leg++) {
        for (int joint = 0; joint < 3; joint++) {
            cheetahMainVisualization->q[leg * 3 + joint] =
                    _legController->datas[leg].q[joint];
            // std::cout<<"leg: "<<leg<<" joint: "<<joint<<" position: "<<_legController->datas[leg].q[joint]<<std::endl;
            // by jjg
        }
    }
    // cheetahMainVisualization->q[6]=0.0;  // by jjg
    // cheetahMainVisualization->q[7]=0.0;  // by jjg
    // cheetahMainVisualization->q[8]=0.0;  // by jjg

    cheetahMainVisualization->p.setZero();
    cheetahMainVisualization->p = _stateEstimate.position;

    // Sets the leg controller commands for the robot appropriate commands
    finalizeStep();
}

void RobotRunner::setupStep() {
    // Update the leg data
    if (robotType == RobotType::MINI_CHEETAH) {
        _legController->updateData(spiData);
    } else if (robotType == RobotType::CHEETAH_3) {
        _legController->updateData(tiBoardData);
    } else {
        assert(false);
    }

    // Setup the leg controller for a new iteration
    _legController->zeroCommand();
    // _legController->setEnabled(true);
    // _legController->setMaxTorqueCheetah3(208.5);

    // state estimator
    // check transition to cheater mode:
    if (!_cheaterModeEnabled && controlParameters->cheater_mode) {
        printf("[RobotRunner] Transitioning to Cheater Mode...\n");
        initializeStateEstimator(true);
        // todo any configuration
        _cheaterModeEnabled = true;
    }

    // check transition from cheater mode:
    if (_cheaterModeEnabled && !controlParameters->cheater_mode) {
        printf("[RobotRunner] Transitioning from Cheater Mode...\n");
        initializeStateEstimator(false);
        // todo any configuration
        _cheaterModeEnabled = false;
    }
    // get_rc_control_settings(&rc_control);
    get_rc_control_settings2(&remoteController);
    // todo safety checks, sanity checks, etc...
}

void RobotRunner::finalizeStep() {
    if (robotType == RobotType::MINI_CHEETAH) {
        _legController->updateCommand(spiCommand);
    } else if (robotType == RobotType::CHEETAH_3) {
        _legController->updateCommand(tiBoardCommand);
    } else {
        assert(false);
    }
    _legController->setLcm(&leg_control_data_lcm, &leg_control_command_lcm);
    _stateEstimate.setLcm(state_estimator_lcm);
//    if (REMOTE_CONTROL) {
//        ;
//    } else {
//        _lcm.publish("leg_control_command", &leg_control_command_lcm);
//        _lcm.publish("leg_control_data", &leg_control_data_lcm);
//        _lcm.publish("state_estimator", &state_estimator_lcm);
//        _contactData.contact[0] = (float) (*contact_force)(0);
//        _contactData.contact[1] = (float) (*contact_force)(1);
//        _contactData.contact[2] = (float) (*contact_force)(2);
//        _contactData.contact[3] = (float) (*contact_force)(3);
//        _contactLcm.publish("footpad_contact", &_contactData);
//    };
#if REMOTE_CONTROL==false or IS_SIM==true
    _lcm.publish("leg_control_command", &leg_control_command_lcm);
    _lcm.publish("leg_control_data", &leg_control_data_lcm);
    _lcm.publish("state_estimator", &state_estimator_lcm);
    _contactData.contact[0] = (float) (*contact_force)(0);
    _contactData.contact[1] = (float) (*contact_force)(1);
    _contactData.contact[2] = (float) (*contact_force)(2);
    _contactData.contact[3] = (float) (*contact_force)(3);
    _contactLcm.publish("footpad_contact", &_contactData);
#endif

    _iterations++;
}

void RobotRunner::initializeStateEstimator(bool cheaterMode) {
    _stateEstimator->removeAllEstimators();
    _stateEstimator->addEstimator<ContactEstimator<float>>();
    Vec4<float> contactDefault;
    contactDefault << 0.5, 0.5, 0.5, 0.5;
    _stateEstimator->setContactPhase(contactDefault);
    if (cheaterMode) {
        _stateEstimator->addEstimator<CheaterOrientationEstimator<float>>();
        _stateEstimator->addEstimator<CheaterPositionVelocityEstimator<float>>();
    } else {
        _stateEstimator->addEstimator<VectorNavOrientationEstimator<float>>();
        _stateEstimator->addEstimator<LinearKFPositionVelocityEstimator<float>>();
    }
}

RobotRunner::~RobotRunner() {
    delete _legController;
    delete _stateEstimator;
    delete _jpos_initializer;
}

void RobotRunner::cleanup() {}
