#pragma once

// system includes
#include <memory>

// Qt includes
#include <QMainWindow>
#include <QElapsedTimer>
#include <QStringListModel>
#include <QSettings>
#include <QCompleter>
#include <QSettings>
#include <qmodbusdevice.h>

// forward declares
class QModbusClient;
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
    void refreshSerialPorts();

    void connectPressed();
    void requestPressed();
    void writePressed();

    void modbusErrorOccured(int error);
    void modbusStateChanged(int state);

    void replyFinished();
    void replyErrorOccurred(QModbusDevice::Error error);
    void replyIntermediateErrorOccurred(QModbusDevice::IntermediateError error);

private:
    void updateRequestFields();

    const std::unique_ptr<Ui::MainWindow> m_ui;
    QSettings m_settings;
    std::unique_ptr<QModbusClient> m_modbus;
    const std::unique_ptr<ModbusTableModel> m_model;
    std::unique_ptr<QModbusReply> m_reply;
    QElapsedTimer m_timer;
    QStringListModel m_completerModel;
    QCompleter m_completer;
};
