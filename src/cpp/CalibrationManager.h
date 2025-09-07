#ifndef CALIBRATIONMANAGER_H
#define CALIBRATIONMANAGER_H

#include <QObject>
#include <QTimer>
#include <QVariantMap>
#include <QDateTime>

class DatabaseManager;

class CalibrationManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isCalibrating READ isCalibrating NOTIFY calibrationStatusChanged)
    Q_PROPERTY(int calibrationProgress READ calibrationProgress NOTIFY calibrationProgressChanged)
    Q_PROPERTY(QString calibrationStep READ calibrationStep NOTIFY calibrationStepChanged)
    Q_PROPERTY(bool isCalibrated READ isCalibrated NOTIFY calibratedStatusChanged)
    Q_PROPERTY(QDateTime lastCalibrationTime READ lastCalibrationTime NOTIFY lastCalibrationTimeChanged)
    
public:
    explicit CalibrationManager(DatabaseManager *dbManager, QObject *parent = nullptr);
    
    bool isCalibrating() const { return m_isCalibrating; }
    int calibrationProgress() const { return m_calibrationProgress; }
    QString calibrationStep() const { return m_calibrationStep; }
    bool isCalibrated() const { return m_isCalibrated; }
    QDateTime lastCalibrationTime() const { return m_lastCalibrationTime; }
    
public slots:
    Q_INVOKABLE void startCalibration(const QString &calibrationType = "full");
    Q_INVOKABLE void cancelCalibration();
    Q_INVOKABLE void acceptCalibrationStep();
    Q_INVOKABLE void retryCalibrationStep();
    Q_INVOKABLE QVariantList getCalibrationHistory();
    Q_INVOKABLE QVariantMap getLastCalibrationData();
    Q_INVOKABLE bool isCalibrationRequired();
    Q_INVOKABLE int getCalibrationValidityDays();
    
signals:
    void calibrationStatusChanged();
    void calibrationProgressChanged();
    void calibrationStepChanged();
    void calibratedStatusChanged();
    void lastCalibrationTimeChanged();
    void calibrationCompleted(bool success);
    void calibrationFailed(const QString &reason);
    void calibrationStepCompleted(const QString &step, bool success);
    
private slots:
    void performCalibrationStep();
    void onCalibrationTimeout();
    
private:
    void loadCalibrationState();
    void saveCalibrationData(const QVariantMap &data);
    void simulateCalibrationStep();
    void resetCalibration();
    void completeCalibration(bool success);
    void initializeCalibrationSteps();
    
    struct CalibrationStep {
        QString name;
        QString description;
        int duration; // in milliseconds
        QVariantMap expectedValues;
        QVariantMap tolerances;
    };
    
    DatabaseManager *m_dbManager;
    bool m_isCalibrating;
    bool m_isCalibrated;
    int m_calibrationProgress;
    QString m_calibrationStep;
    QString m_calibrationType;
    QDateTime m_lastCalibrationTime;
    QDateTime m_calibrationStartTime;
    
    QTimer *m_calibrationTimer;
    QList<CalibrationStep> m_calibrationSteps;
    int m_currentStepIndex;
    int m_stepRetryCount;
    
    static const int MAX_RETRY_COUNT = 3;
    static const int CALIBRATION_VALIDITY_DAYS = 30;
};

#endif // CALIBRATIONMANAGER_H