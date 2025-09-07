#include "HL7Manager.h"

#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QRandomGenerator>

HL7Manager::HL7Manager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_isConnected(false)
    , m_messagesSent(0)
    , m_messagesReceived(0)
    , m_connectionTimer(new QTimer(this))
    , m_heartbeatTimer(new QTimer(this))
{
    // Initialize HL7 configuration
    m_sendingApplication = "BloodGasAnalyzer";
    m_sendingFacility = "LAB";
    m_receivingApplication = "HIS";
    m_receivingFacility = "HOSPITAL";
    
    // Setup timers
    m_connectionTimer->setSingleShot(true);
    connect(m_connectionTimer, &QTimer::timeout, this, &HL7Manager::onConnectionTimeout);
    
    m_heartbeatTimer->setSingleShot(false);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &HL7Manager::onHeartbeatTimeout);
    
    // Default server URL (for demonstration)
    m_serverUrl = "http://localhost:8080/hl7";
}

void HL7Manager::setServerUrl(const QString &url)
{
    if (m_serverUrl != url) {
        m_serverUrl = url;
        emit serverUrlChanged();
        
        // Reconnect if we were connected
        if (m_isConnected) {
            disconnectFromServer();
            connectToServer();
        }
    }
}

void HL7Manager::connectToServer(const QString &url)
{
    if (!url.isEmpty()) {
        setServerUrl(url);
    }
    
    if (m_serverUrl.isEmpty()) {
        emit connectionFailed("No server URL specified");
        return;
    }
    
    // For demonstration purposes, simulate connection
    // In a real implementation, this would establish an actual HL7 connection
    m_connectionTimer->start(CONNECTION_TIMEOUT_MS);
    
    // Simulate connection success after a short delay
    // QTimer::singleShot(2000, this, [this]() {
        m_connectionTimer->stop();
        m_isConnected = true;
        emit connectionStatusChanged();
        setupHeartbeat();
        qDebug() << "Connected to HL7 server:" << m_serverUrl;
    // });
}

void HL7Manager::disconnectFromServer()
{
    if (!m_isConnected) {
        return;
    }
    
    stopHeartbeat();
    m_isConnected = false;
    emit connectionStatusChanged();
    
    qDebug() << "Disconnected from HL7 server";
}

bool HL7Manager::sendResults(const QVariantMap &results)
{
    if (!m_isConnected) {
        emit hl7Error("Not connected to HL7 server");
        return false;
    }
    
    // Generate HL7 ORU^R01 message for lab results
    QString hl7Message = generateHL7Message(results, "ORU^R01");
    
    if (!validateHL7Message(hl7Message)) {
        emit hl7Error("Invalid HL7 message generated");
        return false;
    }
    
    // Store message in history
    HL7Message message;
    message.type = "ORU^R01";
    message.content = hl7Message;
    message.timestamp = QDateTime::currentDateTime();
    message.status = "SENT";
    m_messageHistory.append(message);
    
    m_messagesSent++;
    emit messagesSentChanged();
    emit messageSent(hl7Message);
    
    qDebug() << "Sent HL7 results message";
    return true;
}

bool HL7Manager::sendPatientInfo(const QVariantMap &patientInfo)
{
    if (!m_isConnected) {
        emit hl7Error("Not connected to HL7 server");
        return false;
    }
    
    // Generate HL7 ADT^A04 message for patient registration
    QString hl7Message = generateHL7Message(patientInfo, "ADT^A04");
    
    // Store message in history
    HL7Message message;
    message.type = "ADT^A04";
    message.content = hl7Message;
    message.timestamp = QDateTime::currentDateTime();
    message.status = "SENT";
    m_messageHistory.append(message);
    
    m_messagesSent++;
    emit messagesSentChanged();
    emit messageSent(hl7Message);
    
    qDebug() << "Sent HL7 patient info message";
    return true;
}

QString HL7Manager::generateHL7Message(const QVariantMap &data, const QString &messageType)
{
    QStringList segments;
    QString controlId = generateMessageControlId();
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    
    // MSH - Message Header
    QStringList mshFields = {
        "MSH",
        "^~\\&",
        m_sendingApplication,
        m_sendingFacility,
        m_receivingApplication,
        m_receivingFacility,
        timestamp,
        "",
        messageType,
        controlId,
        "P", // Processing ID
        "2.5" // Version ID
    };
    segments.append(mshFields.join("|"));
    
    if (messageType == "ORU^R01") {
        // Lab results message
        
        // PID - Patient Identification
        QString patientId = data.value("patientId", "UNKNOWN").toString();
        QStringList pidFields = {
            "PID",
            "1",
            patientId,
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            ""
        };
        segments.append(pidFields.join("|"));
        
        // OBR - Observation Request
        QString sampleId = data.value("sampleId", "AUTO").toString();
        QStringList obrFields = {
            "OBR",
            "1",
            sampleId,
            "",
            "BGA^Blood Gas Analysis^LOCAL",
            "",
            timestamp,
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            ""
        };
        segments.append(obrFields.join("|"));
        
        // OBX - Observation/Result segments
        int seqNum = 1;
        QStringList resultFields = {"pH", "pCO2", "pO2", "HCO3", "SO2", "BE", "Na", "K", "Cl", "Ca", "Glucose", "Lactate"};
        
        for (const QString &field : resultFields) {
            if (data.contains(field)) {
                QStringList obxFields = {
                    "OBX",
                    QString::number(seqNum++),
                    "NM", // Numeric
                    field + "^" + field + "^LOCAL",
                    "",
                    data.value(field).toString(),
                    getUnitForField(field),
                    "",
                    "",
                    "F", // Final
                    "",
                    "",
                    timestamp,
                    "",
                    ""
                };
                segments.append(obxFields.join("|"));
            }
        }
    }
    
    return segments.join("\r");
}

QString HL7Manager::getUnitForField(const QString &field)
{
    static QMap<QString, QString> units = {
        {"pH", "pH"},
        {"pCO2", "mmHg"},
        {"pO2", "mmHg"},
        {"HCO3", "mmol/L"},
        {"SO2", "%"},
        {"BE", "mmol/L"},
        {"Na", "mmol/L"},
        {"K", "mmol/L"},
        {"Cl", "mmol/L"},
        {"Ca", "mmol/L"},
        {"Glucose", "mg/dL"},
        {"Lactate", "mmol/L"}
    };
    
    return units.value(field, "");
}

QStringList HL7Manager::getMessageHistory()
{
    QStringList history;
    for (const HL7Message &msg : m_messageHistory) {
        QString entry = QString("[%1] %2 - %3")
                       .arg(msg.timestamp.toString("yyyy-MM-dd hh:mm:ss"))
                       .arg(msg.type)
                       .arg(msg.status);
        history.append(entry);
    }
    return history;
}

void HL7Manager::testConnection()
{
    if (!m_isConnected) {
        emit connectionFailed("Not connected to server");
        return;
    }
    
    // Send a simple test message
    QVariantMap testData;
    testData["test"] = "connection";
    testData["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    // For demonstration, just log the test
    qDebug() << "HL7 connection test performed";
}

QString HL7Manager::generateMessageControlId()
{
    return QString::number(QDateTime::currentSecsSinceEpoch()) + 
           QString::number(QRandomGenerator::global()->bounded(1000));
}

bool HL7Manager::validateHL7Message(const QString &message)
{
    // Basic validation - check if message starts with MSH
    return message.startsWith("MSH|") && message.contains("\r");
}

QString HL7Manager::escapeHL7Text(const QString &text)
{
    QString escaped = text;
    escaped.replace("\\", "\\E\\");
    escaped.replace("|", "\\F\\");
    escaped.replace("^", "\\S\\");
    escaped.replace("&", "\\T\\");
    escaped.replace("\r", "\\X0D\\");
    return escaped;
}

void HL7Manager::setupHeartbeat()
{
    m_heartbeatTimer->start(HEARTBEAT_INTERVAL_MS);
}

void HL7Manager::stopHeartbeat()
{
    m_heartbeatTimer->stop();
}

void HL7Manager::onConnectionTimeout()
{
    emit connectionFailed("Connection timeout");
}

void HL7Manager::onHeartbeatTimeout()
{
    if (m_isConnected) {
        // Send keepalive message
        qDebug() << "HL7 heartbeat sent";
    }
}

void HL7Manager::onNetworkReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        return;
    }
    
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        m_messagesReceived++;
        emit messagesReceivedChanged();
        emit messageReceived(QString::fromUtf8(response));
    } else {
        emit hl7Error(reply->errorString());
    }
    
    reply->deleteLater();
}