#include "modbustablemodel.h"

namespace {
enum {
    ColumnDecimal,
    ColumnHex,
    ColumnBinary,
    ColumnAscii,
    ColumnCount
};
}

void ModbusTableModel::setResult(const QModbusDataUnit &result)
{
    beginResetModel();
    m_result = result;
    endResetModel();
}

QModelIndex ModbusTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid());
    return createIndex(row, column);
}

QModelIndex ModbusTableModel::parent(const QModelIndex &child) const
{
    return {};
}

int ModbusTableModel::rowCount(const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid());
    return m_result.isValid() ? m_result.valueCount() : 0;
}

int ModbusTableModel::columnCount(const QModelIndex &parent) const
{
    Q_ASSERT(!parent.isValid());
    return ColumnCount;
}

QVariant ModbusTableModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(index.isValid());
    Q_ASSERT(m_result.isValid());
    Q_ASSERT(index.row() >= 0);
    Q_ASSERT(index.row() < m_result.valueCount());
    Q_ASSERT(index.column() >= 0);
    Q_ASSERT(index.column() < ColumnCount);

    const auto &values = m_result.values();
    Q_ASSERT(index.row() < values.size());

    const auto &value = values[index.row()];

    switch (role)
    {
    case Qt::DisplayRole:
        switch (index.column())
        {
        case ColumnDecimal: return QString::number(value);
        case ColumnHex:     return QString::number(value, 16).rightJustified(4, '0').insert(2, ' ');
        case ColumnBinary:  return QString::number(value, 2) .rightJustified(16, '0').insert(8, ' ');
        case ColumnAscii:   return QString{QChar{(value&0xFF00)>>8}} + QChar{value&0x00FF};
        }
        __builtin_unreachable();
        break;
    case Qt::EditRole:
        switch (index.column())
        {
        case ColumnDecimal:
        case ColumnHex:
        case ColumnBinary:
        case ColumnAscii:
            return value;
        }
        __builtin_unreachable();
        break;
    }

    return {};
}

QVariant ModbusTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (orientation)
    {
    case Qt::Horizontal:
        Q_ASSERT(section >= 0);
        Q_ASSERT(section < ColumnCount);

        switch (role)
        {
        case Qt::DisplayRole:
        case Qt::EditRole:
            switch (section)
            {
            case ColumnDecimal: return tr("Decimal");
            case ColumnHex:     return tr("Hex");
            case ColumnBinary:  return tr("Binary");
            case ColumnAscii:   return tr("Ascii");
            }
            __builtin_unreachable();
            break;
        }

        break;

    case Qt::Vertical:
        Q_ASSERT(m_result.isValid());
        Q_ASSERT(section >= 0);
        Q_ASSERT(section < m_result.valueCount());

        switch (role)
        {
        case Qt::DisplayRole: return QString::number(m_result.startAddress() + section);
        case Qt::EditRole:    return m_result.startAddress() + section;
        }

        break;

    default:
        __builtin_unreachable();
    }

    return {};
}
