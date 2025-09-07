#ifndef AUTHENTICATIONMANAGER_H
#define AUTHENTICATIONMANAGER_H

#include <QObject>
#include <QTimer>
#include <QDateTime>

class DatabaseManager;

class AuthenticationManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isLoggedIn READ isLoggedIn NOTIFY loginStatusChanged)
    Q_PROPERTY(QString currentUser READ currentUser NOTIFY currentUserChanged)
    Q_PROPERTY(QString currentUserRole READ currentUserRole NOTIFY currentUserRoleChanged)
    Q_PROPERTY(int sessionTimeRemaining READ sessionTimeRemaining NOTIFY sessionTimeRemainingChanged)
    
public:
    explicit AuthenticationManager(DatabaseManager *dbManager, QObject *parent = nullptr);
    
    bool isLoggedIn() const { return m_isLoggedIn; }
    QString currentUser() const { return m_currentUser; }
    QString currentUserRole() const { return m_currentUserRole; }
    int sessionTimeRemaining() const;
    
public slots:
    Q_INVOKABLE bool login(const QString &username, const QString &password);
    Q_INVOKABLE void logout();
    Q_INVOKABLE bool changePassword(const QString &currentPassword, const QString &newPassword);
    Q_INVOKABLE void extendSession();
    Q_INVOKABLE bool hasPermission(const QString &permission) const;
    
signals:
    void loginStatusChanged();
    void currentUserChanged();
    void currentUserRoleChanged();
    void sessionTimeRemainingChanged();
    void userLoggedIn(const QString &username);
    void userLoggedOut();
    void loginFailed(const QString &reason);
    void sessionExpiring(); // Warning 2 minutes before expiry
    void sessionExpired();
    
private slots:
    void onSessionTimeout();
    void onSessionWarning();
    void updateSessionTimer();
    
private:
    void startSession();
    void endSession();
    void setupSessionTimers();
    
    DatabaseManager *m_dbManager;
    bool m_isLoggedIn;
    QString m_currentUser;
    QString m_currentUserRole;
    QDateTime m_loginTime;
    QDateTime m_lastActivity;
    
    QTimer *m_sessionTimer;        // Main session timeout (30 minutes)
    QTimer *m_warningTimer;        // Warning timer (28 minutes)
    QTimer *m_updateTimer;         // Updates remaining time display (1 minute)
    
    static const int SESSION_DURATION_MS = 30 * 60 * 1000; // 30 minutes
    static const int WARNING_TIME_MS = 2 * 60 * 1000;      // 2 minutes warning
};

#endif // AUTHENTICATIONMANAGER_H