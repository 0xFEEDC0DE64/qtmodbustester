#pragma once

// system includes
#include <memory>

// Qt includes
#include <QDialog>

// local includes
#include "changevaluesmodel.h"

// forward declares
class QModbusTcpClient;
namespace Ui {class ChangeValuesDialog; }

class ChangeValuesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeValuesDialog(QModbusTcpClient &modbus, QWidget *parent = nullptr);
    ~ChangeValuesDialog() override;

private slots:
    void sendRequest();

private:
    const std::unique_ptr<Ui::ChangeValuesDialog> m_ui;
    QModbusTcpClient &m_modbus;
    ChangeValuesModel m_model;
};
