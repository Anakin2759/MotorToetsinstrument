# 电机控制与测试系统 (Motor Control and Testing System)

## 项目简介

这是一个基于EtherCAT协议的电机控制与测试系统，使用Qt框架构建图形用户界面。该系统支持多轴电机控制、数据采集、测试流程设计等功能，适用于工业自动化控制场景。

## 主要功能

- **EtherCAT主站控制**: 基于EtherCAT协议实现多轴电机同步控制
- **伺服驱动器管理**: 支持SZHC402伺服驱动器及其他兼容驱动器
- **数据采集与分析**: 实时采集电机运行数据，支持报表生成
- **测试流程设计**: 可视化测试流程设计器，支持复杂测试序列
- **用户界面**: 现代化的Qt界面，支持多种主题样式
- **配置管理**: 灵活的配置文件管理，支持多种设置格式
- **日志记录**: 完整的日志系统，支持实时显示和文件记录

## 系统架构

```text
src/
├── controller/          # 控制器层
│   ├── MainController.cpp/h    # 主控制器
│   └── interface/              # 接口定义
├── model/              # 数据模型层
│   ├── Axis.cpp/h              # 轴模型
│   ├── EtherCatMaster.cpp/h    # EtherCAT主站
│   ├── MotorDataModel.cpp/h    # 电机数据模型
│   └── ServoDriver_*.cpp/h     # 伺服驱动器实现
├── view/               # 视图层 (Qt界面)
│   ├── MainWindow.cpp/h        # 主窗口
│   ├── MotorDataView.cpp/h     # 电机数据显示
│   └── *Widget.cpp/h           # 各种界面组件
└── utils/              # 工具层
    ├── ConfigManager.h         # 配置管理
    ├── Logger.h                # 日志系统
    ├── ExcelWriter.cpp/h       # Excel报表生成
    └── SQLRW.cpp/h             # 数据库读写
```

## 构建要求

### 系统要求

- Ubuntu
- CMake 3.16+
- Qt 5.15+ 或 Qt 6.x
-
- EtherCAT驱动库 (etherlab/IGH)

### 依赖库

- Qt5/Qt6 (Core, Widgets, Network, SerialPort)
- SQLite3
- Modbus库 (如果使用Modbus功能)
- Excel操作库 (如QtXlsxWriter)

## 构建步骤

1. **克隆项目**

   ```bash
   git clone <repository-url>
   cd MotorControlAndTesting
   ```

2. **安装依赖**
   - 安装Qt开发环境
   - 安装CMake
   - 配置EtherCAT开发环境
   - 安装boost到环境变量

## 配置说明

### 网络配置

- EtherCAT网络接口配置
- IP地址和子网掩码设置

### 电机参数

- 轴参数配置 (位置、速度、加速度限制)
- 伺服驱动器参数设置

### 测试参数

- 测试序列定义
- 数据采集频率设置
- 报表输出格式配置

## 使用说明

1. **启动应用**: 运行主程序，等待EtherCAT网络初始化
2. **配置电机**: 在设置界面配置各轴参数
3. **设计测试流程**: 使用流程设计器创建测试序列
4. **执行测试**: 启动测试，实时监控数据
5. **生成报表**: 测试完成后自动生成Excel报表

## 开发说明

### 代码规范

- 使用C++17标准
- 遵循Qt编码规范
- 使用智能指针管理内存
- 异常安全编程

### 调试技巧

- 使用Qt Creator进行调试
- 查看EtherCAT网络状态
- 分析日志文件定位问题

## 许可证

本项目采用MIT许可证。详见LICENSE文件。

## 贡献

欢迎提交Issue和Pull Request来改进项目。
