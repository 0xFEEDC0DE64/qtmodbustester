#pragma once

// system includes
#include <memory>

// Qt includes
#include <QDialog>
#include <QModbusDataUnit>

// local includes
#include "changevaluesmodel.h"

// forward declares
class QModbusTcpClient;
class QModbusReply;
namespace Ui {class ChangeValuesDialog; }

class ChangeValuesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeValuesDialog(QModbusTcpClient &modbus, int serverAddress, QModbusDataUnit::RegisterType registerType, QWidget *parent = nullptr);
    ~ChangeValuesDialog() override;

private slots:
    void sendRequest();
    void replyFinished();

private:
    const std::unique_ptr<Ui::ChangeValuesDialog> m_ui;
    QModbusTcpClient &m_modbus;
    ChangeValuesModel m_model;
    std::unique_ptr<QModbusReply> m_reply;
};
