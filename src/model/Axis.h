#pragma once
#include <atomic>

#include "LoadRateCalculator.h"
#include "VelocityStateMachine.h"

class Axis
{
public:
    explicit Axis(int32_t index = 0);
    ~Axis();

    void setTargetSpeed(int32_t velocity)
    {
        m_velocityStateMachine->setTargetSpeed(velocity);
    }
    int32_t getCalculatedVelocity()
    {
        return m_velocityStateMachine->update();
    };

    inline const size_t& getIndex()
    {
        return m_index;
    }
    double getLoadRate(int32_t torque)
    {
        return (*m_loadRateCalculator)(torque);
    }

private:
    size_t m_index;

    VelocityStateMachine* m_velocityStateMachine;
    LoadRateCalculator* m_loadRateCalculator;
};