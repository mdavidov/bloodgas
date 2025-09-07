import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "components"

Rectangle {
    id: root
    color: window.backgroundColor
    
    property bool deviceReady: calibrationManager ? calibrationManager.isCalibrated : false
    property bool analysisInProgress: bloodGasAnalyzer ? bloodGasAnalyzer.isAnalyzing : false
    
    RowLayout {
        anchors.fill: parent
        spacing: 0
        
        // Sidebar navigation
        Rectangle {
            Layout.preferredWidth: 200
            Layout.fillHeight: true
            color: window.primaryColor
            
            Column {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 10
                
                Text {
                    text: "Navigation"
                    color: "white"
                    font.pixelSize: 18
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                
                Rectangle {
                    width: parent.width
                    height: 2
                    color: "white"
                    opacity: 0.3
                }
                
                // Navigation buttons
                TouchButton {
                    width: parent.width
                    text: "New Analysis"
                    useAccentColor: true
                    enabled: deviceReady && !analysisInProgress
                    onClicked: window.navigateToView("sample")
                }
                
                TouchButton {
                    width: parent.width
                    text: "View History"
                    onClicked: window.navigateToView("history")
                }
                
                TouchButton {
                    width: parent.width
                    text: "Calibration"
                    onClicked: window.navigateToView("calibration")
                }
                
                Rectangle {
                    width: parent.width
                    height: 2
                    color: "white"
                    opacity: 0.3
                }
                
                // Device status
                Column {
                    width: parent.width
                    spacing: 5
                    
                    Text {
                        text: "Device Status"
                        color: "white"
                        font.pixelSize: 14
                        font.bold: true
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    
                    Rectangle {
                        width: parent.width
                        height: 30
                        color: deviceReady ? window.successColor : window.errorColor
                        radius: 5
                        
                        Text {
                            anchors.centerIn: parent
                            text: deviceReady ? "Ready" : "Needs Calibration"
                            color: "white"
                            font.pixelSize: 12
                            font.bold: true
                        }
                    }
                    
                    Rectangle {
                        visible: analysisInProgress
                        width: parent.width
                        height: 30
                        color: window.accentColor
                        radius: 5
                        
                        Text {
                            anchors.centerIn: parent
                            text: "Analyzing..."
                            color: "white"
                            font.pixelSize: 12
                            font.bold: true
                        }
                        
                        // Animated indicator
                        Rectangle {
                            width: 4
                            height: parent.height
                            color: Qt.lighter(parent.color, 1.3)
                            x: animatedIndicator.x
                            
                            SequentialAnimation on x {
                                running: analysisInProgress
                                loops: Animation.Infinite
                                PropertyAnimation { from: 0; to: parent.width - 4; duration: 1000 }
                                PropertyAnimation { from: parent.width - 4; to: 0; duration: 1000 }
                            }
                            
                            NumberAnimation {
                                id: animatedIndicator
                            }
                        }
                    }
                }
            }
        }
        
        // Main content area
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: window.backgroundColor
            
            ScrollView {
                anchors.fill: parent
                anchors.margins: 20
                clip: true
                
                Column {
                    width: parent.width
                    spacing: 30
                    
                    // Welcome section
                    Rectangle {
                        width: parent.width
                        height: 120
                        color: window.surfaceColor
                        radius: 10
                        border.color: window.primaryColor
                        border.width: 1
                        
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 20
                            
                            Column {
                                Layout.fillWidth: true
                                spacing: 5
                                
                                Text {
                                    text: "Welcome, " + (authManager ? authManager.currentUser : "User")
                                    font.pixelSize: 24
                                    font.bold: true
                                    color: window.primaryColor
                                }
                                
                                Text {
                                    text: "Role: " + (authManager ? authManager.currentUserRole : "")
                                    font.pixelSize: 16
                                    color: "#666666"
                                }
                                
                                Text {
                                    text: new Date().toLocaleDateString(Qt.locale(), Locale.LongFormat)
                                    font.pixelSize: 14
                                    color: "#999999"
                                }
                            }
                            
                            Rectangle {
                                Layout.preferredWidth: 80
                                Layout.preferredHeight: 80
                                color: window.primaryColor
                                radius: 40
                                
                                Text {
                                    anchors.centerIn: parent
                                    text: "👨‍⚕️"
                                    font.pixelSize: 32
                                }
                            }
                        }
                    }
                    
                    // Quick actions
                    Rectangle {
                        width: parent.width
                        height: quickActions.implicitHeight + 40
                        color: window.surfaceColor
                        radius: 10
                        border.color: window.primaryColor
                        border.width: 1
                        
                        Column {
                            id: quickActions
                            anchors.fill: parent
                            anchors.margins: 20
                            spacing: 20
                            
                            Text {
                                text: "Quick Actions"
                                font.pixelSize: 20
                                font.bold: true
                                color: window.primaryColor
                            }
                            
                            GridLayout {
                                width: parent.width
                                columns: Math.floor(width / 250)
                                columnSpacing: 20
                                rowSpacing: 15
                                
                                TouchButton {
                                    Layout.preferredWidth: 230
                                    Layout.preferredHeight: 60
                                    text: "🧪 New Blood Gas Analysis"
                                    enabled: deviceReady && !analysisInProgress
                                    useAccentColor: true
                                    onClicked: window.navigateToView("sample")
                                }
                                
                                TouchButton {
                                    Layout.preferredWidth: 230
                                    Layout.preferredHeight: 60
                                    text: "📊 View Recent Results"
                                    onClicked: window.navigateToView("history")
                                }
                                
                                TouchButton {
                                    Layout.preferredWidth: 230
                                    Layout.preferredHeight: 60
                                    text: "⚙️ Device Calibration"
                                    onClicked: window.navigateToView("calibration")
                                }
                                
                                TouchButton {
                                    Layout.preferredWidth: 230
                                    Layout.preferredHeight: 60
                                    text: "📤 Export Data"
                                    enabled: historicalDataModel ? historicalDataModel.count > 0 : false
                                    onClicked: exportData()
                                }
                            }
                        }
                    }
                    
                    // Recent results preview
                    Rectangle {
                        width: parent.width
                        height: recentResults.implicitHeight + 40
                        color: window.surfaceColor
                        radius: 10
                        border.color: window.primaryColor
                        border.width: 1
                        
                        Column {
                            id: recentResults
                            anchors.fill: parent
                            anchors.margins: 20
                            spacing: 15
                            
                            Row {
                                spacing: 10
                                width: parent.width
                                
                                Text {
                                    text: "Recent Results"
                                    font.pixelSize: 20
                                    font.bold: true
                                    color: window.primaryColor
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                                
                                Rectangle {
                                    width: viewAllButton.implicitWidth + 20
                                    height: 30
                                    color: "transparent"
                                    border.color: window.primaryColor
                                    border.width: 1
                                    radius: 15
                                    anchors.verticalCenter: parent.verticalCenter
                                    
                                    Text {
                                        id: viewAllButton
                                        anchors.centerIn: parent
                                        text: "View All"
                                        color: window.primaryColor
                                        font.pixelSize: 12
                                    }
                                    
                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: window.navigateToView("history")
                                    }
                                }
                            }
                            
                            ListView {
                                width: parent.width
                                height: Math.min(200, contentHeight)
                                model: historicalDataModel
                                clip: true
                                
                                delegate: Rectangle {
                                    width: ListView.view.width
                                    height: 60
                                    color: index % 2 === 0 ? "#F8F9FA" : "white"
                                    radius: 5
                                    
                                    RowLayout {
                                        anchors.fill: parent
                                        anchors.margins: 10
                                        
                                        Column {
                                            Layout.fillWidth: true
                                            spacing: 2
                                            
                                            Text {
                                                text: "Sample: " + (sampleId || "N/A")
                                                font.pixelSize: 14
                                                font.bold: true
                                                color: "#333333"
                                            }
                                            
                                            Text {
                                                text: new Date(timestamp).toLocaleString(Qt.locale(), Locale.ShortFormat)
                                                font.pixelSize: 12
                                                color: "#666666"
                                            }
                                        }
                                        
                                        Row {
                                            spacing: 10
                                            
                                            Text {
                                                text: "pH: " + (pH ? pH.toFixed(2) : "N/A")
                                                font.pixelSize: 12
                                                color: "#666666"
                                            }
                                            
                                            Text {
                                                text: "pO₂: " + (pO2 ? pO2.toFixed(1) : "N/A")
                                                font.pixelSize: 12
                                                color: "#666666"
                                            }
                                            
                                            Text {
                                                text: "pCO₂: " + (pCO2 ? pCO2.toFixed(1) : "N/A")
                                                font.pixelSize: 12
                                                color: "#666666"
                                            }
                                        }
                                    }
                                }
                                
                                Text {
                                    visible: parent.count === 0
                                    anchors.centerIn: parent
                                    text: "No recent results available"
                                    font.pixelSize: 14
                                    color: "#999999"
                                }
                            }
                        }
                    }
                    
                    // System information
                    Rectangle {
                        width: parent.width
                        height: systemInfo.implicitHeight + 40
                        color: window.surfaceColor
                        radius: 10
                        border.color: window.primaryColor
                        border.width: 1
                        
                        Column {
                            id: systemInfo
                            anchors.fill: parent
                            anchors.margins: 20
                            spacing: 10
                            
                            Text {
                                text: "System Information"
                                font.pixelSize: 20
                                font.bold: true
                                color: window.primaryColor
                            }
                            
                            Grid {
                                width: parent.width
                                columns: 2
                                columnSpacing: 20
                                rowSpacing: 8
                                
                                Text {
                                    text: "Last Calibration:"
                                    font.pixelSize: 14
                                    color: "#666666"
                                }
                                
                                Text {
                                    text: calibrationManager && calibrationManager.lastCalibrationTime.toString() !== "" ?
                                          new Date(calibrationManager.lastCalibrationTime).toLocaleDateString() : "Never"
                                    font.pixelSize: 14
                                    color: "#333333"
                                }
                                
                                Text {
                                    text: "Total Results:"
                                    font.pixelSize: 14
                                    color: "#666666"
                                }
                                
                                Text {
                                    text: historicalDataModel ? historicalDataModel.count : 0
                                    font.pixelSize: 14
                                    color: "#333333"
                                }
                                
                                Text {
                                    text: "HL7 Status:"
                                    font.pixelSize: 14
                                    color: "#666666"
                                }
                                
                                Text {
                                    text: hl7Manager && hl7Manager.isConnected ? "Connected" : "Disconnected"
                                    font.pixelSize: 14
                                    color: hl7Manager && hl7Manager.isConnected ? window.successColor : window.errorColor
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Functions
    function exportData() {
        if (historicalDataModel) {
            historicalDataModel.exportToCSV("")
            window.showMessage("Data exported successfully!", "success")
        }
    }
    
    // Update when components are loaded
    Component.onCompleted: {
        if (historicalDataModel) {
            historicalDataModel.loadData()
        }
    }
}