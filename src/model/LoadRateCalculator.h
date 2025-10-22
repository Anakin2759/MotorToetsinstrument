#pragma once
#include <array>
#include <cmath>
/**********************
负载率计算器


*************** */
class LoadRateCalculator
{
public:
    // 用一个固定大小的数组存储最近四次数据
    std::array<int, 4> torque_values = {0, 0, 0, 0};
    int index                        = 0; // 当前存储数据的位置
    double x_accumulated             = 0.0;
    const double divisor1            = 7524.0;
    const double divisor2            = 2508.0;

public:
    double operator()(int torque)
    {
        torque = std::abs(torque); // 取绝对值
        // 更新环形缓冲区，保存最近四个数据
        x_accumulated -= torque_values[index]; // 移除旧的值
        torque_values[index] = torque;         // 添加新的值
        x_accumulated += torque;               // 加上新的值

        // 更新索引
        index = (index + 1) % 4;

        // 返回根据最近四次数据计算的负载率
        return (x_accumulated / divisor2) * 100.0;
    }
};