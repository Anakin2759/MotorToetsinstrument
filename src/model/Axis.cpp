#include "Axis.h"

Axis::Axis(int32_t index)
    : m_index(index), m_velocityStateMachine(new VelocityStateMachine()), m_loadRateCalculator(new LoadRateCalculator())
{
}

Axis::~Axis()
{
    delete m_velocityStateMachine;
    delete m_loadRateCalculator;
}