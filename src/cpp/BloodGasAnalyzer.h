#ifndef BLOODGASANALYZER_H
#define BLOODGASANALYZER_H

#include <QObject>
#include <QTimer>
#include <QDateTime>

class HistoricalDataModel;
class DatabaseManager;
class AuthenticationManager;
class CalibrationManager;
class HL7Manager;

class BloodGasAnalyzer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isAnalyzing READ isAnalyzing NOTIFY isAnalyzingChanged)
    Q_PROPERTY(QString currentUser READ currentUser NOTIFY currentUserChanged)
    Q_PROPERTY(bool isCalibrated READ isCalibrated NOTIFY isCalibratedChanged)
    
public:
    explicit BloodGasAnalyzer(QObject *parent = nullptr);
    ~BloodGasAnalyzer();
    
    bool isAnalyzing() const { return m_isAnalyzing; }
    QString currentUser() const { return m_currentUser; }
    bool isCalibrated() const { return m_isCalibrated; }
    
    HistoricalDataModel* getHistoricalDataModel() const { return m_historicalDataModel; }
    DatabaseManager* getDatabaseManager() const { return m_databaseManager; }
    AuthenticationManager* getAuthenticationManager() const { return m_authManager; }
    CalibrationManager* getCalibrationManager() const { return m_calibrationManager; }
    HL7Manager* getHL7Manager() const { return m_hl7Manager; }
    
public slots:
    Q_INVOKABLE void startAnalysis(const QVariantMap &sampleData);
    Q_INVOKABLE void stopAnalysis();
    Q_INVOKABLE void exportResults(const QString &format);
    Q_INVOKABLE QVariantMap getLastResults() const;
    
signals:
    void isAnalyzingChanged();
    void currentUserChanged();
    void isCalibratedChanged();
    void analysisCompleted(const QVariantMap &results);
    void analysisError(const QString &error);
    
private slots:
    void onAnalysisTimeout();
    void onUserLoggedIn(const QString &username);
    void onUserLoggedOut();
    void onCalibrationCompleted(bool success);
    
private:
    void initializeComponents();
    QVariantMap simulateAnalysis(const QVariantMap &sampleData);
    
    HistoricalDataModel *m_historicalDataModel;
    DatabaseManager *m_databaseManager;
    AuthenticationManager *m_authManager;
    CalibrationManager *m_calibrationManager;
    HL7Manager *m_hl7Manager;
    
    QTimer *m_analysisTimer;
    bool m_isAnalyzing;
    QString m_currentUser;
    bool m_isCalibrated;
    QVariantMap m_currentSampleData;
    QVariantMap m_lastResults;
};

#endif // BLOODGASANALYZER_H