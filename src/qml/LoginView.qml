import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "components"


Rectangle {
    id: root
    color: "lightgrey" //window.backgroundColor

    property bool loginInProgress: false

    // Background with medical imagery
    Rectangle {
        anchors.fill: parent
        color: "#E8F4FD"
        
        // Subtle pattern overlay
        Repeater {
            model: 20
            Rectangle {
                x: Math.random() * parent.width
                y: Math.random() * parent.height
                width: 2
                height: 2
                color: window.primaryColor
                opacity: 0.1
                radius: 1
            }
        }
    }
    
    // Login form container
    Rectangle {
        id: loginContainer
        anchors.centerIn: parent
        width: Math.min(400, root.width - 80)
        height: Math.min(500, root.height - 80)
        color: "lightgrey" //window.surfaceColor
        radius: 15
        border.color: window.primaryColor
        border.width: 2
        
        // Drop shadow effect
        Rectangle {
            anchors.fill: parent
            anchors.margins: -5
            color: "transparent"
            border.color: Qt.rgba(0, 0, 0, 0.1)
            border.width: 1
            radius: parent.radius + 5
            z: -1
        }
        
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 40
            spacing: 30
            
            // Logo and title
            Column {
                Layout.alignment: Qt.AlignHCenter
                spacing: 10
                
                Rectangle {
                    id: logo
                    width: 80
                    height: 80
                    color: window.primaryColor
                    radius: 40
                    anchors.horizontalCenter: parent.horizontalCenter
                    
                    Text {
                        anchors.centerIn: parent
                        text: "BGA"
                        color: "white"
                        font.pixelSize: 24
                        font.bold: true
                    }
                }
                
                Text {
                    text: "Blood Gas Analyzer"
                    font.pixelSize: 28
                    font.bold: true
                    color: window.primaryColor
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                
                Text {
                    text: "Secure Login Required"
                    font.pixelSize: 16
                    color: "#666666"
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
            
            // Login form
            Column {
                Layout.fillWidth: true
                spacing: 20
                
                // Username field
                InputField {
                    id: usernameField
                    width: parent.width
                    placeholderText: "Username"
                    icon: "👤"
                    focus: true
                    
                    onAccepted: passwordField.focus = true
                }
                
                // Password field
                InputField {
                    id: passwordField
                    width: parent.width
                    placeholderText: "Password"
                    icon: "🔒"
                    echoMode: TextInput.Password
                    
                    onAccepted: loginButton.clicked()
                }
                
                // Login button
                TouchButton {
                    id: loginButton
                    width: parent.width
                    text: loginInProgress ? "Logging in..." : "Login"
                    enabled: !loginInProgress && usernameField.text.length > 0 && passwordField.text.length > 0
                    
                    onClicked: performLogin()
                }
                
                // Demo user info
                Rectangle {
                    width: parent.width
                    height: demoInfo.implicitHeight + 20
                    color: "#F0F8FF"
                    border.color: window.primaryColor
                    border.width: 1
                    radius: 5
                    
                    Text {
                        id: demoInfo
                        anchors.centerIn: parent
                        width: parent.width - 20
                        text: "Demo Users:\n• admin / admin123 (Administrator)\n• operator / operator123 (Operator)"
                        font.pixelSize: 12
                        color: "#666666"
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
            
            // Status message
            Text {
                id: statusMessage
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 14
                color: window.errorColor
                visible: text.length > 0
                wrapMode: Text.WordWrap
            }
            
            Item {
                Layout.fillHeight: true
            }
            
            // Footer info
            Text {
                text: "Demonstration Medical Device - Not regulatory approved"
                font.pixelSize: 18
                font.bold: true
                color: "#999999"
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }
    
    // Loading overlay
    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.3)
        visible: loginInProgress
        
        Rectangle {
            anchors.centerIn: parent
            width: 100
            height: 100
            color: "white"
            radius: 10
            
            Column {
                anchors.centerIn: parent
                spacing: 10
                
                BusyIndicator {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 40
                    height: 40
                }
                
                Text {
                    text: "Logging in..."
                    font.pixelSize: 12
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }
    
    // Functions
    function performLogin() {
        if (loginInProgress) return
        
        var username = usernameField.text.trim()
        var password = passwordField.text
        
        if (username.length === 0) {
            statusMessage.text = "Please enter username"
            usernameField.focus = true
            return
        }
        
        if (password.length === 0) {
            statusMessage.text = "Please enter password"
            passwordField.focus = true
            return
        }
        
        statusMessage.text = ""
        loginInProgress = true
        
        // Perform login
        if (authManager && authManager.login(username, password)) {
            // Success is handled by the signal connection in main.qml
            console.log("Login successful")
        } else {
            // Login failed
            statusMessage.text = "Invalid username or password"
            loginInProgress = false
            passwordField.text = ""
            passwordField.focus = true
        }
    }
    
    function clearForm() {
        usernameField.text = ""
        passwordField.text = ""
        statusMessage.text = ""
        usernameField.focus = true
    }
    
    // Handle authentication manager signals
    Connections {
        target: authManager
        function onLoginFailed(reason) {
            statusMessage.text = reason
            loginInProgress = false
            passwordField.text = ""
            passwordField.focus = true
        }
        function onUserLoggedIn() {
            loginInProgress = false
            clearForm()
        }
    }
    
    // Auto-focus on show
    Component.onCompleted: {
        usernameField.focus = true
    }
}
