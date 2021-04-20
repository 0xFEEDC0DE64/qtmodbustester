#pragma once

// system includes
#include <memory>

// Qt includes
#include <QMainWindow>
#include <QElapsedTimer>

// forward declares
class QModbusTcpClient;
class QModbusReply;
namespace Ui { class MainWindow; }
class ModbusTableModel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void connectPressed();
    void requestPressed();

    void modbusErrorOccured(int error);
    void modbusStateChanged(int state);

    void replyFinished();

private:
    void updateRequestFields();

    const std::unique_ptr<Ui::MainWindow> m_ui;
    const std::unique_ptr<QModbusTcpClient> m_modbus;
    const std::unique_ptr<ModbusTableModel> m_model;
    std::unique_ptr<QModbusReply> m_reply;
    QElapsedTimer m_timer;
};
