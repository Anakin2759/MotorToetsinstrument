#pragma once

#include <QString>

struct MotorViewData
{
    int servoAddress   = 0;
    int index          = 0;
    int velocity       = 0;
    int position       = 0;
    int torque         = 0;
    int loadrate       = 0;
    double temperature = 0;
    QString status{"未知"};
    QString power{"0W"};
    QString sn{"@"};
    QString errorCode{"0 无报警"};
    QString runTime{"0:0:0:0"};
    QString remainTime{"0:0:0:0"};
    int targetVelocity = 600;

    QString vendor{""};
};
