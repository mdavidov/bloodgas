import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts

import BloodGasAnalyzer
import "components"


ApplicationWindow {
    id: window
    width: 1024
    height: 768
    visible: true
    title: qsTr("Blood Gas Analyzer")
    
    property alias currentView: stackView.currentItem
    property bool isLoggedIn: authManager ? authManager.isLoggedIn : false
    
    // Make window touch-friendly
    flags: Qt.WindowFullscreenButtonHint | Qt.WindowTitleHint | Qt.WindowCloseButtonHint
    
    // Color scheme
    readonly property color primaryColor: "#2E86AB"
    readonly property color secondaryColor: "#A23B72"
    readonly property color accentColor: "#F18F01"
    readonly property color backgroundColor: "#F5F5F5"
    readonly property color surfaceColor: "#FFFFFF"
    readonly property color errorColor: "#E63946"
    readonly property color successColor: "#2D6A4F"
    
    background: Rectangle {
        color: backgroundColor
    }
    
    header: Rectangle {
        height: 80
        color: primaryColor
        
        RowLayout {
            anchors.fill: parent
            anchors.margins: 20
            
            // Logo/Title
            Text {
                text: "Blood Gas Analyzer"
                color: "white"
                font.pixelSize: 24
                font.bold: true
                Layout.fillWidth: true
            }
            
            // User info
            Rectangle {
                visible: isLoggedIn
                color: "transparent"
                border.color: "white"
                border.width: 1
                radius: 5
                width: userInfo.implicitWidth + 20
                height: 40
                
                Text {
                    id: userInfo
                    anchors.centerIn: parent
                    text: isLoggedIn ? "User: " + (authManager ? authManager.currentUser : "") : ""
                    color: "white"
                    font.pixelSize: 14
                }
            }
            
            // Session timer
            Rectangle {
                visible: isLoggedIn
                color: "transparent"
                border.color: "white"
                border.width: 1
                radius: 5
                width: sessionTimer.implicitWidth + 20
                height: 40
                
                Text {
                    id: sessionTimer
                    anchors.centerIn: parent
                    text: isLoggedIn ? "Session: " + Math.floor((authManager ? authManager.sessionTimeRemaining : 0) / 60) + "min" : ""
                    color: "white"
                    font.pixelSize: 14
                }
                
                Timer {
                    interval: 60000 // Update every minute
                    running: isLoggedIn
                    repeat: true
                    onTriggered: sessionTimer.text = isLoggedIn ? "Session: " + Math.floor((authManager ? authManager.sessionTimeRemaining : 0) / 60) + "min" : ""
                }
            }
            
            // Logout button
            Button {
                visible: isLoggedIn
                text: "Logout"
                onClicked: authManager.logout()
                
                background: Rectangle {
                    color: parent.pressed ? Qt.darker(secondaryColor, 1.2) : secondaryColor
                    radius: 5
                    border.color: "white"
                    border.width: 1
                }
                
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 14
                }
            }
        }
    }
    
    StackView {
        id: stackView
        anchors.fill: parent
        
        initialItem: isLoggedIn ? mainViewComponent : loginViewComponent
        
        Connections {
            target: authManager
            function onUserLoggedIn() {
                stackView.replace(mainViewComponent)
            }
            function onUserLoggedOut() {
                stackView.replace(loginViewComponent)
            }
            function onSessionExpired() {
                showMessage("Session expired. Please login again.", "warning")
            }
            function onSessionExpiring() {
                showMessage("Session will expire in 2 minutes. Extend session or save your work.", "info")
            }
        }
    }
    
    // Message overlay
    Rectangle {
        id: messageOverlay
        anchors.top: header.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: Math.min(400, parent.width - 40)
        height: messageText.implicitHeight + 20
        color: successColor
        radius: 5
        visible: false
        z: 1000
        
        Text {
            id: messageText
            anchors.centerIn: parent
            color: "white"
            font.pixelSize: 14
            wrapMode: Text.WordWrap
            width: parent.width - 20
            horizontalAlignment: Text.AlignHCenter
        }
        
        Timer {
            id: messageTimer
            interval: 3000
            onTriggered: messageOverlay.visible = false
        }
    }
    
    // Component definitions
    Component {
        id: loginViewComponent
        LoginView {}
    }
    
    Component {
        id: mainViewComponent
        MainView {}
    }
    
    // Utility functions
    function showMessage(text, type) {
        messageText.text = text
        
        switch(type) {
            case "error":
                messageOverlay.color = errorColor
                break
            case "warning":
                messageOverlay.color = accentColor
                break
            case "success":
                messageOverlay.color = successColor
                break
            case "info":
            default:
                messageOverlay.color = primaryColor
                break
        }
        
        messageOverlay.visible = true
        messageTimer.restart()
    }
    
    function navigateToView(viewName) {
        switch(viewName) {
            case "calibration":
                stackView.push("qrc:/src/qml/CalibrationView.qml")
                break
            case "history":
                stackView.push("qrc:/src/qml/HistoryView.qml")
                break
            case "sample":
                stackView.push("qrc:/src/qml/SampleInputView.qml")
                break
            default:
                console.log("Unknown view:", viewName)
        }
    }
    
    // Error handling
    Connections {
        target: bloodGasAnalyzer
        function onAnalysisError(error) {
            showMessage("Analysis Error: " + error, "error")
        }
        function onAnalysisCompleted() {
            showMessage("Analysis completed successfully!", "success")
        }
    }
    
    Connections {
        target: calibrationManager
        function onCalibrationCompleted(success) {
            showMessage(success ? "Calibration completed successfully!" : "Calibration failed. Please try again.", 
                       success ? "success" : "error")
        }
        function onCalibrationFailed(reason) {
            showMessage("Calibration failed: " + reason, "error")
        }
    }
}