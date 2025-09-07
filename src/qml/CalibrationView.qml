import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "components"


Rectangle {
    id: root
    color: "lightgrey" //window.backgroundColor

    property bool calibrationInProgress: calibrationManager ? calibrationManager.isCalibrating : false
    property int calibrationProgress: calibrationManager ? calibrationManager.calibrationProgress : 0
    property string currentStep: calibrationManager ? calibrationManager.calibrationStep : ""
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20
        
        // Header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            color: "lightgrey" //window.surfaceColor
            radius: 10
            border.color: window.primaryColor
            border.width: 2
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 20
                
                TouchButton {
                    text: "← Back"
                    onClicked: stackView.pop()
                }
                
                Text {
                    Layout.fillWidth: true
                    text: "Device Calibration"
                    font.pixelSize: 24
                    font.bold: true
                    color: window.primaryColor
                    horizontalAlignment: Text.AlignHCenter
                }
                
                Rectangle {
                    Layout.preferredWidth: 100
                    Layout.preferredHeight: 40
                    color: calibrationInProgress ? window.accentColor : 
                           (calibrationManager && calibrationManager.isCalibrated ? window.successColor : window.errorColor)
                    radius: 5
                    
                    Text {
                        anchors.centerIn: parent
                        text: calibrationInProgress ? "In Progress" : 
                              (calibrationManager && calibrationManager.isCalibrated ? "Calibrated" : "Required")
                        color: "white"
                        font.pixelSize: 12
                        font.bold: true
                    }
                }
            }
        }
        
        // Calibration content
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "lightgrey" //window.surfaceColor
            radius: 10
            border.color: window.primaryColor
            border.width: 1
            
            ScrollView {
                anchors.fill: parent
                anchors.margins: 20
                clip: true
                
                Column {
                    width: parent.width
                    spacing: 20
                    
                    // Progress indicator
                    Rectangle {
                        width: parent.width
                        height: 100
                        color: "#F8F9FA"
                        radius: 10
                        border.color: "#E0E0E0"
                        border.width: 1
                        
                        Column {
                            anchors.centerIn: parent
                            spacing: 10
                            
                            Text {
                                text: calibrationInProgress ? "Calibration Progress: " + calibrationProgress + "%" : "Ready to Calibrate"
                                font.pixelSize: 18
                                font.bold: true
                                color: window.primaryColor
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                            
                            Rectangle {
                                width: 300
                                height: 20
                                color: "#E0E0E0"
                                radius: 10
                                anchors.horizontalCenter: parent.horizontalCenter
                                
                                Rectangle {
                                    width: parent.width * (calibrationProgress / 100.0)
                                    height: parent.height
                                    color: window.successColor
                                    radius: parent.radius
                                    
                                    Behavior on width {
                                        NumberAnimation {
                                            duration: 300
                                            easing.type: Easing.OutQuad
                                        }
                                    }
                                }
                            }
                            
                            Text {
                                visible: calibrationInProgress
                                text: "Current Step: " + currentStep
                                font.pixelSize: 14
                                color: "#666666"
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                        }
                    }
                    
                    // Calibration controls
                    Rectangle {
                        width: parent.width
                        height: calibrationControls.implicitHeight + 40
                        color: "white"
                        radius: 10
                        border.color: "#E0E0E0"
                        border.width: 1
                        
                        Column {
                            id: calibrationControls
                            anchors.centerIn: parent
                            width: parent.width - 40
                            spacing: 20
                            
                            Text {
                                text: "Calibration Options"
                                font.pixelSize: 20
                                font.bold: true
                                color: window.primaryColor
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                            
                            GridLayout {
                                width: parent.width
                                columns: 2
                                columnSpacing: 20
                                rowSpacing: 15
                                
                                TouchButton {
                                    Layout.fillWidth: true
                                    text: "Full Calibration"
                                    enabled: !calibrationInProgress
                                    useAccentColor: true
                                    onClicked: startCalibration("full")
                                }
                                
                                TouchButton {
                                    Layout.fillWidth: true
                                    text: "pH Only"
                                    enabled: !calibrationInProgress
                                    onClicked: startCalibration("ph_only")
                                }
                                
                                TouchButton {
                                    Layout.fillWidth: true
                                    text: "Gas Sensors"
                                    enabled: !calibrationInProgress
                                    onClicked: startCalibration("gas_only")
                                }
                                
                                TouchButton {
                                    Layout.fillWidth: true
                                    text: "Electrolytes"
                                    enabled: !calibrationInProgress
                                    onClicked: startCalibration("electrolytes")
                                }
                            }
                            
                            TouchButton {
                                visible: calibrationInProgress
                                width: 200
                                text: "Cancel Calibration"
                                anchors.horizontalCenter: parent.horizontalCenter
                                onClicked: cancelCalibration()
                            }
                        }
                    }
                    
                    // Calibration history
                    Rectangle {
                        width: parent.width
                        height: Math.min(300, historyList.contentHeight + 80)
                        color: "white"
                        radius: 10
                        border.color: "#E0E0E0"
                        border.width: 1
                        
                        Column {
                            anchors.fill: parent
                            anchors.margins: 20
                            spacing: 10
                            
                            Text {
                                text: "Calibration History"
                                font.pixelSize: 18
                                font.bold: true
                                color: window.primaryColor
                            }
                            
                            ListView {
                                id: historyList
                                width: parent.width
                                height: parent.height - parent.spacing - 30
                                clip: true
                                
                                model: ListModel {
                                    // Placeholder data - in real app this would come from database
                                    ListElement {
                                        date: "2024-01-15"
                                        type: "Full Calibration"
                                        status: "Success"
                                        operator: "admin"
                                    }
                                    ListElement {
                                        date: "2024-01-10"
                                        type: "pH Only"
                                        status: "Success" 
                                        operator: "operator"
                                    }
                                }
                                
                                delegate: Rectangle {
                                    width: historyList.width
                                    height: 50
                                    color: index % 2 === 0 ? "#F8F9FA" : "white"
                                    radius: 5
                                    
                                    RowLayout {
                                        anchors.fill: parent
                                        anchors.margins: 10
                                        
                                        Text {
                                            text: date
                                            font.pixelSize: 12
                                            color: "#666666"
                                            Layout.preferredWidth: 80
                                        }
                                        
                                        Text {
                                            text: type
                                            font.pixelSize: 12
                                            font.bold: true
                                            Layout.fillWidth: true
                                        }
                                        
                                        Rectangle {
                                            Layout.preferredWidth: 60
                                            Layout.preferredHeight: 20
                                            color: status === "Success" ? window.successColor : window.errorColor
                                            radius: 10
                                            
                                            Text {
                                                anchors.centerIn: parent
                                                text: status
                                                color: "white"
                                                font.pixelSize: 10
                                                font.bold: true
                                            }
                                        }
                                        
                                        Text {
                                            text: operator
                                            font.pixelSize: 10
                                            color: "#999999"
                                            Layout.preferredWidth: 60
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Functions
    function startCalibration(type) {
        if (calibrationManager) {
            calibrationManager.startCalibration(type)
        }
    }
    
    function cancelCalibration() {
        if (calibrationManager) {
            calibrationManager.cancelCalibration()
        }
    }
    
    // Handle calibration manager signals
    Connections {
        target: calibrationManager
        function onCalibrationCompleted(success) {
            if (success) {
                window.showMessage("Calibration completed successfully!", "success")
            } else {
                window.showMessage("Calibration failed. Please try again.", "error")
            }
        }
        function onCalibrationFailed(reason) {
            window.showMessage("Calibration failed: " + reason, "error")
        }
    }
}
