import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "components"


Rectangle {
    id: root
    color: "lightgrey" //window.backgroundColor

    property bool analysisInProgress: bloodGasAnalyzer ? bloodGasAnalyzer.isAnalyzing : false
    
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
                    text: "Sample Input & Analysis"
                    font.pixelSize: 24
                    font.bold: true
                    color: window.primaryColor
                    horizontalAlignment: Text.AlignHCenter
                }
                
                Rectangle {
                    Layout.preferredWidth: 120
                    Layout.preferredHeight: 40
                    color: analysisInProgress ? window.accentColor : 
                           (bloodGasAnalyzer && bloodGasAnalyzer.isCalibrated ? window.successColor : window.errorColor)
                    radius: 5
                    
                    Text {
                        anchors.centerIn: parent
                        text: analysisInProgress ? "Analyzing..." : 
                              (bloodGasAnalyzer && bloodGasAnalyzer.isCalibrated ? "Ready" : "Not Ready")
                        color: "white"
                        font.pixelSize: 12
                        font.bold: true
                    }
                }
            }
        }
        
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 20
            
            // Sample input section
            Rectangle {
                Layout.preferredWidth: 400
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
                        
                        Text {
                            text: "Sample Information"
                            font.pixelSize: 20
                            font.bold: true
                            color: window.primaryColor
                        }
                        
                        Rectangle {
                            width: parent.width
                            height: 2
                            color: window.primaryColor
                            opacity: 0.3
                        }
                        
                        // Sample ID
                        Column {
                            width: parent.width
                            spacing: 5
                            
                            Text {
                                text: "Sample ID"
                                font.pixelSize: 14
                                font.bold: true
                                color: "#666666"
                            }
                            
                            InputField {
                                id: sampleIdField
                                width: parent.width
                                text: generateSampleId()
                                placeholderText: "Sample ID"
                                icon: "🧪"
                            }
                        }
                        
                        // Patient ID
                        Column {
                            width: parent.width
                            spacing: 5
                            
                            Text {
                                text: "Patient ID (Optional)"
                                font.pixelSize: 14
                                font.bold: true
                                color: "#666666"
                            }
                            
                            InputField {
                                id: patientIdField
                                width: parent.width
                                placeholderText: "Patient ID"
                                icon: "👤"
                            }
                        }
                        
                        // Temperature
                        Column {
                            width: parent.width
                            spacing: 5
                            
                            Text {
                                text: "Temperature (°C)"
                                font.pixelSize: 14
                                font.bold: true
                                color: "#666666"
                            }
                            
                            InputField {
                                id: temperatureField
                                width: parent.width
                                text: "37.0"
                                placeholderText: "37.0"
                                icon: "🌡️"
                                validator: DoubleValidator {
                                    bottom: 30.0
                                    top: 42.0
                                    decimals: 1
                                }
                            }
                        }
                        
                        Rectangle {
                            width: parent.width
                            height: 1
                            color: "#E0E0E0"
                        }
                        
                        // Analysis type selection
                        Column {
                            width: parent.width
                            spacing: 10
                            
                            Text {
                                text: "Analysis Type"
                                font.pixelSize: 16
                                font.bold: true
                                color: "#666666"
                            }
                            
                            Column {
                                width: parent.width
                                spacing: 5

                                CheckBox {
                                    id: bloodGasCheck
                                    text: "Blood Gas (pH, pO₂, pCO₂)"
                                    checked: true
                                    font.pixelSize: 14
                                }
                                CheckBox {
                                    id: electrolyteCheck
                                    text: "Electrolytes (Na, K, Cl, Ca)"
                                    checked: true
                                    font.pixelSize: 14
                                }
                                
                                CheckBox {
                                    id: metaboliteCheck
                                    text: "Metabolites (Glucose, Lactate)"
                                    checked: true
                                    font.pixelSize: 14
                                }
                            }
                        }
                        
                        Rectangle {
                            width: parent.width
                            height: 1
                            color: "#E0E0E0"
                        }
                        
                        // Control buttons
                        Column {
                            width: parent.width
                            spacing: 15
                            
                            TouchButton {
                                width: parent.width
                                text: analysisInProgress ? "Analysis in Progress..." : "Start Analysis"
                                enabled: !analysisInProgress && canStartAnalysis()
                                useAccentColor: true
                                onClicked: startAnalysis()
                            }

                            TouchButton {
                                visible: analysisInProgress
                                width: parent.width
                                text: "Stop Analysis"
                                onClicked: stopAnalysis()
                            }
                            
                            TouchButton {
                                width: parent.width
                                text: "Clear Form"
                                enabled: !analysisInProgress
                                onClicked: clearForm()
                            }
                        }
                    }
                }
            }
            
            // Results display section
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "lightgrey" //"lightgrey" //window.surfaceColor
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
                        
                        Row {
                            spacing: 10
                            width: parent.width
                            
                            Text {
                                text: "Analysis Results"
                                font.pixelSize: 20
                                font.bold: true
                                color: window.primaryColor
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            
                            Rectangle {
                                visible: analysisInProgress
                                width: 20
                                height: 20
                                color: "transparent"
                                anchors.verticalCenter: parent.verticalCenter
                                
                                BusyIndicator {
                                    anchors.fill: parent
                                    running: analysisInProgress
                                }
                            }
                        }
                        
                        Rectangle {
                            width: parent.width
                            height: 2
                            color: window.primaryColor
                            opacity: 0.3
                        }
                        
                        // Progress indicator
                        Rectangle {
                            visible: analysisInProgress
                            width: parent.width
                            height: 60
                            color: "#F0F8FF"
                            radius: 5
                            border.color: window.primaryColor
                            border.width: 1
                            
                            Column {
                                anchors.centerIn: parent
                                spacing: 8
                                
                                Text {
                                    text: "Analysis in progress..."
                                    font.pixelSize: 16
                                    font.bold: true
                                    color: window.primaryColor
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }
                                
                                Rectangle {
                                    width: 200
                                    height: 4
                                    color: "#E0E0E0"
                                    radius: 2
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    
                                    Rectangle {
                                        width: parent.width
                                        height: parent.height
                                        color: window.primaryColor
                                        radius: parent.radius
                                        
                                        SequentialAnimation on width {
                                            running: analysisInProgress
                                            loops: Animation.Infinite
                                            PropertyAnimation { from: 0; to: 200; duration: 2000 }
                                            PropertyAnimation { from: 200; to: 0; duration: 1000 }
                                        }
                                    }
                                }
                            }
                        }
                        
                        // Results display
                        Column {
                            id: resultsColumn
                            width: parent.width
                            spacing: 15
                            visible: !analysisInProgress && resultsAvailable
                            
                            property bool resultsAvailable: false
                            property var currentResults: null
                            
                            // Blood Gas section
                            Rectangle {
                                visible: bloodGasCheck.checked && parent.resultsAvailable
                                width: parent.width
                                height: bloodGasGrid.implicitHeight + 30
                                color: "#F8F9FA"
                                radius: 8
                                border.color: "#E0E0E0"
                                border.width: 1
                                
                                Column {
                                    anchors.fill: parent
                                    anchors.margins: 15
                                    spacing: 10
                                    
                                    Text {
                                        text: "Blood Gas Parameters"
                                        font.pixelSize: 16
                                        font.bold: true
                                        color: window.primaryColor
                                    }
                                    
                                    Grid {
                                        id: bloodGasGrid
                                        width: parent.width
                                        columns: 3
                                        columnSpacing: 20
                                        rowSpacing: 10
                                        
                                        // pH
                                        Rectangle {
                                            width: (parent.width - 40) / 3
                                            height: 60
                                            color: "white"
                                            radius: 5
                                            border.color: getValueBorderColor("pH")
                                            border.width: 2
                                            
                                            Column {
                                                anchors.centerIn: parent
                                                spacing: 2
                                                
                                                Text {
                                                    text: "pH"
                                                    font.pixelSize: 12
                                                    color: "#666666"
                                                    anchors.horizontalCenter: parent.horizontalCenter
                                                }
                                                
                                                Text {
                                                    text: getResultValue("pH")
                                                    font.pixelSize: 16
                                                    font.bold: true
                                                    color: "#333333"
                                                    anchors.horizontalCenter: parent.horizontalCenter
                                                }
                                                
                                                Text {
                                                    text: "7.35-7.45"
                                                    font.pixelSize: 9
                                                    color: "#999999"
                                                    anchors.horizontalCenter: parent.horizontalCenter
                                                }
                                            }
                                        }
                                        
                                        // pO2
                                        Rectangle {
                                            width: (parent.width - 40) / 3
                                            height: 60
                                            color: "white"
                                            radius: 5
                                            border.color: getValueBorderColor("pO2")
                                            border.width: 2
                                            
                                            Column {
                                                anchors.centerIn: parent
                                                spacing: 2
                                                
                                                Text {
                                                    text: "pO₂"
                                                    font.pixelSize: 12
                                                    color: "#666666"
                                                    anchors.horizontalCenter: parent.horizontalCenter
                                                }
                                                
                                                Text {
                                                    text: getResultValue("pO2") + " mmHg"
                                                    font.pixelSize: 16
                                                    font.bold: true
                                                    color: "#333333"
                                                    anchors.horizontalCenter: parent.horizontalCenter
                                                }
                                                
                                                Text {
                                                    text: "80-100"
                                                    font.pixelSize: 9
                                                    color: "#999999"
                                                    anchors.horizontalCenter: parent.horizontalCenter
                                                }
                                            }
                                        }
                                        
                                        // pCO2
                                        Rectangle {
                                            width: (parent.width - 40) / 3
                                            height: 60
                                            color: "white"
                                            radius: 5
                                            border.color: getValueBorderColor("pCO2")
                                            border.width: 2
                                            
                                            Column {
                                                anchors.centerIn: parent
                                                spacing: 2
                                                
                                                Text {
                                                    text: "pCO₂"
                                                    font.pixelSize: 12
                                                    color: "#666666"
                                                    anchors.horizontalCenter: parent.horizontalCenter
                                                }
                                                
                                                Text {
                                                    text: getResultValue("pCO2") + " mmHg"
                                                    font.pixelSize: 16
                                                    font.bold: true
                                                    color: "#333333"
                                                    anchors.horizontalCenter: parent.horizontalCenter
                                                }
                                                
                                                Text {
                                                    text: "35-45"
                                                    font.pixelSize: 9
                                                    color: "#999999"
                                                    anchors.horizontalCenter: parent.horizontalCenter
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            
                            // Additional parameters would go here...
                            // (Electrolytes, Metabolites sections similar to Blood Gas)
                            
                            // Action buttons for results
                            Row {
                                visible: parent.resultsAvailable
                                spacing: 15
                                width: parent.width
                                
                                TouchButton {
                                    width: 150
                                    text: "Print Results"
                                    onClicked: printResults()
                                }
                                
                                TouchButton {
                                    width: 150
                                    text: "Export Results"
                                    onClicked: exportResults()
                                }
                                
                                TouchButton {
                                    width: 150
                                    text: "Send HL7"
                                    enabled: hl7Manager ? hl7Manager.isConnected : false
                                    onClicked: sendHL7()
                                }
                            }
                        }
                        
                        // No results message
                        Text {
                            visible: !analysisInProgress && !resultsColumn.resultsAvailable
                            width: parent.width
                            text: "No analysis results available.\nStart an analysis to see results here."
                            font.pixelSize: 16
                            color: "#999999"
                            horizontalAlignment: Text.AlignHCenter
                            wrapMode: Text.WordWrap
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
            }
        }
    }
    
    // Functions
    function generateSampleId() {
        var now = new Date()
        return "BGA" + now.getFullYear().toString().substr(2, 2) + 
               String(now.getMonth() + 1).padStart(2, '0') +
               String(now.getDate()).padStart(2, '0') +
               String(now.getHours()).padStart(2, '0') +
               String(now.getMinutes()).padStart(2, '0')
    }
    
    function canStartAnalysis() {
        return sampleIdField.text.length > 0 && 
               //temperatureField.acceptableInput &&
               (bloodGasCheck.checked || electrolyteCheck.checked || metaboliteCheck.checked)
    }
    
    function startAnalysis() {
        if (!bloodGasAnalyzer || analysisInProgress) return
        
        var sampleData = {
            "sampleId": sampleIdField.text,
            "patientId": patientIdField.text,
            "temperature": parseFloat(temperatureField.text),
            "bloodGas": bloodGasCheck.checked,
            "electrolytes": electrolyteCheck.checked,
            "metabolites": metaboliteCheck.checked,
            "timestamp": new Date().toISOString()
        }
        
        bloodGasAnalyzer.startAnalysis(sampleData)
        window.showMessage("Analysis started...", "info")
    }
    
    function stopAnalysis() {
        if (bloodGasAnalyzer) {
            bloodGasAnalyzer.stopAnalysis()
            window.showMessage("Analysis stopped", "warning")
        }
    }
    
    function clearForm() {
        sampleIdField.text = generateSampleId()
        patientIdField.text = ""
        temperatureField.text = "37.0"
        bloodGasCheck.checked = true
        electrolyteCheck.checked = true
        metaboliteCheck.checked = true
        resultsColumn.resultsAvailable = false
        resultsColumn.currentResults = null
    }
    
    function getResultValue(parameter) {
        if (!resultsColumn.currentResults || !resultsColumn.currentResults[parameter]) {
            return "N/A"
        }
        return resultsColumn.currentResults[parameter].toFixed(parameter === "pH" ? 3 : 1)
    }
    
    function getValueBorderColor(parameter) {
        if (!resultsColumn.currentResults || !resultsColumn.currentResults[parameter]) {
            return "#E0E0E0"
        }
        
        var value = resultsColumn.currentResults[parameter]
        var normal = false
        
        switch(parameter) {
            case "pH":
                normal = value >= 7.35 && value <= 7.45
                break
            case "pO2":
                normal = value >= 80 && value <= 100
                break
            case "pCO2":
                normal = value >= 35 && value <= 45
                break
        }
        
        return normal ? window.successColor : window.errorColor
    }
    
    function printResults() {
        window.showMessage("Print functionality not implemented in demo", "info")
    }
    
    function exportResults() {
        if (bloodGasAnalyzer) {
            bloodGasAnalyzer.exportResults("PDF")
            window.showMessage("Results exported", "success")
        }
    }
    
    function sendHL7() {
        if (hl7Manager && resultsColumn.currentResults) {
            hl7Manager.sendResults(resultsColumn.currentResults)
            window.showMessage("Results sent via HL7", "success")
        }
    }
    
    // Handle analysis completion
    Connections {
        target: bloodGasAnalyzer
        function onAnalysisCompleted(results) {
            resultsColumn.currentResults = results
            resultsColumn.resultsAvailable = true
            window.showMessage("Analysis completed successfully!", "success")
        }
        function onAnalysisError(error) {
            resultsColumn.resultsAvailable = false
            window.showMessage("Analysis error: " + error, "error")
        }
        function onAnalysisStopped() {
            window.showMessage("Analysis stopped", "warning")
        }
        function onCalibrationStatusChanged(isCalibrated) {
            if (isCalibrated) {
                window.showMessage("Device calibrated and ready", "success")
            } else {
                window.showMessage("Device not calibrated", "error")
            }
        }
        function onHL7StatusChanged(isConnected) {
            if (isConnected) {
                window.showMessage("HL7 connection established", "success")
            }
            else {
                window.showMessage("HL7 connection lost", "error")
            }
        }
        function onExportCompleted(format) {
            window.showMessage("Results exported as " + format, "success")
        }
        function onExportError(error) {
            window.showMessage("Export error: " + error, "error")
        }
        function onResultsSent() {
            window.showMessage("Results sent successfully", "success")
        }
        function onResultsSendError(error) {
            window.showMessage("Error sending results: " + error, "error")
        }
        function onIsAnalyzingChanged(isAnalyzing) {
            window.showMessage("Analysis " + (isAnalyzing ? "started" : "stopped"), isAnalyzing ? "info" : "warning")
        }
    }

    // Initialize
    Component.onCompleted: {
        clearForm()
    }
}
