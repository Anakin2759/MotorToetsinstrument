#pragma once
#include <QCheckBox>
#include <QLabel>
#include <QWidget>

class ServoDriverWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ServoDriverWidget(QWidget* parent = nullptr);
    void paintEvent(QPaintEvent* event) override;
    inline void setServoDriverInfo(const QString& index, const QString& name, const QString& info)
    {
        if (m_IndexLabel)
        {
            m_IndexLabel->setText(index);
        }
        if (m_NameLabel)
        {
            m_NameLabel->setText(name);
        }
        if (m_driverInfoLabel)
        {
            m_driverInfoLabel->setText(info);
        }
    }
    inline bool getChecked() const
    {
        return m_checkBox->isChecked();
    }

private:
    QLabel* m_IndexLabel{nullptr};
    QLabel* m_NameLabel{nullptr};
    QLabel* m_driverInfoLabel{nullptr};
    QCheckBox* m_checkBox{nullptr};
};