
#include "ControlParameters/ControlParameters.h"
#define Ttype double

class Tele_UserParameters : public ControlParameters {
public:
    Tele_UserParameters():
    ControlParameters("user-parameters"),
    INIT_PARAMETER(abad_stiff),
    INIT_PARAMETER(hip_stiff),
    INIT_PARAMETER(knee_stiff),
    INIT_PARAMETER(abad_damping),
    INIT_PARAMETER(hip_damping),
    INIT_PARAMETER(knee_damping)
    {}
    DECLARE_PARAMETER(Ttype, abad_stiff);
    DECLARE_PARAMETER(Ttype, hip_stiff);
    DECLARE_PARAMETER(Ttype, knee_stiff);
    DECLARE_PARAMETER(Ttype, abad_damping);
    DECLARE_PARAMETER(Ttype, hip_damping);
    DECLARE_PARAMETER(Ttype, knee_damping);
};