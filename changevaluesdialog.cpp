#include "changevaluesdialog.h"
#include "ui_changevaluesdialog.h"

// Qt includes
#include <QDebug>
#include <QModbusTcpClient>

ChangeValuesDialog::ChangeValuesDialog(QModbusTcpClient &modbus, QWidget *parent) :
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


    {
        const auto addItem = [&](const auto &text, const auto &value){
            m_ui->comboBoxType->addItem(text, QVariant::fromValue<QModbusDataUnit::RegisterType>(value));
        };
        addItem(tr("Discrete Inputs"),   QModbusDataUnit::DiscreteInputs);
        addItem(tr("Coils"),             QModbusDataUnit::Coils);
        addItem(tr("Input Registers"),   QModbusDataUnit::InputRegisters);
        addItem(tr("Holding Registers"), QModbusDataUnit::HoldingRegisters);
    }
}

ChangeValuesDialog::~ChangeValuesDialog()
{
    qDebug() << "called";
}

void ChangeValuesDialog::sendRequest()
{
    accept();
}
