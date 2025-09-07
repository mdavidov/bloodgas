#include "HistoricalDataModel.h"
#include "DatabaseManager.h"

#include <thread>
#include <chrono>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStandardPaths>
using namespace std::chrono_literals;

HistoricalDataModel::HistoricalDataModel(DatabaseManager *dbManager, QObject *parent)
    : QAbstractListModel(parent)
    , m_dbManager(dbManager)
    , m_hasFilters(false)
{
}

int HistoricalDataModel::rowCount(const QModelIndex&) const
{
    return m_hasFilters ? m_filteredData.size() : m_data.size();
}

QVariant HistoricalDataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount())
        return QVariant();

    const QList<QVariantMap>& dataList = m_hasFilters ? m_filteredData : m_data;
    const QVariantMap& item = dataList.at(index.row());

    switch (role) {
    case TimestampRole:
        return item.value("timestamp");
    case OperatorRole:
        return item.value("operator");
    case SampleIdRole:
        return item.value("sampleId");
    case PatientIdRole:
        return item.value("patientId");
    case PhRole:
        return item.value("pH");
    case PCO2Role:
        return item.value("pCO2");
    case PO2Role:
        return item.value("pO2");
    case HCO3Role:
        return item.value("HCO3");
    case SO2Role:
        return item.value("SO2");
    case BERole:
        return item.value("BE");
    case NaRole:
        return item.value("Na");
    case KRole:
        return item.value("K");
    case ClRole:
        return item.value("Cl");
    case CaRole:
        return item.value("Ca");
    case GlucoseRole:
        return item.value("Glucose");
    case LactateRole:
        return item.value("Lactate");
    case TemperatureRole:
        return item.value("temperature");
    case FullDataRole:
        return item;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> HistoricalDataModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TimestampRole] = "timestamp";
    roles[OperatorRole] = "operator";
    roles[SampleIdRole] = "sampleId";
    roles[PatientIdRole] = "patientId";
    roles[PhRole] = "pH";
    roles[PCO2Role] = "pCO2";
    roles[PO2Role] = "pO2";
    roles[HCO3Role] = "HCO3";
    roles[SO2Role] = "SO2";
    roles[BERole] = "BE";
    roles[NaRole] = "Na";
    roles[KRole] = "K";
    roles[ClRole] = "Cl";
    roles[CaRole] = "Ca";
    roles[GlucoseRole] = "Glucose";
    roles[LactateRole] = "Lactate";
    roles[TemperatureRole] = "temperature";
    roles[FullDataRole] = "fullData";
    return roles;
}

void HistoricalDataModel::loadData()
{
    if (!m_dbManager) {
        qWarning() << "No DB manager available";
        return;
    }
    if (m_dbManager->getAllResults().empty()) {
        qDebug() << "No results from DB manager found";
        return;
    }

    beginResetModel();
    m_data.clear();

    QVariantList results = m_dbManager->getAllResults();
    for (const auto& variant : results) {
        QVariantMap result = variant.toMap();
        m_data.append(result);
    }

    m_hasFilters = false;
    _endResetModel();

    emit dataLoaded();
    qDebug() << "Loaded" << m_data.size() << "historical results";
}

void HistoricalDataModel::addResult(const QVariantMap &result)
{
    if (!m_dbManager) {
        qWarning() << "No database manager available";
        return;
    }
    
    // Save to database first
    if (!m_dbManager->saveResult(result)) {
        qWarning() << "Failed to save result to database";
        return;
    }
    
    // Add to model
    QVariantMap processedResult = createResultMap(result);
    
    beginInsertRows(QModelIndex(), 0, 0);
    m_data.prepend(processedResult);
    endInsertRows();
    
    // Update filtered data if filters are active
    if (m_hasFilters) {
        applyFilters();
    }
    
    emit countChanged();
    emit resultAdded(processedResult);
    
    qDebug() << "Added result to historical data:" << processedResult.value("sampleId");
}

void HistoricalDataModel::removeResult(int index)
{
    if (index < 0 || index >= rowCount())
        return;
    
    const QList<QVariantMap> &dataList = m_hasFilters ? m_filteredData : m_data;
    const QVariantMap &item = dataList.at(index);
    
    // Remove from database
    if (m_dbManager && !m_dbManager->removeResult(item.value("id").toInt())) {
        qWarning() << "Failed to remove result from database";
        return;
    }
    
    beginRemoveRows(QModelIndex(), index, index);
    
    if (m_hasFilters) {
        // Find and remove from original data
        QString sampleId = item.value("sampleId").toString();
        for (int i = 0; i < m_data.size(); ++i) {
            if (m_data.at(i).value("sampleId").toString() == sampleId) {
                m_data.removeAt(i);
                break;
            }
        }
        m_filteredData.removeAt(index);
    } else {
        m_data.removeAt(index);
    }
    
    endRemoveRows();
    
    emit countChanged();
    emit resultRemoved(index);
}

void HistoricalDataModel::clearAll()
{
    if (!m_dbManager) {
        qWarning() << "No database manager available";
        return;
    }
    
    beginResetModel();
    
    // Clear database
    m_dbManager->clearAllResults();
    
    // Clear model data
    m_data.clear();
    m_filteredData.clear();
    m_hasFilters = false;
    
    _endResetModel();
}

QVariantMap HistoricalDataModel::getResult(int index) const
{
    if (index < 0 || index >= rowCount())
        return QVariantMap();
    
    const QList<QVariantMap> &dataList = m_hasFilters ? m_filteredData : m_data;
    return dataList.at(index);
}

void HistoricalDataModel::filterByDate(const QDateTime &startDate, const QDateTime &endDate)
{
    m_startDateFilter = startDate;
    m_endDateFilter = endDate;
    applyFilters();
}

void HistoricalDataModel::filterByOperator(const QString &operatorName)
{
    m_operatorFilter = operatorName;
    applyFilters();
}

void HistoricalDataModel::filterByPatient(const QString &patientId)
{
    m_patientFilter = patientId;
    applyFilters();
}

void HistoricalDataModel::clearFilters()
{
    beginResetModel();
    m_operatorFilter.clear();
    m_patientFilter.clear();
    m_startDateFilter = QDateTime();
    m_endDateFilter = QDateTime();
    m_hasFilters = false;
    m_filteredData.clear();
    _endResetModel();
}

void HistoricalDataModel::applyFilters()
{
    beginResetModel();
    
    m_filteredData.clear();
    m_hasFilters = !m_operatorFilter.isEmpty() || !m_patientFilter.isEmpty() ||
                   m_startDateFilter.isValid() || m_endDateFilter.isValid();
    
    if (!m_hasFilters) {
        _endResetModel();
        return;
    }
    
    for (const QVariantMap &item : m_data) {
        bool include = true;
        
        // Filter by operator
        if (!m_operatorFilter.isEmpty() && 
            !item.value("operator").toString().contains(m_operatorFilter, Qt::CaseInsensitive)) {
            include = false;
        }
        
        // Filter by patient
        if (include && !m_patientFilter.isEmpty() &&
            !item.value("patientId").toString().contains(m_patientFilter, Qt::CaseInsensitive)) {
            include = false;
        }
        
        // Filter by date range
        if (include && (m_startDateFilter.isValid() || m_endDateFilter.isValid())) {
            QDateTime itemDate = QDateTime::fromString(item.value("timestamp").toString(), Qt::ISODate);
            if (m_startDateFilter.isValid() && itemDate < m_startDateFilter) {
                include = false;
            }
            if (include && m_endDateFilter.isValid() && itemDate > m_endDateFilter) {
                include = false;
            }
        }
        
        if (include) {
            m_filteredData.append(item);
        }
    }

    _endResetModel();
    qDebug() << "Applied filters, showing" << m_filteredData.size() << "of" << m_data.size() << "results";
}

void HistoricalDataModel::exportToCSV(const QString &filePath)
{
    QString actualPath = filePath;
    if (actualPath.isEmpty()) {
        QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        actualPath = QDir(documentsPath).filePath("blood_gas_results.csv");
    }
    
    QFile file(actualPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Could not open file for writing:" << actualPath;
        return;
    }
    
    QTextStream stream(&file);
    
    // Write header
    stream << "Timestamp,Operator,Sample ID,Patient ID,pH,pCO2,pO2,HCO3,SO2,BE,Na,K,Cl,Ca,Glucose,Lactate,Temperature\n";
    
    // Write data
    const QList<QVariantMap> &dataList = m_hasFilters ? m_filteredData : m_data;
    for (const QVariantMap &item : dataList) {
        stream << item.value("timestamp").toString() << ","
               << item.value("operator").toString() << ","
               << item.value("sampleId").toString() << ","
               << item.value("patientId").toString() << ","
               << item.value("pH").toString() << ","
               << item.value("pCO2").toString() << ","
               << item.value("pO2").toString() << ","
               << item.value("HCO3").toString() << ","
               << item.value("SO2").toString() << ","
               << item.value("BE").toString() << ","
               << item.value("Na").toString() << ","
               << item.value("K").toString() << ","
               << item.value("Cl").toString() << ","
               << item.value("Ca").toString() << ","
               << item.value("Glucose").toString() << ","
               << item.value("Lactate").toString() << ","
               << item.value("temperature").toString() << "\n";
    }
    
    file.close();
    qDebug() << "Exported" << dataList.size() << "results to" << actualPath;
}

QVariantMap HistoricalDataModel::createResultMap(const QVariantMap &data) const
{
    QVariantMap result = data;
    
    // Ensure all required fields are present with default values
    if (!result.contains("timestamp")) {
        result["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    }
    if (!result.contains("operator")) {
        result["operator"] = "Unknown";
    }
    if (!result.contains("sampleId")) {
        result["sampleId"] = "AUTO_" + QString::number(QDateTime::currentSecsSinceEpoch());
    }
    if (!result.contains("patientId")) {
        result["patientId"] = "";
    }
    
    return result;
}
void HistoricalDataModel::_endResetModel()
{
    endResetModel();
    std::this_thread::sleep_for(100ms);
    emit countChanged();
}
