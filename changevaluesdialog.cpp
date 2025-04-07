#include "changevaluesdialog.h"
#include "ui_changevaluesdialog.h"

// system includes
#include <utility>

// Qt includes
#include <QDebug>
#include <QModbusClient>
#include <QMessageBox>

ChangeValuesDialog::ChangeValuesDialog(QModbusClient &modbus, int serverAddress, QModbusDataUnit::RegisterType registerType, QWidget *parent) :
    QDialog{parent},
    m_ui{std::make_unique<Ui::ChangeValuesDialog>()},
    m_modbus{modbus}
{
    m_ui->setupUi(this);

    qDebug() << "called";

    m_ui->tableView->setModel(&m_model);

    m_ui->spinBoxFirstRegister->setValue(m_model.firstRegister());
    m_ui->spinBoxCount->setValue(m_model.count());

    connect(m_ui->spinBoxFirstRegister, &QSpinBox::valueChanged, &m_model, &ChangeValuesModel::setFirstRegister);
    connect(m_ui->spinBoxCount,         &QSpinBox::valueChanged, &m_model, &ChangeValuesModel::setCount);

    connect(m_ui->buttonBox, &QDialogButtonBox::accepted, this, &ChangeValuesDialog::sendRequest);

    m_ui->spinBoxSlave->setValue(serverAddress);

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
            QVariant::fromValue<QModbusDataUnit::RegisterType>(registerType)
            )
        );
}

ChangeValuesDialog::~ChangeValuesDialog()
{
    qDebug() << "called";
}

void ChangeValuesDialog::sendRequest()
{
    const auto registerType = m_ui->comboBoxType->currentData();
    if (!registerType.isValid() || !registerType.canConvert<QModbusDataUnit::RegisterType>())
    {
        qDebug() << registerType << registerType.typeName() << registerType.canConvert<int>() << registerType.canConvert<QModbusDataUnit::RegisterType>();
        QMessageBox::warning(this,
                             tr("Invalid register type selected!"),
                             tr("Invalid register type selected!"));
        return;
    }

    QModbusDataUnit dataUnit{registerType.value<QModbusDataUnit::RegisterType>(), m_ui->spinBoxFirstRegister->value(), m_model.values()};
    if (m_reply = std::unique_ptr<QModbusReply>(m_modbus.sendWriteRequest(std::move(dataUnit), m_ui->spinBoxSlave->value())))
    {
        if (m_reply->isFinished())
            replyFinished();
        else
        {
            m_ui->buttonBox->setEnabled(false);
            connect(m_reply.get(), &QModbusReply::finished, this, &ChangeValuesDialog::replyFinished);
        }
    }
    else
    {
        QMessageBox::warning(this,
                             tr("Request sending failed!"),
                             tr("Request sending failed:\n\n%0").arg(m_modbus.errorString()));
    }
}

void ChangeValuesDialog::replyFinished()
{
    Q_ASSERT(m_reply);

    if (!m_reply->isFinished())
    {
        qWarning() << "not yet finished?!";
        return;
    }

    m_ui->buttonBox->setEnabled(true);

    if (const QModbusDevice::Error error = m_reply->error(); error == QModbusDevice::NoError)
    {
        //accept();
    }
    else
    {
        QMessageBox::warning(this,
                             tr("Request failed!"),
                             tr("Request failed with %0: %1")
                                 .arg(error)
                                 .arg(m_reply->errorString()));
    }
}
