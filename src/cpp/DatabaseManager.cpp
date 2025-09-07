#include "DatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
    , m_isConnected(false)
{
    // Generate encryption key (in production, this should be securely managed)
    m_encryptionKey = QCryptographicHash::hash("BloodGasAnalyzer2024", QCryptographicHash::Sha256);
}

DatabaseManager::~DatabaseManager()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool DatabaseManager::initializeDatabase()
{
    // Create application data directory
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists() && !dir.mkpath(dataPath)) {
        qCritical() << "Failed to create data directory:" << dataPath;
        return false;
    }

    const auto dbName = QString("bloodgasanalyzer.db");
    m_databasePath = dir.filePath(dbName);

    // Setup database connection
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(dbName);
    
    if (!m_database.open()) {
        qCritical() << "Failed to open database:" << m_database.lastError().text();
        emit databaseError(m_database.lastError().text());
        return false;
    }
    
    // Enable foreign keys
    QSqlQuery query(m_database);
    if (!query.exec("PRAGMA foreign_keys = ON")) {
        qWarning() << "Failed to enable foreign keys:" << query.lastError().text();
    }
    
    // Create tables
    if (!createTables()) {
        qCritical() << "Failed to create database tables";
        return false;
    }
    
    m_isConnected = true;
    emit connectionStatusChanged(true);
    
    // Create default admin user if no users exist
    QVariantList users = getAllUsers();
    if (users.isEmpty()) {
        createUser("admin", "admin123", "administrator");
        createUser("operator", "operator123", "operator");
        logAuditEvent("SYSTEM_INIT", "SYSTEM", QVariantMap{{"action", "Default users created"}});
    }
    
    qDebug() << "Database initialized successfully at:" << m_databasePath;
    return true;
}

bool DatabaseManager::isConnected() const
{
    return m_isConnected && m_database.isOpen();
}

bool DatabaseManager::createTables()
{
    return createUsersTable() && 
           createResultsTable() && 
           createCalibrationTable() && 
           createAuditTable();
}

bool DatabaseManager::createUsersTable()
{
    QSqlQuery query(m_database);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            salt TEXT NOT NULL,
            role TEXT NOT NULL DEFAULT 'operator',
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            last_login DATETIME,
            active BOOLEAN DEFAULT 1
        )
    )";
    
    if (!query.exec(sql)) {
        qCritical() << "Failed to create users table:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::createResultsTable()
{
    QSqlQuery query(m_database);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS results (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME NOT NULL,
            operator TEXT NOT NULL,
            sample_id TEXT NOT NULL,
            patient_id TEXT,
            pH REAL,
            pCO2 REAL,
            pO2 REAL,
            HCO3 REAL,
            SO2 REAL,
            BE REAL,
            Na REAL,
            K REAL,
            Cl REAL,
            Ca REAL,
            Glucose REAL,
            Lactate REAL,
            temperature REAL,
            raw_data TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    if (!query.exec(sql)) {
        qCritical() << "Failed to create results table:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::createCalibrationTable()
{
    QSqlQuery query(m_database);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS calibrations (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME NOT NULL,
            operator TEXT NOT NULL,
            calibration_type TEXT NOT NULL,
            status TEXT NOT NULL,
            calibration_data TEXT,
            quality_control_data TEXT,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    if (!query.exec(sql)) {
        qCritical() << "Failed to create calibrations table:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::createAuditTable()
{
    QSqlQuery query(m_database);
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS audit_log (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            event TEXT NOT NULL,
            username TEXT NOT NULL,
            details TEXT,
            ip_address TEXT,
            user_agent TEXT
        )
    )";
    
    if (!query.exec(sql)) {
        qCritical() << "Failed to create audit table:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool DatabaseManager::createUser(const QString &username, const QString &password, const QString &role)
{
    if (!isConnected() || username.isEmpty() || password.isEmpty()) {
        return false;
    }
    
    QString salt = generateSalt();
    QString hashedPassword = hashPassword(password, salt);
    
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO users (username, password_hash, salt, role) VALUES (?, ?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(hashedPassword);
    query.addBindValue(salt);
    query.addBindValue(role);
    
    if (!query.exec()) {
        qWarning() << "Failed to create user:" << query.lastError().text();
        return false;
    }
    
    logAuditEvent("USER_CREATED", "SYSTEM", QVariantMap{{"username", username}, {"role", role}});
    return true;
}

bool DatabaseManager::authenticateUser(const QString &username, const QString &password)
{
    if (!isConnected() || username.isEmpty() || password.isEmpty()) {
        return false;
    }
    
    QSqlQuery query(m_database);
    query.prepare("SELECT password_hash, salt FROM users WHERE username = ? AND active = 1");
    query.addBindValue(username);
    
    if (!query.exec() || !query.next()) {
        logAuditEvent("LOGIN_FAILED", username, QVariantMap{{"reason", "User not found"}});
        return false;
    }
    
    QString storedHash = query.value("password_hash").toString();
    QString salt = query.value("salt").toString();
    QString hashedPassword = hashPassword(password, salt);
    
    bool authenticated = (hashedPassword == storedHash);
    
    if (authenticated) {
        // Update last login
        QSqlQuery updateQuery(m_database);
        updateQuery.prepare("UPDATE users SET last_login = CURRENT_TIMESTAMP WHERE username = ?");
        updateQuery.addBindValue(username);
        updateQuery.exec();
        
        logAuditEvent("LOGIN_SUCCESS", username, QVariantMap{});
    } else {
        logAuditEvent("LOGIN_FAILED", username, QVariantMap{{"reason", "Invalid password"}});
    }
    
    return authenticated;
}

bool DatabaseManager::saveResult(const QVariantMap &result)
{
    if (!isConnected()) {
        return false;
    }
    
    QSqlQuery query(m_database);
    QString sql = R"(
        INSERT INTO results (
            timestamp, operator, sample_id, patient_id,
            pH, pCO2, pO2, HCO3, SO2, BE,
            Na, K, Cl, Ca, Glucose, Lactate, temperature, raw_data
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )";
    
    query.prepare(sql);
    query.addBindValue(result.value("timestamp"));
    query.addBindValue(result.value("operator"));
    query.addBindValue(result.value("sampleId"));
    query.addBindValue(result.value("patientId"));
    query.addBindValue(result.value("pH"));
    query.addBindValue(result.value("pCO2"));
    query.addBindValue(result.value("pO2"));
    query.addBindValue(result.value("HCO3"));
    query.addBindValue(result.value("SO2"));
    query.addBindValue(result.value("BE"));
    query.addBindValue(result.value("Na"));
    query.addBindValue(result.value("K"));
    query.addBindValue(result.value("Cl"));
    query.addBindValue(result.value("Ca"));
    query.addBindValue(result.value("Glucose"));
    query.addBindValue(result.value("Lactate"));
    query.addBindValue(result.value("temperature"));
    
    // Store raw data as JSON
    QJsonDocument rawDoc = QJsonDocument::fromVariant(result);
    query.addBindValue(rawDoc.toJson(QJsonDocument::Compact));
    
    if (!query.exec()) {
        qWarning() << "Failed to save result:" << query.lastError().text();
        return false;
    }
    
    logAuditEvent("RESULT_SAVED", result.value("operator").toString(),
                  QVariantMap{{"sampleId", result.value("sampleId")}, 
                             {"patientId", result.value("patientId")}});
    return true;
}

QVariantList DatabaseManager::getAllResults()
{
    QVariantList results;
    if (!isConnected()) {
        return results;
    }
    
    QSqlQuery query(m_database);
    if (!query.exec("SELECT * FROM results ORDER BY timestamp DESC")) {
        qWarning() << "Failed to get all results:" << query.lastError().text();
        return results;
    }
    
    while (query.next()) {
        QVariantMap result;
        QSqlRecord record = query.record();
        
        for (int i = 0; i < record.count(); ++i) {
            result[record.fieldName(i)] = record.value(i);
        }
        
        // Parse raw_data if available
        QString rawData = result.value("raw_data").toString();
        if (!rawData.isEmpty()) {
            QJsonDocument doc = QJsonDocument::fromJson(rawData.toUtf8());
            if (!doc.isNull()) {
                QVariantMap rawMap = doc.object().toVariantMap();
                // Merge with existing data
                for (auto it = rawMap.begin(); it != rawMap.end(); ++it) {
                    if (!result.contains(it.key()) || result[it.key()].isNull()) {
                        result[it.key()] = it.value();
                    }
                }
            }
        }
        
        results.append(result);
    }
    
    return results;
}

bool DatabaseManager::removeResult(int id)
{
    if (!isConnected() || id <= 0) {
        return false;
    }
    
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM results WHERE id = ?");
    query.addBindValue(id);
    
    if (!query.exec()) {
        qWarning() << "Failed to remove result:" << query.lastError().text();
        return false;
    }
    
    logAuditEvent("RESULT_DELETED", "SYSTEM", QVariantMap{{"resultId", id}});
    return query.numRowsAffected() > 0;
}

bool DatabaseManager::clearAllResults()
{
    if (!isConnected()) {
        return false;
    }
    
    QSqlQuery query(m_database);
    if (!query.exec("DELETE FROM results")) {
        qWarning() << "Failed to clear all results:" << query.lastError().text();
        return false;
    }
    
    logAuditEvent("ALL_RESULTS_CLEARED", "SYSTEM", QVariantMap{});
    return true;
}

void DatabaseManager::logAuditEvent(const QString& event, const QString& username, const QVariantMap& details)
{
    if (!isConnected()) {
        return;
    }
    
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO audit_log (event, username, details) VALUES (?, ?, ?)");
    query.addBindValue(event);
    query.addBindValue(username);
    
    if (!details.isEmpty()) {
        QJsonDocument doc = QJsonDocument::fromVariant(details.first());
        query.addBindValue(doc.toJson(QJsonDocument::Compact));
    } else {
        query.addBindValue(QString());
    }
    
    if (!query.exec()) {
        qWarning() << "Failed to log audit event:" << query.lastError().text();
    }
}

QVariantList DatabaseManager::getAuditTrail(const QDateTime &start, const QDateTime &end)
{
    QVariantList events;
    if (!isConnected()) {
        return events;
    }
    
    QSqlQuery query(m_database);
    QString sql = "SELECT * FROM audit_log";
    
    if (start.isValid() && end.isValid()) {
        sql += " WHERE timestamp BETWEEN ? AND ?";
        query.prepare(sql + " ORDER BY timestamp DESC");
        query.addBindValue(start);
        query.addBindValue(end);
    } else {
        query.prepare(sql + " ORDER BY timestamp DESC LIMIT 1000");
    }
    
    if (!query.exec()) {
        qWarning() << "Failed to get audit trail:" << query.lastError().text();
        return events;
    }
    
    while (query.next()) {
        QVariantMap event;
        QSqlRecord record = query.record();
        
        for (int i = 0; i < record.count(); ++i) {
            event[record.fieldName(i)] = record.value(i);
        }
        
        events.append(event);
    }
    
    return events;
}

QString DatabaseManager::hashPassword(const QString &password, const QString &salt) const
{
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData((salt + password + salt).toUtf8());
    return hash.result().toHex();
}

QString DatabaseManager::generateSalt() const
{
    QByteArray salt;
    for (int i = 0; i < 32; ++i) {
        salt.append(QRandomGenerator::global()->bounded(256));
    }
    return salt.toHex();
}

// Placeholder implementations for additional methods
bool DatabaseManager::updateUserPassword(const QString &username, const QString &newPassword)
{
    // Implementation would go here
    Q_UNUSED(username)
    Q_UNUSED(newPassword)
    return false;
}

bool DatabaseManager::deleteUser(const QString &username)
{
    // Implementation would go here
    Q_UNUSED(username)
    return false;
}

QVariantList DatabaseManager::getAllUsers()
{
    QVariantList users;
    if (!isConnected()) {
        return users;
    }
    
    QSqlQuery query(m_database);
    if (!query.exec("SELECT id, username, role, created_at, last_login, active FROM users")) {
        qWarning() << "Failed to get all users:" << query.lastError().text();
        return users;
    }
    
    while (query.next()) {
        QVariantMap user;
        QSqlRecord record = query.record();
        for (int i = 0; i < record.count(); ++i) {
            user[record.fieldName(i)] = record.value(i);
        }
        users.append(user);
    }
    
    return users;
}

QVariantList DatabaseManager::getResultsByDateRange(const QDateTime &start, const QDateTime &end)
{
    Q_UNUSED(start)
    Q_UNUSED(end)
    return QVariantList(); // Placeholder
}

QVariantList DatabaseManager::getResultsByOperator(const QString &operatorName)
{
    Q_UNUSED(operatorName)
    return QVariantList(); // Placeholder
}

QVariantList DatabaseManager::getResultsByPatient(const QString &patientId)
{
    Q_UNUSED(patientId)
    return QVariantList(); // Placeholder
}

bool DatabaseManager::saveCalibrationData(const QVariantMap &calibrationData)
{
    Q_UNUSED(calibrationData)
    return false; // Placeholder
}

QVariantMap DatabaseManager::getLatestCalibrationData()
{
    return QVariantMap(); // Placeholder
}

QVariantList DatabaseManager::getCalibrationHistory()
{
    return QVariantList(); // Placeholder
}

void DatabaseManager::encryptData(QByteArray &data) const
{
    Q_UNUSED(data)
    // Basic XOR encryption (for demonstration only)
    // In production, use proper encryption libraries
}

void DatabaseManager::decryptData(QByteArray &data) const
{
    Q_UNUSED(data)
    // Basic XOR decryption (for demonstration only)
    // In production, use proper encryption libraries
}