#pragma once

#include <cmath>

// 枚举运动状态
enum class MotionState
{
    Stopped,      // 停止
    Accelerating, // 加速
    Decelerating, // 减速
    ConstantSpeed // 恒速
};

// 状态机类
class VelocityStateMachine
{
public:
    explicit VelocityStateMachine(int maxAccel = 20, int maxDecel = 20, int deltaTime_ms = 1)
        : m_maxAccel(maxAccel),      // 最大加速度（脉冲/毫秒）
          m_maxDecel(maxDecel),      // 最大减速度（脉冲/毫秒）
          m_deltaTime(deltaTime_ms), // 帧时间（毫秒）
          m_targetSpeed(0),
          m_actualSpeed(0),
          m_state(MotionState::Stopped)
    {
    }

    void reset()
    {
        m_targetSpeed = 0;
        m_actualSpeed = 0;
        m_state       = MotionState::Stopped;
    }

    // 更新速度，返回当前速度（脉冲/毫秒）
    int update()
    {
        switch (m_state)
        {
            case MotionState::Accelerating:
                accelerate();
                break;
            case MotionState::Decelerating:
                decelerate();
                break;
            case MotionState::ConstantSpeed:
                maintainSpeed();
                break;
            case MotionState::Stopped:
                stopMotion();
                break;
        }

        return m_actualSpeed; // 返回每毫秒的实际速度（脉冲/毫秒）
    }

    // 设置目标速度
    void setTargetSpeed(int targetSpeed)
    {
        m_targetSpeed = targetSpeed;

        if (m_targetSpeed == 0)
        {
            m_state = MotionState::Decelerating; // 减速停止
        }
        else if (m_targetSpeed > m_actualSpeed)
        {
            m_state = MotionState::Accelerating; // 加速
        }
        else if (m_targetSpeed < m_actualSpeed)
        {
            m_state = MotionState::Decelerating; // 减速
        }
        else
        {
            m_state = MotionState::ConstantSpeed; // 已经达到目标速度
        }
    }

    MotionState getState() const
    {
        return m_state;
    }

private:
    int m_maxAccel;    // 最大加速度（脉冲/毫秒）
    int m_maxDecel;    // 最大减速度（脉冲/毫秒）
    int m_deltaTime;   // 帧时间（毫秒）
    int m_targetSpeed; // 目标速度（脉冲/毫秒）
    int m_actualSpeed; // 实际速度（脉冲/毫秒）
    MotionState m_state;

    // 加速逻辑
    void accelerate()
    {
        // 限制每毫秒加速不超过 m_maxAccel 个脉冲
        int speedIncrement = std::min(m_maxAccel, m_targetSpeed - m_actualSpeed);

        // 更新实际速度
        m_actualSpeed += speedIncrement;

        // 如果实际速度达到或超过目标速度，切换为恒速状态
        if (m_actualSpeed >= m_targetSpeed)
        {
            m_actualSpeed = m_targetSpeed;
            m_state       = MotionState::ConstantSpeed; // 切换为恒速状态
        }
    }

    // 减速逻辑
    void decelerate()
    {
        // 限制每毫秒减速不超过 m_maxDecel 个脉冲
        int speedDecrement = std::min(m_maxDecel, m_actualSpeed - m_targetSpeed);

        // 更新实际速度
        m_actualSpeed -= speedDecrement;

        // 如果实际速度达到目标速度或停止，切换状态
        if (m_actualSpeed <= m_targetSpeed)
        {
            m_actualSpeed = m_targetSpeed;

            if (m_targetSpeed == 0)
            {
                m_state = MotionState::Stopped; // 停止状态
            }
            else
            {
                m_state = MotionState::ConstantSpeed; // 恒速状态
            }
        }
    }

    // 恒速逻辑
    void maintainSpeed()
    {
        // 保持恒速
    }

    // 停止逻辑
    void stopMotion()
    {
        m_actualSpeed = 0;
    }
};
