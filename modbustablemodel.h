#pragma once

// Qt includes
#include <QAbstractTableModel>
#include <QModbusDataUnit>

class ModbusTableModel : public QAbstractTableModel
{
public:
    using QAbstractTableModel::QAbstractTableModel;

    void setResult(const QModbusDataUnit &result);

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QModbusDataUnit m_result;
};
