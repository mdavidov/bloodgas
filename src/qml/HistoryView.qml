import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "components"


Rectangle {
    id: root
    color: "lightgrey" //window.backgroundColor

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
                    text: "Historical Results"
                    font.pixelSize: 24
                    font.bold: true
                    color: window.primaryColor
                    horizontalAlignment: Text.AlignHCenter
                }
                
                TouchButton {
                    text: "Export CSV"
                    enabled: historicalDataModel ? historicalDataModel.count > 0 : false
                    onClicked: exportData()
                }
            }
        }
        
        // Filter section
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            color: "lightgrey" //window.surfaceColor
            radius: 10
            border.color: "#E0E0E0"
            border.width: 1
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 15
                
                InputField {
                    id: patientFilter
                    Layout.preferredWidth: 150
                    placeholderText: "Patient ID"
                    onTextChanged: applyFilters()
                }
                
                InputField {
                    id: operatorFilter
                    Layout.preferredWidth: 150
                    placeholderText: "Operator"
                    onTextChanged: applyFilters()
                }
                
                TouchButton {
                    text: "Clear Filters"
                    onClicked: clearFilters()
                }
                
                Item {
                    Layout.fillWidth: true
                }
                
                Text {
                    text: "Total: " + (historicalDataModel ? historicalDataModel.count : 0) + " results"
                    font.pixelSize: 14
                    color: "#666666"
                }
            }
        }
        
        // Results list
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "lightgrey" //window.surfaceColor
            radius: 10
            border.color: "#E0E0E0"
            border.width: 1
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 10
                
                // Column headers
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    color: window.primaryColor
                    radius: 5
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 5
                        
                        Text {
                            Layout.preferredWidth: 100
                            text: "Date/Time"
                            color: "white"
                            font.pixelSize: 12
                            font.bold: true
                        }
                        
                        Text {
                            Layout.preferredWidth: 80
                            text: "Sample ID"
                            color: "white"
                            font.pixelSize: 12
                            font.bold: true
                        }
                        
                        Text {
                            Layout.preferredWidth: 80
                            text: "Patient ID"
                            color: "white"
                            font.pixelSize: 12
                            font.bold: true
                        }
                        
                        Text {
                            Layout.preferredWidth: 60
                            text: "pH"
                            color: "white"
                            font.pixelSize: 12
                            font.bold: true
                        }
                        
                        Text {
                            Layout.preferredWidth: 60
                            text: "pO₂"
                            color: "white"
                            font.pixelSize: 12
                            font.bold: true
                        }
                        
                        Text {
                            Layout.preferredWidth: 60
                            text: "pCO₂"
                            color: "white"
                            font.pixelSize: 12
                            font.bold: true
                        }
                        
                        Text {
                            Layout.preferredWidth: 60
                            text: "HCO₃"
                            color: "white"
                            font.pixelSize: 12
                            font.bold: true
                        }
                        
                        Text {
                            Layout.fillWidth: true
                            text: "Operator"
                            color: "white"
                            font.pixelSize: 12
                            font.bold: true
                        }
                    }
                }
                
                // Results ListView
                ListView {
                    id: resultsList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: historicalDataModel
                    
                    delegate: Rectangle {
                        width: resultsList.width
                        height: 60
                        color: index % 2 === 0 ? "#F8F9FA" : "white"
                        radius: 5
                        
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 10
                            spacing: 5
                            
                            Column {
                                Layout.preferredWidth: 100
                                spacing: 2
                                
                                Text {
                                    text: new Date(timestamp).toLocaleDateString()
                                    font.pixelSize: 11
                                    font.bold: true
                                    color: "#333333"
                                    width: parent.width
                                    elide: Text.ElideRight
                                }
                                
                                Text {
                                    text: new Date(timestamp).toLocaleTimeString()
                                    font.pixelSize: 9
                                    color: "#666666"
                                    width: parent.width
                                    elide: Text.ElideRight
                                }
                            }
                            
                            Text {
                                Layout.preferredWidth: 80
                                text: sampleId || "N/A"
                                font.pixelSize: 11
                                color: "#333333"
                                elide: Text.ElideRight
                            }
                            
                            Text {
                                Layout.preferredWidth: 80
                                text: patientId || "N/A"
                                font.pixelSize: 11
                                color: "#333333"
                                elide: Text.ElideRight
                            }
                            
                            Text {
                                Layout.preferredWidth: 60
                                text: pH ? pH.toFixed(2) : "N/A"
                                font.pixelSize: 11
                                color: getValueColor(pH, 7.35, 7.45)
                                horizontalAlignment: Text.AlignHCenter
                            }
                            
                            Text {
                                Layout.preferredWidth: 60
                                text: pO2 ? pO2.toFixed(1) : "N/A"
                                font.pixelSize: 11
                                color: getValueColor(pO2, 80, 100)
                                horizontalAlignment: Text.AlignHCenter
                            }
                            
                            Text {
                                Layout.preferredWidth: 60
                                text: pCO2 ? pCO2.toFixed(1) : "N/A"
                                font.pixelSize: 11
                                color: getValueColor(pCO2, 35, 45)
                                horizontalAlignment: Text.AlignHCenter
                            }
                            
                            Text {
                                Layout.preferredWidth: 60
                                text: HCO3 ? HCO3.toFixed(1) : "N/A"
                                font.pixelSize: 11
                                color: getValueColor(HCO3, 22, 28)
                                horizontalAlignment: Text.AlignHCenter
                            }
                            
                            Text {
                                Layout.fillWidth: true
                                text: operator || "N/A"
                                font.pixelSize: 11
                                color: "#666666"
                                elide: Text.ElideRight
                            }
                        }
                        
                        MouseArea {
                            anchors.fill: parent
                            onClicked: showResultDetails(index)
                        }
                    }
                    
                    // Empty state
                    Text {
                        visible: resultsList.count === 0
                        anchors.centerIn: parent
                        text: "No results found"
                        font.pixelSize: 16
                        color: "#999999"
                    }
                }
            }
        }
    }
    
    // Result details dialog
    Rectangle {
        id: detailsDialog
        anchors.centerIn: parent
        width: 500
        height: 600
        color: "white"
        radius: 10
        border.color: window.primaryColor
        border.width: 2
        visible: false
        z: 1000
        
        // Background overlay
        Rectangle {
            anchors.fill: parent
            anchors.margins: -1000
            color: Qt.rgba(0, 0, 0, 0.5)
            z: -1
            
            MouseArea {
                anchors.fill: parent
                onClicked: detailsDialog.visible = false
            }
        }
        
        Column {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 15
            
            // Header
            Row {
                width: parent.width
                spacing: 10
                
                Text {
                    text: "Result Details"
                    font.pixelSize: 20
                    font.bold: true
                    color: window.primaryColor
                    anchors.verticalCenter: parent.verticalCenter
                }
                
                Item {
                    width: parent.width - parent.children[0].width - closeButton.width - 20
                    height: 1
                }
                
                TouchButton {
                    id: closeButton
                    width: 60
                    height: 30
                    text: "Close"
                    onClicked: detailsDialog.visible = false
                }
            }
            
            Rectangle {
                width: parent.width
                height: 2
                color: window.primaryColor
                opacity: 0.3
            }
            
            // Details content
            ScrollView {
                width: parent.width
                height: parent.height - 100
                clip: true
                
                Column {
                    id: detailsContent
                    width: parent.width
                    spacing: 10
                    
                    property var currentResult: null
                    
                    // This would be populated when showing details
                }
            }
        }
    }
    
    // Functions
    function getValueColor(value, normalMin, normalMax) {
        if (!value) return "#999999"
        if (value < normalMin || value > normalMax) {
            return window.errorColor
        }
        return "#333333"
    }
    
    function applyFilters() {
        if (!historicalDataModel) return
        
        var patientFilter = patientFilter.text.trim()
        var operatorFilter = operatorFilter.text.trim()
        
        if (patientFilter.length > 0) {
            historicalDataModel.filterByPatient(patientFilter)
        }
        if (operatorFilter.length > 0) {
            historicalDataModel.filterByOperator(operatorFilter)
        }
        if (patientFilter.length === 0 && operatorFilter.length === 0) {
            historicalDataModel.clearFilters()
        }
    }
    
    function clearFilters() {
        patientFilter.text = ""
        operatorFilter.text = ""
        if (historicalDataModel) {
            historicalDataModel.clearFilters()
        }
    }
    
    function exportData() {
        if (historicalDataModel) {
            historicalDataModel.exportToCSV("")
            window.showMessage("Data exported to Documents folder", "success")
        }
    }
    
    function showResultDetails(index) {
        if (!historicalDataModel) return
        
        var result = historicalDataModel.getResult(index)
        if (!result) return
        
        // Clear previous content
        for (var i = detailsContent.children.length - 1; i >= 0; i--) {
            detailsContent.children[i].destroy()
        }
        
        // Create detail items
        var detailItems = [
            {label: "Sample ID", value: result.sampleId || "N/A"},
            {label: "Patient ID", value: result.patientId || "N/A"},
            {label: "Timestamp", value: new Date(result.timestamp).toLocaleString()},
            {label: "Operator", value: result.operator || "N/A"},
            {label: "pH", value: result.pH ? result.pH.toFixed(3) : "N/A"},
            {label: "pO₂ (mmHg)", value: result.pO2 ? result.pO2.toFixed(1) : "N/A"},
            {label: "pCO₂ (mmHg)", value: result.pCO2 ? result.pCO2.toFixed(1) : "N/A"},
            {label: "HCO₃ (mmol/L)", value: result.HCO3 ? result.HCO3.toFixed(1) : "N/A"},
            {label: "SO₂ (%)", value: result.SO2 ? result.SO2.toFixed(1) : "N/A"},
            {label: "BE (mmol/L)", value: result.BE ? result.BE.toFixed(1) : "N/A"},
            {label: "Na (mmol/L)", value: result.Na ? result.Na.toFixed(1) : "N/A"},
            {label: "K (mmol/L)", value: result.K ? result.K.toFixed(2) : "N/A"},
            {label: "Cl (mmol/L)", value: result.Cl ? result.Cl.toFixed(1) : "N/A"},
            {label: "Ca (mmol/L)", value: result.Ca ? result.Ca.toFixed(2) : "N/A"},
            {label: "Glucose (mg/dL)", value: result.Glucose ? result.Glucose.toFixed(1) : "N/A"},
            {label: "Lactate (mmol/L)", value: result.Lactate ? result.Lactate.toFixed(1) : "N/A"},
            {label: "Temperature (°C)", value: result.temperature ? result.temperature.toFixed(1) : "N/A"}
        ]
        
        // Create UI elements for each detail
        for (var j = 0; j < detailItems.length; j++) {
            var item = detailItems[j]
            var component = Qt.createQmlObject('
                import QtQuick
                Rectangle {
                    width: parent.width
                    height: 40
                    color: index % 2 === 0 ? "#F8F9FA" : "white"
                    radius: 5
                    property int index: ' + j + '
                    Row {
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.margins: 10
                        spacing: 10
                        Text {
                            text: "' + item.label + ':"
                            font.pixelSize: 12
                            font.bold: true
                            color: "#666666"
                            width: 120
                        }
                        Text {
                            text: "' + item.value + '"
                            font.pixelSize: 12
                            color: "#333333"
                        }
                    }
                }', detailsContent)
        }
        
        detailsDialog.visible = true
    }
    
    // Initialize
    Component.onCompleted: {
        if (historicalDataModel) {
            historicalDataModel.loadData()
        }
    }
}
