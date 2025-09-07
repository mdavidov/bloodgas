#ifndef HL7MANAGER_H
#define HL7MANAGER_H

#include <QObject>
#include <QVariantMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

class HL7Manager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionStatusChanged)
    Q_PROPERTY(QString serverUrl READ serverUrl WRITE setServerUrl NOTIFY serverUrlChanged)
    Q_PROPERTY(int messagesSent READ messagesSent NOTIFY messagesSentChanged)
    Q_PROPERTY(int messagesReceived READ messagesReceived NOTIFY messagesReceivedChanged)
    
public:
    explicit HL7Manager(QObject *parent = nullptr);
    
    bool isConnected() const { return m_isConnected; }
    QString serverUrl() const { return m_serverUrl; }
    void setServerUrl(const QString &url);
    int messagesSent() const { return m_messagesSent; }
    int messagesReceived() const { return m_messagesReceived; }
    
public slots:
    Q_INVOKABLE void connectToServer(const QString &url = QString());
    Q_INVOKABLE void disconnectFromServer();
    Q_INVOKABLE bool sendResults(const QVariantMap &results);
    Q_INVOKABLE bool sendPatientInfo(const QVariantMap &patientInfo);
    Q_INVOKABLE QStringList getMessageHistory();
    Q_INVOKABLE void testConnection();
    Q_INVOKABLE QString generateHL7Message(const QVariantMap &data, const QString &messageType = "ORU^R01");
    
signals:
    void connectionStatusChanged();
    void serverUrlChanged();
    void messagesSentChanged();
    void messagesReceivedChanged();
    void messageReceived(const QString &message);
    void messageSent(const QString &message);
    void connectionFailed(const QString &error);
    void hl7Error(const QString &error);
    
private slots:
    void onNetworkReply();
    void onConnectionTimeout();
    void onHeartbeatTimeout();
    
private:
    struct HL7Message {
        QString type;
        QString content;
        QDateTime timestamp;
        QString status;
    };
    
    void setupHeartbeat();
    void stopHeartbeat();
    QString formatHL7Segment(const QString &segmentType, const QStringList &fields);
    QString escapeHL7Text(const QString &text);
    QString generateMessageControlId();
    QString getUnitForField(const QString &field);
    QDateTime parseHL7DateTime(const QString &hl7DateTime);
    bool validateHL7Message(const QString &message);
    
    QNetworkAccessManager *m_networkManager;
    bool m_isConnected;
    QString m_serverUrl;
    int m_messagesSent;
    int m_messagesReceived;
    
    QTimer *m_connectionTimer;
    QTimer *m_heartbeatTimer;
    QList<HL7Message> m_messageHistory;
    
    // HL7 Configuration
    QString m_sendingApplication;
    QString m_sendingFacility;
    QString m_receivingApplication;
    QString m_receivingFacility;
    
    static const int CONNECTION_TIMEOUT_MS = 10000; // 10 seconds
    static const int HEARTBEAT_INTERVAL_MS = 60000; // 1 minute
};

#endif // HL7MANAGER_H