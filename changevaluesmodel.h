#pragma once

// Qt includes
#include <QAbstractTableModel>
#include <QVector>

class ChangeValuesModel : public QAbstractTableModel
{
public:
    explicit ChangeValuesModel(QObject *parent = nullptr);
    ~ChangeValuesModel() override;

    uint16_t firstRegister() const { return m_firstRegister; }
    uint16_t count() const { return m_count; }

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    QVector<quint16> values() const;

public slots:
    void setFirstRegister(uint16_t firstRegister);
    void setCount(uint16_t count);

private:
    uint16_t m_firstRegister{};
    uint16_t m_count{100};

    QVector<uint16_t> m_registers;
};
