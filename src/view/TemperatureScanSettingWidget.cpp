#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <filesystem>

#include "TemperatureScanSettingWidget.h"
#include "utils/Logger.h"

void populateAvailablePorts(QComboBox* modbus_portComboBox)
{
    std::filesystem::path devDir("/dev"); // 指定设备目录
    if (std::filesystem::exists(devDir) && std::filesystem::is_directory(devDir))
    {
        for (const auto& entry : std::filesystem::directory_iterator(devDir))
        {
            std::string filename = entry.path().filename().string();
            // 根据需要过滤端口名称
            if (filename.find("ttyUSB") != std::string::npos || filename.find("ttyACM") != std::string::npos)
            {
                LOG_TRACE("添加可用端口: {}", filename);
                filename = "/dev/" + filename;
                modbus_portComboBox->addItem(QString::fromStdString(filename));
            }
        }
    }
}
TemperatureScanSettingWidget::TemperatureScanSettingWidget(QWidget* parent) : QWidget(parent)
{
    QGridLayout* layout             = new QGridLayout(this);
    QLabel* startStationNumberLabel = new QLabel("第一个从站号", this);
    QLabel* endStationNumberLabel   = new QLabel("最后一个从站号", this);
    QLabel* comLabel                = new QLabel("串口号", this);
    QLabel* baudrateLabel           = new QLabel("波特率", this);
    QLabel* dataBitsLabel           = new QLabel("数据位", this);
    QLabel* stopBitsLabel           = new QLabel("停止位", this);
    QLabel* parityLabel             = new QLabel("校验位", this);

    comComboBox = new QComboBox(this);
    populateAvailablePorts(comComboBox);

    startStationNumberEdit = new QLineEdit(this);
    endStationNumberEdit   = new QLineEdit(this);
    baudrateEdit           = new QLineEdit(this);
    dataBitsEdit           = new QLineEdit(this);
    stopBitsEdit           = new QLineEdit(this);
    parityEdit             = new QLineEdit(this);

    baudrateEdit->setText("9600");
    dataBitsEdit->setText("8");
    stopBitsEdit->setText("1");
    parityEdit->setText("N");

    connectButton    = new QPushButton("连接", this);
    disconnectButton = new QPushButton("断开", this);

    size_t i = 0;

    layout->addWidget(comLabel, i, 0);
    layout->addWidget(comComboBox, i++, 1);

    layout->addWidget(startStationNumberLabel, i, 0);
    layout->addWidget(startStationNumberEdit, i++, 1);
    layout->addWidget(endStationNumberLabel, i, 0);
    layout->addWidget(endStationNumberEdit, i++, 1);

    layout->addWidget(baudrateLabel, i, 0);
    layout->addWidget(baudrateEdit, i++, 1);
    layout->addWidget(dataBitsLabel, i, 0);
    layout->addWidget(dataBitsEdit, i++, 1);
    layout->addWidget(stopBitsLabel, i, 0);
    layout->addWidget(stopBitsEdit, i++, 1);
    layout->addWidget(parityLabel, i, 0);
    layout->addWidget(parityEdit, i++, 1);

    layout->addWidget(connectButton, i, 0);
    layout->addWidget(disconnectButton, i++, 1);

    layout->setColumnStretch(0, 1); // 第一列的宽度占 2 的比例
    layout->setColumnStretch(1, 1); // 第二列的宽度占 1 的比例

    connect(connectButton, &QPushButton::clicked,
            [this]()
            {
                size_t startStationNumber;
                size_t endStationNumber;
                std::string com;
                int baudrate;
                int dataBits;
                int stopBits;
                char parity;

                bool ok;
                startStationNumber = startStationNumberEdit->text().toInt(&ok);
                if (!ok)
                {
                    startStationNumber = std::numeric_limits<size_t>::max();
                    LOG_ERROR("无效的起始站号");
                }

                endStationNumber = endStationNumberEdit->text().toInt(&ok);
                if (!ok)
                {
                    endStationNumber = std::numeric_limits<size_t>::max();
                    LOG_ERROR("无效的结束站号");
                }
                if (startStationNumber > endStationNumber)
                {
                    startStationNumber = std::numeric_limits<size_t>::max();
                    endStationNumber   = std::numeric_limits<size_t>::max();
                    LOG_ERROR("起始站号大于终止站号");
                }
                if (comComboBox->count() == 0)
                {
                    com = "";
                    LOG_ERROR("ComboBox 中没有任何项");
                }
                else
                {
                    com = comComboBox->currentText().toStdString();
                    // ComboBox 中有项，可以继续操作
                }

                baudrate = baudrateEdit->text().toInt(&ok);
                if (!ok)
                {
                    baudrate = 9600;
                    LOG_ERROR("无效的波特率");
                }

                dataBits = dataBitsEdit->text().toInt(&ok);
                if (!ok)
                {
                    dataBits = 8;
                    LOG_ERROR("无效的数据位");
                }

                stopBits = stopBitsEdit->text().toInt(&ok);
                if (!ok)
                {
                    stopBits = 1;
                    LOG_ERROR("无效的停止位");
                }

                QString parityText = parityEdit->text().toUpper(); // 转换为大写以避免大小写问题
                parity             = 'N';                          // 默认值为 'N'

                // 检查输入是否合法
                if (parityText == "N" || parityText == "E" || parityText == "O")
                {
                    parity = parityText.toStdString()[0]; // 转换为 char 类型
                }
                else
                {
                    LOG_ERROR("无效的校验位");
                }

                emit startTemperatureScan(startStationNumber, endStationNumber, com, baudrate, dataBits, stopBits,
                                          parity);
            });
    connect(disconnectButton, &QPushButton::clicked, [this]() { emit stopTemperatureScan(); });
}

TemperatureScanSettingWidget::~TemperatureScanSettingWidget()
{
}