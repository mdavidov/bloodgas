#include "CalibrationManager.h"
#include "DatabaseManager.h"

#include <QDebug>
#include <QTimer>
#include <QRandomGenerator>
#include <QJsonDocument>
#include <QJsonObject>

CalibrationManager::CalibrationManager(DatabaseManager *dbManager, QObject *parent)
    : QObject(parent)
    , m_dbManager(dbManager)
    , m_isCalibrating(false)
    , m_isCalibrated(true)
    , m_calibrationProgress(0)
    , m_currentStepIndex(0)
    , m_stepRetryCount(0)
    , m_calibrationTimer(new QTimer(this))
{
    // Setup calibration timer
    m_calibrationTimer->setSingleShot(true);
    connect(m_calibrationTimer, &QTimer::timeout, this, &CalibrationManager::onCalibrationTimeout);
    
    // Initialize calibration steps
    initializeCalibrationSteps();
    
    // Load previous calibration state
    loadCalibrationState();
}

void CalibrationManager::initializeCalibrationSteps()
{
    m_calibrationSteps.clear();
    
    // Define calibration steps for blood gas analyzer
    CalibrationStep step1;
    step1.name = "System Check";
    step1.description = "Performing system diagnostics and sensor check";
    step1.duration = 2000; // ms
    step1.expectedValues = QVariantMap{{"temperature", 37.0}, {"pressure", 760.0}};
    step1.tolerances = QVariantMap{{"temperature", 1.0}, {"pressure", 5.0}};
    m_calibrationSteps.append(step1);
    
    CalibrationStep step2;
    step2.name = "pH Calibration";
    step2.description = "Calibrating pH electrode with buffer solutions";
    step2.duration = 2000; // ms
    step2.expectedValues = QVariantMap{{"pH_buffer1", 7.40}, {"pH_buffer2", 6.84}};
    step2.tolerances = QVariantMap{{"pH_buffer1", 0.02}, {"pH_buffer2", 0.02}};
    m_calibrationSteps.append(step2);
    
    CalibrationStep step3;
    step3.name = "Gas Calibration";
    step3.description = "Calibrating pO2 and pCO2 sensors";
    step3.duration = 2000; // ms
    step3.expectedValues = QVariantMap{{"pO2_cal", 150.0}, {"pCO2_cal", 40.0}};
    step3.tolerances = QVariantMap{{"pO2_cal", 5.0}, {"pCO2_cal", 2.0}};
    m_calibrationSteps.append(step3);
    
    CalibrationStep step4;
    step4.name = "Electrolyte Calibration";
    step4.description = "Calibrating ion-selective electrodes";
    step4.duration = 2000; // ms
    step4.expectedValues = QVariantMap{{"Na_cal", 140.0}, {"K_cal", 4.0}, {"Cl_cal", 100.0}};
    step4.tolerances = QVariantMap{{"Na_cal", 2.0}, {"K_cal", 0.2}, {"Cl_cal", 2.0}};
    m_calibrationSteps.append(step4);
    
    CalibrationStep step5;
    step5.name = "Quality Control";
    step5.description = "Running quality control samples";
    step5.duration = 2000; // ms
    step5.expectedValues = QVariantMap{{"qc_passed", true}};
    step5.tolerances = QVariantMap{};
    m_calibrationSteps.append(step5);
}

void CalibrationManager::startCalibration(const QString& calibrationType)
{
    if (m_isCalibrating) {
        qWarning() << "Calibration already in progress";
        return;
    }
    
    m_calibrationType = calibrationType;
    m_isCalibrating = true;
    m_calibrationProgress = 0;
    m_currentStepIndex = 0;
    m_stepRetryCount = 0;
    m_calibrationStartTime = QDateTime::currentDateTime();
    
    emit calibrationStatusChanged();
    emit calibrationProgressChanged();
    
    // Start first calibration step
    performCalibrationStep();
    
    qDebug() << "Started calibration:" << calibrationType;
}

void CalibrationManager::cancelCalibration()
{
    if (!m_isCalibrating) {
        return;
    }
    m_calibrationTimer->stop();
    resetCalibration();
    qDebug() << "Calibration cancelled";
}

void CalibrationManager::acceptCalibrationStep()
{
    if (!m_isCalibrating) {
        return;
    }
    m_calibrationTimer->stop();
    
    // Move to next step
    m_currentStepIndex++;
    m_stepRetryCount = 0;
    
    if (m_currentStepIndex >= m_calibrationSteps.size()) {
        // Calibration completed successfully
        completeCalibration(true);
    } else {
        // Continue with next step
        performCalibrationStep();
    }
}

void CalibrationManager::retryCalibrationStep()
{
    if (!m_isCalibrating) {
        return;
    }
    
    m_stepRetryCount++;
    if (m_stepRetryCount >= MAX_RETRY_COUNT) {
        // Too many retries, fail calibration
        completeCalibration(false);
        emit calibrationFailed("Too many retries for step: " + m_calibrationStep);
        return;
    }
    
    // Retry current step
    performCalibrationStep();
}

void CalibrationManager::performCalibrationStep()
{
    if (m_currentStepIndex >= m_calibrationSteps.size()) {
        completeCalibration(true);
        return;
    }

    if (m_calibrationType == "ph_only") {
        // Skip non-pH steps
        while (m_currentStepIndex < m_calibrationSteps.size() &&
               m_calibrationSteps[m_currentStepIndex].name != "pH Calibration") {
            m_currentStepIndex++;
        }
        if (m_currentStepIndex >= m_calibrationSteps.size()) {
            completeCalibration(true);
            return;
        }
    }
    const CalibrationStep& step = m_calibrationSteps[m_currentStepIndex];
    m_calibrationStep = step.name;
    m_calibrationProgress = (m_currentStepIndex * 100) / m_calibrationSteps.size();
    
    emit calibrationStepChanged();
    emit calibrationProgressChanged();
    
    // Start step timer
    m_calibrationTimer->start(step.duration);
    
    qDebug() << "Performing calibration step:" << step.name;
}

void CalibrationManager::onCalibrationTimeout()
{
    if (!m_isCalibrating || m_currentStepIndex >= m_calibrationSteps.size()) {
        return;
    }
    // Simulate calibration step completion
    simulateCalibrationStep();
}

void CalibrationManager::simulateCalibrationStep()
{
    const CalibrationStep &step = m_calibrationSteps[m_currentStepIndex];
    
    // Simulate success/failure (90% success rate for demo)
    bool success = QRandomGenerator::global()->bounded(100) < 90;
    
    emit calibrationStepCompleted(step.name, success);
    
    if (success) {
        // Auto-proceed to next step
        acceptCalibrationStep();
    } else {
        // Step failed, require user intervention
        qWarning() << "Calibration step failed:" << step.name;
        emit calibrationFailed("Calibration step failed: " + step.name + ". Retry or cancel calibration.");
    }
}

void CalibrationManager::completeCalibration(bool success)
{
    m_calibrationTimer->stop();
    
    if (success) {
        m_isCalibrated = true;
        m_lastCalibrationTime = QDateTime::currentDateTime();
        m_calibrationProgress = 100;
        
        // Save calibration data
        QVariantMap calibrationData;
        calibrationData["type"] = m_calibrationType;
        calibrationData["timestamp"] = m_lastCalibrationTime;
        calibrationData["duration"] = m_calibrationStartTime.msecsTo(m_lastCalibrationTime);
        calibrationData["steps_completed"] = m_calibrationSteps.size();
        calibrationData["success"] = true;
        
        saveCalibrationData(calibrationData);
        
        emit calibratedStatusChanged();
        emit lastCalibrationTimeChanged();
        emit calibrationProgressChanged();
        
        qDebug() << "Calibration completed successfully";
    } else {
        qDebug() << "Calibration failed";
    }
    
    resetCalibration();
    emit calibrationCompleted(success);
}

void CalibrationManager::resetCalibration()
{
    m_isCalibrating = false;
    m_calibrationStep.clear();
    m_calibrationProgress = 0;
    m_currentStepIndex = 0;
    m_stepRetryCount = 0;
    
    emit calibrationStatusChanged();
    emit calibrationStepChanged();
    emit calibrationProgressChanged();
}

void CalibrationManager::loadCalibrationState()
{
    if (!m_dbManager) {
        return;
    }
    
    QVariantMap lastCalibration = m_dbManager->getLatestCalibrationData();
    if (!lastCalibration.isEmpty()) {
        m_lastCalibrationTime = lastCalibration.value("timestamp").toDateTime();
        m_isCalibrated = isCalibrationRequired() ? false : true;
        
        emit calibratedStatusChanged();
        emit lastCalibrationTimeChanged();
        
        qDebug() << "Loaded calibration state. Last calibration:" << m_lastCalibrationTime;
    }
}

void CalibrationManager::saveCalibrationData(const QVariantMap &data)
{
    if (m_dbManager) {
        m_dbManager->saveCalibrationData(data);
    }
}

QVariantList CalibrationManager::getCalibrationHistory()
{
    if (m_dbManager) {
        return m_dbManager->getCalibrationHistory();
    }
    return QVariantList();
}

QVariantMap CalibrationManager::getLastCalibrationData()
{
    if (m_dbManager) {
        return m_dbManager->getLatestCalibrationData();
    }
    return QVariantMap();
}

bool CalibrationManager::isCalibrationRequired()
{
    if (!m_lastCalibrationTime.isValid()) {
        return true;
    }
    
    QDateTime now = QDateTime::currentDateTime();
    return m_lastCalibrationTime.daysTo(now) >= CALIBRATION_VALIDITY_DAYS;
}

int CalibrationManager::getCalibrationValidityDays()
{
    if (!m_lastCalibrationTime.isValid()) {
        return 0;
    }
    
    QDateTime now = QDateTime::currentDateTime();
    int daysPassed = m_lastCalibrationTime.daysTo(now);
    return qMax(0, CALIBRATION_VALIDITY_DAYS - daysPassed);
}