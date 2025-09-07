#include "AuthenticationManager.h"
#include "DatabaseManager.h"

#include <QDebug>
#include <QDateTime>
#include <QTimer>

AuthenticationManager::AuthenticationManager(DatabaseManager *dbManager, QObject *parent)
    : QObject(parent)
    , m_dbManager(dbManager)
    , m_isLoggedIn(true)
    , m_sessionTimer(new QTimer(this))
    , m_warningTimer(new QTimer(this))
    , m_updateTimer(new QTimer(this))
{
    setupSessionTimers();
}

bool AuthenticationManager::login(const QString &username, const QString &password)
{
    if (username.isEmpty() || password.isEmpty()) {
        emit loginFailed("Username and password are required");
        return false;
    }
    
    if (!m_dbManager || !m_dbManager->isConnected()) {
        emit loginFailed("Database not available");
        return false;
    }
    
    // Authenticate with database
    if (!m_dbManager->authenticateUser(username, password)) {
        emit loginFailed("Invalid username or password");
        return false;
    }
    
    // Get user role (simplified - in production, fetch from database)
    QString role = "operator"; // Default role
    if (username.toLower() == "admin") {
        role = "administrator";
    } else if (username.toLower() == "supervisor") {
        role = "supervisor";
    }
    
    // Start session
    m_currentUser = username;
    m_currentUserRole = role;
    startSession();
    
    emit currentUserChanged();
    emit currentUserRoleChanged();
    emit userLoggedIn(username);
    
    qDebug() << "User logged in:" << username << "(" << role << ")";
    return true;
}

void AuthenticationManager::logout()
{
    if (!m_isLoggedIn) {
        return;
    }
    
    QString previousUser = m_currentUser;
    endSession();
    
    emit userLoggedOut();
    qDebug() << "User logged out:" << previousUser;
}

bool AuthenticationManager::changePassword(const QString &currentPassword, const QString &newPassword)
{
    if (!m_isLoggedIn) {
        return false;
    }
    
    if (currentPassword.isEmpty() || newPassword.isEmpty()) {
        return false;
    }
    
    // Validate current password
    if (!m_dbManager || !m_dbManager->authenticateUser(m_currentUser, currentPassword)) {
        return false;
    }
    
    // Update password in database
    return m_dbManager->updateUserPassword(m_currentUser, newPassword);
}

void AuthenticationManager::extendSession()
{
    if (!m_isLoggedIn) {
        return;
    }
    
    // Reset session timers
    m_lastActivity = QDateTime::currentDateTime();
    m_sessionTimer->start(SESSION_DURATION_MS);
    m_warningTimer->start(SESSION_DURATION_MS - WARNING_TIME_MS);
    
    emit sessionTimeRemainingChanged();
    
    qDebug() << "Session extended for user:" << m_currentUser;
}

bool AuthenticationManager::hasPermission(const QString &permission) const
{
    if (!m_isLoggedIn) {
        return false;
    }
    
    // Role-based permissions
    if (m_currentUserRole == "administrator") {
        return true; // Admin has all permissions
    }
    
    if (m_currentUserRole == "supervisor") {
        // Supervisors can do most things except user management
        return permission != "user_management" && permission != "system_config";
    }
    
    if (m_currentUserRole == "operator") {
        // Operators can perform basic operations
        QStringList operatorPermissions = {
            "run_analysis",
            "view_results",
            "export_results",
            "basic_calibration"
        };
        return operatorPermissions.contains(permission);
    }
    
    return false;
}

int AuthenticationManager::sessionTimeRemaining() const
{
    if (!m_isLoggedIn) {
        return 0;
    }
    
    QDateTime now = QDateTime::currentDateTime();
    QDateTime expiry = m_lastActivity.addMSecs(SESSION_DURATION_MS);
    
    return qMax(0, (int)now.msecsTo(expiry) / 1000); // Return seconds
}

void AuthenticationManager::startSession()
{
    m_isLoggedIn = true;
    m_loginTime = QDateTime::currentDateTime();
    m_lastActivity = m_loginTime;
    
    // Start session timers
    m_sessionTimer->start(SESSION_DURATION_MS);
    m_warningTimer->start(SESSION_DURATION_MS - WARNING_TIME_MS);
    m_updateTimer->start(60000); // Update every minute
    
    emit loginStatusChanged();
    emit sessionTimeRemainingChanged();
}

void AuthenticationManager::endSession()
{
    m_isLoggedIn = false;
    m_currentUser.clear();
    m_currentUserRole.clear();
    m_loginTime = QDateTime();
    m_lastActivity = QDateTime();
    
    // Stop all timers
    m_sessionTimer->stop();
    m_warningTimer->stop();
    m_updateTimer->stop();
    
    emit loginStatusChanged();
    emit currentUserChanged();
    emit currentUserRoleChanged();
    emit sessionTimeRemainingChanged();
}

void AuthenticationManager::setupSessionTimers()
{
    // Session timeout timer
    m_sessionTimer->setSingleShot(true);
    connect(m_sessionTimer, &QTimer::timeout, this, &AuthenticationManager::onSessionTimeout);
    
    // Session warning timer
    m_warningTimer->setSingleShot(true);
    connect(m_warningTimer, &QTimer::timeout, this, &AuthenticationManager::onSessionWarning);
    
    // Update timer for remaining time display
    m_updateTimer->setSingleShot(false);
    connect(m_updateTimer, &QTimer::timeout, this, &AuthenticationManager::updateSessionTimer);
}

void AuthenticationManager::onSessionTimeout()
{
    if (m_isLoggedIn) {
        qDebug() << "Session expired for user:" << m_currentUser;
        endSession();
        emit sessionExpired();
    }
}

void AuthenticationManager::onSessionWarning()
{
    if (m_isLoggedIn) {
        emit sessionExpiring();
        qDebug() << "Session expiring warning for user:" << m_currentUser;
    }
}

void AuthenticationManager::updateSessionTimer()
{
    if (m_isLoggedIn) {
        emit sessionTimeRemainingChanged();
    }
}