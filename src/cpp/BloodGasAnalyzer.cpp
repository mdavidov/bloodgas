#include "BloodGasAnalyzer.h"
#include "HistoricalDataModel.h"
#include "DatabaseManager.h"
#include "AuthenticationManager.h"
#include "CalibrationManager.h"
#include "HL7Manager.h"

#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QRandomGenerator>

BloodGasAnalyzer::BloodGasAnalyzer(QObject *parent)
    : QObject(parent)
    , m_historicalDataModel(nullptr)
    , m_databaseManager(nullptr)
    , m_authManager(nullptr)
    , m_calibrationManager(nullptr)
    , m_hl7Manager(nullptr)
    , m_analysisTimer(new QTimer(this))
    , m_isAnalyzing(false)
    , m_isCalibrated(false)
{
    initializeComponents();
    
    // Setup analysis timer
    m_analysisTimer->setSingleShot(true);
    connect(m_analysisTimer, &QTimer::timeout, this, &BloodGasAnalyzer::onAnalysisTimeout);
}

BloodGasAnalyzer::~BloodGasAnalyzer()
{
    // Cleanup is handled by Qt's parent-child relationship
}

void BloodGasAnalyzer::initializeComponents()
{
    // Create database manager first
    m_databaseManager = new DatabaseManager(this);
    
    // Create historical data model
    m_historicalDataModel = new HistoricalDataModel(m_databaseManager, this);
    
    // Create authentication manager
    m_authManager = new AuthenticationManager(m_databaseManager, this);
    connect(m_authManager, &AuthenticationManager::userLoggedIn, 
            this, &BloodGasAnalyzer::onUserLoggedIn);
    connect(m_authManager, &AuthenticationManager::userLoggedOut, 
            this, &BloodGasAnalyzer::onUserLoggedOut);
    
    // Create calibration manager
    m_calibrationManager = new CalibrationManager(m_databaseManager, this);
    connect(m_calibrationManager, &CalibrationManager::calibrationCompleted,
            this, &BloodGasAnalyzer::onCalibrationCompleted);
    
    // Create HL7 manager
    m_hl7Manager = new HL7Manager(this);
    
    // Initialize database
    if (!m_databaseManager->initializeDatabase()) {
        qWarning() << "Failed to initialize database";
    }
    
    // Load historical data
    m_historicalDataModel->loadData();
}

void BloodGasAnalyzer::startAnalysis(const QVariantMap &sampleData)
{
    if (m_isAnalyzing) {
        qWarning() << "Analysis already in progress";
        return;
    }
    
    if (m_currentUser.isEmpty()) {
        emit analysisError("No user logged in");
        return;
    }
    
    if (!m_isCalibrated) {
        emit analysisError("Device not calibrated");
        return;
    }
    
    m_currentSampleData = sampleData;
    m_isAnalyzing = true;
    emit isAnalyzingChanged(true);
    
    // Simulate analysis time (3-5 seconds)
    int analysisTime = 3000 + QRandomGenerator::global()->bounded(2000);
    m_analysisTimer->start(analysisTime);
    
    qDebug() << "Started analysis for sample:" << sampleData;
}

void BloodGasAnalyzer::stopAnalysis()
{
    if (!m_isAnalyzing)
        return;
        
    m_analysisTimer->stop();
    m_isAnalyzing = false;
    emit isAnalyzingChanged(false);
    
    qDebug() << "Analysis stopped by user";
}

void BloodGasAnalyzer::onAnalysisTimeout()
{
    if (!m_isAnalyzing)
        return;
    
    // Simulate analysis results
    QVariantMap results = simulateAnalysis(m_currentSampleData);
    m_lastResults = results;
    
    // Add to historical data
    m_historicalDataModel->addResult(results);
    
    // Send to HL7 if configured
    m_hl7Manager->sendResults(results);
    
    m_isAnalyzing = false;
    emit isAnalyzingChanged(false);
    emit analysisCompleted(results);
    
    qDebug() << "Analysis completed with results:" << results;
}

QVariantMap BloodGasAnalyzer::simulateAnalysis(const QVariantMap &sampleData)
{
    QVariantMap results;
    
    // Basic blood gas parameters with realistic ranges
    results["pH"] = 7.35 + (QRandomGenerator::global()->bounded(100) / 1000.0);
    results["pCO2"] = 35.0 + QRandomGenerator::global()->bounded(15);
    results["pO2"] = 80.0 + QRandomGenerator::global()->bounded(40);
    results["HCO3"] = 22.0 + QRandomGenerator::global()->bounded(6);
    results["SO2"] = 95.0 + QRandomGenerator::global()->bounded(5);
    results["BE"] = -2.0 + QRandomGenerator::global()->bounded(8);
    
    // Electrolytes
    results["Na"] = 135.0 + QRandomGenerator::global()->bounded(10);
    results["K"] = 3.5 + (QRandomGenerator::global()->bounded(20) / 10.0);
    results["Cl"] = 95.0 + QRandomGenerator::global()->bounded(15);
    results["Ca"] = 2.2 + (QRandomGenerator::global()->bounded(6) / 10.0);
    
    // Metabolites
    results["Glucose"] = 70.0 + QRandomGenerator::global()->bounded(50);
    results["Lactate"] = 0.5 + (QRandomGenerator::global()->bounded(30) / 10.0);
    
    // Add metadata
    results["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    results["operator"] = m_currentUser;
    results["sampleId"] = sampleData.value("sampleId", "AUTO_" + QString::number(QDateTime::currentSecsSinceEpoch()));
    results["patientId"] = sampleData.value("patientId", "");
    results["temperature"] = sampleData.value("temperature", 37.0);
    
    return results;
}

void BloodGasAnalyzer::exportResults(const QString &format)
{
    if (m_lastResults.isEmpty()) {
        qWarning() << "No results to export";
        return;
    }
    
    // TODO: Implement export functionality
    qDebug() << "Export results in format:" << format;
}

QVariantMap BloodGasAnalyzer::getLastResults() const
{
    return m_lastResults;
}

void BloodGasAnalyzer::onUserLoggedIn(const QString &username)
{
    m_currentUser = username;
    emit currentUserChanged();
    qDebug() << "User logged in:" << username;
}

void BloodGasAnalyzer::onUserLoggedOut()
{
    m_currentUser.clear();
    emit currentUserChanged();
    qDebug() << "User logged out";
}

void BloodGasAnalyzer::onCalibrationCompleted(bool success)
{
    m_isCalibrated = success;
    emit isCalibratedChanged();
    qDebug() << "Calibration completed:" << (success ? "SUCCESS" : "FAILED");
}