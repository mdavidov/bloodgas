#ifndef HISTORICALDATAMODEL_H
#define HISTORICALDATAMODEL_H

#include <QAbstractListModel>
#include <QVariantMap>
#include <QDateTime>

class DatabaseManager;

class HistoricalDataModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    
public:
    enum Roles {
        TimestampRole = Qt::UserRole + 1,
        OperatorRole,
        SampleIdRole,
        PatientIdRole,
        PhRole,
        PCO2Role,
        PO2Role,
        HCO3Role,
        SO2Role,
        BERole,
        NaRole,
        KRole,
        ClRole,
        CaRole,
        GlucoseRole,
        LactateRole,
        TemperatureRole,
        FullDataRole
    };
    
    explicit HistoricalDataModel(DatabaseManager* dbManager, QObject* parent = nullptr);
    
    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    int count() const { return m_data.size(); }
    
public slots:
    Q_INVOKABLE void loadData();
    Q_INVOKABLE void addResult(const QVariantMap& result);
    Q_INVOKABLE void removeResult(int index);
    Q_INVOKABLE void clearAll();
    Q_INVOKABLE QVariantMap getResult(int index) const;
    Q_INVOKABLE void filterByDate(const QDateTime& startDate, const QDateTime& endDate);
    Q_INVOKABLE void filterByOperator(const QString& operatorName);
    Q_INVOKABLE void filterByPatient(const QString& patientId);
    Q_INVOKABLE void clearFilters();
    Q_INVOKABLE void exportToCSV(const QString& filePath);

signals:
    void countChanged();
    void dataLoaded();
    void resultAdded(const QVariantMap& result);
    void resultRemoved(int index);
    
private:
    void applyFilters();
    QVariantMap createResultMap(const QVariantMap& data) const;
    void _endResetModel();

    DatabaseManager* m_dbManager;
    QList<QVariantMap> m_data;
    QList<QVariantMap> m_filteredData;
    
    // Filter criteria
    QString m_operatorFilter;
    QString m_patientFilter;
    QDateTime m_startDateFilter;
    QDateTime m_endDateFilter;
    bool m_hasFilters;
};

#endif // HISTORICALDATAMODEL_H