#include "mainwindow.h"
#include "ui_mainwindow.h"

// Qt includes
#include <QDebug>
#include <QModbusTcpClient>
#include <QModbusReply>
#include <QMetaEnum>
#include <QMessageBox>
#include <QMetaType>
#include <QTimer>

// local includes
#include "modbustablemodel.h"
#include "changevaluesdialog.h"

// utilities
namespace {
QDebug operator<<(QDebug debug, QModbusDataUnit::RegisterType registerType);
QString toString(QModbusDataUnit::RegisterType registerType);
} // namespace

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{parent},
    m_ui{std::make_unique<Ui::MainWindow>()},
    m_modbus{std::make_unique<QModbusTcpClient>(this)},
    m_model{std::make_unique<ModbusTableModel>(this)}
{
    m_ui->setupUi(this);

    m_ui->spinBoxTimeout->setValue(m_modbus->timeout());
    connect(m_modbus.get(), &QModbusClient::timeoutChanged, m_ui->spinBoxTimeout, &QSpinBox::setValue);
    connect(m_ui->spinBoxTimeout, &QSpinBox::valueChanged, m_modbus.get(), &QModbusClient::setTimeout);

    m_ui->spinBoxRetries->setValue(m_modbus->numberOfRetries());
    //connect(m_modbus.get(), &QModbusClient::numberOfRetriesChanged, m_ui->spinBoxRetries, &QSpinBox::setValue);
    connect(m_ui->spinBoxRetries, &QSpinBox::valueChanged, m_modbus.get(), &QModbusClient::setNumberOfRetries);

    modbusStateChanged(m_modbus->state());

    connect(m_modbus.get(), &QModbusClient::errorOccurred,
                      this, &MainWindow::modbusErrorOccured);

    connect(m_modbus.get(), &QModbusClient::stateChanged,
                      this, &MainWindow::modbusStateChanged);

    {
        const auto addItem = [&](const auto &text, const auto &value){
            m_ui->comboBoxType->addItem(text, QVariant::fromValue<QModbusDataUnit::RegisterType>(value));
        };
        addItem(tr("Discrete Inputs"),   QModbusDataUnit::DiscreteInputs);
        addItem(tr("Coils"),             QModbusDataUnit::Coils);
        addItem(tr("Input Registers"),   QModbusDataUnit::InputRegisters);
        addItem(tr("Holding Registers"), QModbusDataUnit::HoldingRegisters);
    }
    m_ui->comboBoxType->setCurrentIndex(
                m_ui->comboBoxType->findData(
                    QVariant::fromValue<QModbusDataUnit::RegisterType>(QModbusDataUnit::HoldingRegisters)
                )
           );

    connect(m_ui->pushButtonConnect, &QAbstractButton::pressed, this, &MainWindow::connectPressed);
    connect(m_ui->pushButtonRequest, &QAbstractButton::pressed, this, &MainWindow::requestPressed);
    connect(m_ui->pushButtonWrite,   &QAbstractButton::pressed, this, &MainWindow::writePressed);

    m_ui->tableView->setModel(m_model.get());
}

MainWindow::~MainWindow() = default;

void MainWindow::connectPressed()
{
    if (m_reply)
    {
        QMessageBox::warning(this,
                             tr("Another request is still pending!"),
                             tr("Another request is still pending!"));
        return;
    }

    switch (const auto state = m_modbus->state())
    {
    case QModbusDevice::ConnectedState:
        m_modbus->disconnectDevice();
        break;
    case QModbusDevice::UnconnectedState:
        m_modbus->setConnectionParameter(QModbusDevice::NetworkAddressParameter, m_ui->lineEditServer->text());
        m_modbus->setConnectionParameter(QModbusDevice::NetworkPortParameter, m_ui->spinBoxPort->value());
        if (!m_modbus->connectDevice())
        {

        }
        break;
    default:
        QMessageBox::warning(this,
                             tr("Modbus client is in wrong state"),
                             tr("Modbus client is in wrong state:\n\n%0")
                                .arg(QMetaEnum::fromType<QModbusDevice::State>().valueToKey(state))
                             );
    }
}

void MainWindow::requestPressed()
{
    if (const auto state = m_modbus->state(); state != QModbusDevice::ConnectedState)
    {
        QMessageBox::warning(this,
                             tr("Modbus client is in wrong state"),
                             tr("Modbus client is in wrong state:\n\n%0")
                                .arg(QMetaEnum::fromType<QModbusDevice::State>().valueToKey(state))
                             );
        return;
    }

    if (m_reply)
    {
        QMessageBox::warning(this,
                             tr("Another request is still pending!"),
                             tr("Another request is still pending!"));
        return;
    }

    const auto registerType = m_ui->comboBoxType->currentData();
    if (!registerType.isValid() || !registerType.canConvert<QModbusDataUnit::RegisterType>())
    {
        qDebug() << registerType << registerType.typeName() << registerType.canConvert<int>() << registerType.canConvert<QModbusDataUnit::RegisterType>();
        QMessageBox::warning(this,
                             tr("Invalid register type selected!"),
                             tr("Invalid register type selected!"));
        return;
    }

    m_timer.start();

    QModbusDataUnit dataUnit(registerType.value<QModbusDataUnit::RegisterType>(), m_ui->spinBoxRegister->value(), m_ui->spinBoxCount->value());
    qDebug() << m_ui->spinBoxSlave->value() << dataUnit.registerType() << dataUnit.startAddress() << dataUnit.valueCount();
    if (m_reply = std::unique_ptr<QModbusReply>(m_modbus->sendReadRequest(std::move(dataUnit), m_ui->spinBoxSlave->value())))
    {
        updateRequestFields();

        if (m_reply->isFinished())
            replyFinished();
        else
        {
            m_ui->labelRequestStatus->setText(tr("Pending..."));
            if (!m_ui->checkBoxAutorefresh->isChecked())
                m_model->setResult({});
            connect(m_reply.get(), &QModbusReply::finished, this, &MainWindow::replyFinished);
        }
    }
    else
    {
        m_ui->checkBoxAutorefresh->setChecked(false);
        m_model->setResult({});
        QMessageBox::warning(this,
                             tr("Request sending failed!"),
                             tr("Request sending failed:\n\n%0").arg(m_modbus->errorString()));
    }
}

void MainWindow::writePressed()
{
    QModbusDataUnit::RegisterType registerType{QModbusDataUnit::RegisterType::Invalid};

    if (const auto registerTypeData = m_ui->comboBoxType->currentData();
        registerTypeData.isValid() && registerTypeData.canConvert<QModbusDataUnit::RegisterType>())
        registerType = registerTypeData.value<QModbusDataUnit::RegisterType>();

    ChangeValuesDialog dialog{*m_modbus, m_ui->spinBoxSlave->value(), registerType, this};
    dialog.exec();
    qDebug() << "called";
}

void MainWindow::modbusErrorOccured(int error)
{
    const auto typedError = QModbusDevice::Error(error);
    qWarning() << typedError << m_modbus->errorString();

    statusBar()->showMessage(tr("Modbus client failed with %0: %1")
                                .arg(typedError)
                                .arg(m_modbus->errorString()),
                             5000);
}

void MainWindow::modbusStateChanged(int state)
{
    qDebug() << QModbusDevice::State(state);

    m_ui->labelConnectionStatus->setText(QMetaEnum::fromType<QModbusDevice::State>().valueToKey(state));

    if (state == QModbusDevice::ConnectedState)
        m_ui->pushButtonConnect->setText(tr("Disconnect"));
    else if (state == QModbusDevice::UnconnectedState)
    {
        m_ui->pushButtonConnect->setText(tr("Connect"));
        if (!m_reply)
            m_ui->labelRequestStatus->setText(tr("Idle"));
    }

    m_ui->lineEditServer->setEnabled(state == QModbusDevice::UnconnectedState);
    m_ui->spinBoxPort->setEnabled(state == QModbusDevice::UnconnectedState);
    m_ui->pushButtonConnect->setEnabled(state == QModbusDevice::ConnectedState || state == QModbusDevice::UnconnectedState);
    m_ui->pushButtonRequest->setEnabled(state == QModbusDevice::ConnectedState);
}

void MainWindow::replyFinished()
{
    Q_ASSERT(m_reply);

    if (!m_reply->isFinished())
    {
        qWarning() << "not yet finished?!";
        return;
    }

    const auto elapsed = m_timer.elapsed();

    if (const QModbusDevice::Error error = m_reply->error(); error == QModbusDevice::NoError)
    {
        const auto result = m_reply->result();

        if (result.isValid())
        {
            m_model->setResult(result);

            m_ui->labelRequestStatus->setText(tr("Succeeded!"));

            statusBar()->showMessage(tr("Showing %0 from %1 to %2 (%3 registers) (took %4ms)")
                                        .arg(toString(result.registerType()))
                                        .arg(result.startAddress())
                                        .arg(result.startAddress() + result.valueCount())
                                        .arg(result.valueCount())
                                        .arg(elapsed),
                                     5000);

            if (m_ui->checkBoxAutorefresh->isChecked())
                QTimer::singleShot(m_ui->spinBoxDelay->value(), this, &MainWindow::requestPressed);
        }
        else
        {
            qWarning() << "result is invalid!";

            m_model->setResult({});

            m_ui->checkBoxAutorefresh->setChecked(false);

            m_ui->labelRequestStatus->setText(tr("Failed!"));

            const auto msg = tr("Request finished without any indication for an error but still the result is not valid! (took %0ms)")
                    .arg(elapsed);

            statusBar()->showMessage(msg, 5000);

            QMessageBox::warning(this,
                                 tr("Request failed!"),
                                 msg);
        }
    }
    else
    {
        qWarning() << error << m_reply->errorString();

        if (const auto result = m_reply->result(); result.isValid())
        {
            qDebug() << "registerType =" << result.registerType();
            qDebug() << "startAddress =" << result.startAddress();
            qDebug() << "valueCount =" << result.valueCount();
        }

        m_model->setResult({});

        m_ui->checkBoxAutorefresh->setChecked(false);

        m_ui->labelRequestStatus->setText(tr("Failed!"));

        const auto msg = tr("Request failed with %0: %1 (took %2ms)")
                             .arg(error)
                             .arg(m_reply->errorString())
                             .arg(elapsed);

        statusBar()->showMessage(msg, 5000);

        if (m_ui->checkBoxShowMsgBoxes->isChecked())
            QMessageBox::warning(this,
                                 tr("Request failed!"),
                                 msg);
    }

    m_reply = nullptr;
    updateRequestFields();
}

void MainWindow::updateRequestFields()
{
    m_ui->spinBoxSlave->setEnabled(!m_reply);
    m_ui->comboBoxType->setEnabled(!m_reply);
    m_ui->spinBoxRegister->setEnabled(!m_reply);
    m_ui->spinBoxCount->setEnabled(!m_reply);
    m_ui->pushButtonRequest->setEnabled(m_modbus->state() == QModbusDevice::ConnectedState && !m_reply);
}

namespace {
QDebug operator<<(QDebug debug, QModbusDataUnit::RegisterType registerType)
{
    QDebugStateSaver saver(debug);

    debug.nospace() << "QModbusDataUnit::RegisterType(";

    switch (registerType)
    {
    case QModbusDataUnit::RegisterType::Invalid:          debug << "Invalid";          break;
    case QModbusDataUnit::RegisterType::DiscreteInputs:   debug << "DiscreteInputs";   break;
    case QModbusDataUnit::RegisterType::Coils:            debug << "Coils";            break;
    case QModbusDataUnit::RegisterType::InputRegisters:   debug << "InputRegisters";   break;
    case QModbusDataUnit::RegisterType::HoldingRegisters: debug << "HoldingRegisters"; break;
    default:                                              debug << int(registerType);  break;
    }

    return debug << ')';
}

QString toString(QModbusDataUnit::RegisterType registerType)
{
    switch (registerType)
    {
    case QModbusDataUnit::RegisterType::Invalid:          return MainWindow::tr("Invalid");
    case QModbusDataUnit::RegisterType::DiscreteInputs:   return MainWindow::tr("DiscreteInputs");
    case QModbusDataUnit::RegisterType::Coils:            return MainWindow::tr("Coils");
    case QModbusDataUnit::RegisterType::InputRegisters:   return MainWindow::tr("InputRegisters");
    case QModbusDataUnit::RegisterType::HoldingRegisters: return MainWindow::tr("HoldingRegisters");
    default:                                              return MainWindow::tr("Unknown RegisterType(%0)").arg(int(registerType));
    }
}
} // namespace
