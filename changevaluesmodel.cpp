#include "changevaluesmodel.h"

// Qt includes
#include <QDebug>

ChangeValuesModel::ChangeValuesModel(QObject *parent) :
    QAbstractTableModel{parent}
{
    m_registers.resize(m_count);
    qDebug() << "size =" << m_registers.size();
}

ChangeValuesModel::~ChangeValuesModel()
{
    qDebug() << "called";
}

QModelIndex ChangeValuesModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid());
    return createIndex(row, column);
}

QModelIndex ChangeValuesModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)
    return {};
}

int ChangeValuesModel::rowCount(const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid());
    return m_count;
}

int ChangeValuesModel::columnCount(const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid());
    return 1;
}

QVariant ChangeValuesModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(index.isValid());

    switch (role)
    {
    case Qt::DisplayRole:
        return QString::number(m_registers.at(m_firstRegister + index.row()));
    case Qt::EditRole:
        return m_registers.at(m_firstRegister + index.row());
    }

    return {};
}

QVariant ChangeValuesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (orientation)
    {
    case Qt::Horizontal:
        Q_ASSERT(section == 0);

        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            switch (section)
            {
            case 0: return tr("Value");
            }
            __builtin_unreachable();
            break;
        }

        break;

    case Qt::Vertical:

        switch (role)
        {
        case Qt::DisplayRole: return QString::number(m_firstRegister + section);
        case Qt::EditRole:    return m_firstRegister + section;
        }

    default:
        __builtin_unreachable();
    }

    return {};
}

Qt::ItemFlags ChangeValuesModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool ChangeValuesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    qDebug() << "setData" << index << value << Qt::ItemDataRole(role);

    if (!value.canConvert<uint16_t>())
    {
        qCritical() << "wrong type";
        return false;
    }

    switch (role)
    {
    case Qt::EditRole:
        m_registers[index.row() + m_firstRegister] = value.value<uint16_t>();
        return true;
    }

    return false;
}

QVector<quint16> ChangeValuesModel::values() const
{
    return m_registers.mid(m_firstRegister, m_count);
}

void ChangeValuesModel::setFirstRegister(uint16_t firstRegister)
{
    beginResetModel();
    m_firstRegister = firstRegister;
    if (m_registers.size() < m_firstRegister + m_count)
        m_registers.resize(m_firstRegister + m_count);
    endResetModel();
}

void ChangeValuesModel::setCount(uint16_t count)
{
    beginResetModel();
    m_count = count;
    if (m_registers.size() < m_firstRegister + m_count)
        m_registers.resize(m_firstRegister + m_count);
    endResetModel();
}
