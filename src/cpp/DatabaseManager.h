#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QSqlDatabase>
#include <QVariantMap>
#include <QVariantList>

class DatabaseManager : public QObject
{
    Q_OBJECT
    
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();
    
    bool initializeDatabase();
    bool isConnected() const;
    
    // User management
    bool createUser(const QString &username, const QString &password, const QString &role = "operator");
    bool authenticateUser(const QString &username, const QString &password);
    bool updateUserPassword(const QString &username, const QString &newPassword);
    bool deleteUser(const QString &username);
    QVariantList getAllUsers();
    
    // Results management
    bool saveResult(const QVariantMap &result);
    QVariantList getAllResults();
    QVariantList getResultsByDateRange(const QDateTime &start, const QDateTime &end);
    QVariantList getResultsByOperator(const QString &operatorName);
    QVariantList getResultsByPatient(const QString &patientId);
    bool removeResult(int id);
    bool clearAllResults();
    
    // Calibration data
    bool saveCalibrationData(const QVariantMap &calibrationData);
    QVariantMap getLatestCalibrationData();
    QVariantList getCalibrationHistory();
    
    // Audit trail
    void logAuditEvent(const QString &event, const QString &username, const QVariantMap &details = QVariantMap());
    QVariantList getAuditTrail(const QDateTime &start = QDateTime(), const QDateTime &end = QDateTime());
    
signals:
    void databaseError(const QString &error);
    void connectionStatusChanged(bool connected);
    
private:
    bool createTables();
    bool createUsersTable();
    bool createResultsTable();
    bool createCalibrationTable();
    bool createAuditTable();
    
    QString hashPassword(const QString &password, const QString &salt) const;
    QString generateSalt() const;
    bool executeBatch(const QStringList &queries);
    void encryptData(QByteArray &data) const;
    void decryptData(QByteArray &data) const;
    
    QSqlDatabase m_database;
    QString m_databasePath;
    bool m_isConnected;
    QByteArray m_encryptionKey;
};

#endif // DATABASEMANAGER_H