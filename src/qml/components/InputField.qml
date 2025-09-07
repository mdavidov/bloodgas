import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    
    property alias text: textInput.text
    property alias placeholderText: placeholder.text
    property alias echoMode: textInput.echoMode
    property alias validator: textInput.validator
    property alias inputMethodHints: textInput.inputMethodHints
    // property alias focus: textInput.focus
    property alias readOnly: textInput.readOnly
    property string icon: ""
    property color borderColor: window ? window.primaryColor : "#2E86AB"
    property color focusColor: Qt.lighter(borderColor, 1.2)
    property color backgroundColor: "white"
    property int fontSize: 16
    
    signal accepted()
    // signal textChanged()
    
    // Touch-friendly sizing
    implicitHeight: 50
    
    color: backgroundColor
    border.width: textInput.activeFocus ? 2 : 1
    border.color: textInput.activeFocus ? focusColor : borderColor
    radius: 8
    
    RowLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 10
        
        // Icon
        Text {
            visible: icon.length > 0
            text: icon
            font.pixelSize: 20
            color: borderColor
            Layout.preferredWidth: visible ? implicitWidth : 0
        }
        
        // Text input area
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            // Placeholder text
            Text {
                id: placeholder
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                color: "#999999"
                font.pixelSize: fontSize
                visible: textInput.text.length === 0 && !textInput.activeFocus
            }
            
            // Text input
            TextInput {
                id: textInput
                anchors.fill: parent
                font.pixelSize: fontSize
                color: "#333333"
                verticalAlignment: TextInput.AlignVCenter
                selectByMouse: true
                selectedTextColor: "white"
                selectionColor: borderColor
                
                // Touch-friendly cursor
                cursorDelegate: Rectangle {
                    width: 2
                    color: borderColor
                    
                    SequentialAnimation on opacity {
                        loops: Animation.Infinite
                        running: textInput.cursorVisible
                        PropertyAnimation { to: 1; duration: 500 }
                        PropertyAnimation { to: 0; duration: 500 }
                    }
                }
                
                onAccepted: root.accepted()
                onTextChanged: root.textChanged()
                
                // Handle touch events
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        textInput.forceActiveFocus()
                        textInput.cursorPosition = textInput.positionAt(mouse.x, mouse.y)
                    }
                    onPressAndHold: {
                        textInput.selectAll()
                    }
                }
            }
        }
        
        // Clear button
        Rectangle {
            visible: textInput.text.length > 0 && !textInput.readOnly
            width: 24
            height: 24
            color: "transparent"
            radius: 12
            border.width: 1
            border.color: "#CCCCCC"
            Layout.preferredWidth: visible ? 24 : 0
            Layout.preferredHeight: visible ? 24 : 0
            
            Text {
                anchors.centerIn: parent
                text: "×"
                color: "#666666"
                font.pixelSize: 16
                font.bold: true
            }
            
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    textInput.text = ""
                    textInput.forceActiveFocus()
                }
            }
        }
    }
    
    // Focus indicator
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.width: 2
        border.color: focusColor
        radius: parent.radius
        opacity: textInput.activeFocus ? 0.3 : 0
        
        Behavior on opacity {
            NumberAnimation {
                duration: 150
                easing.type: Easing.OutQuad
            }
        }
    }
    
    // Validation state indicator
    Rectangle {
        visible: textInput.text.length > 0 && !textInput.acceptableInput && validator
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 4
        width: 8
        height: 8
        color: window ? window.errorColor : "#E63946"
        radius: 4
    }
    
    // Accessibility
    Accessible.role: Accessible.EditableText
    Accessible.name: placeholderText
    Accessible.description: "Input field for " + placeholderText
    
    // Animation for border color changes
    Behavior on border.color {
        ColorAnimation {
            duration: 150
            easing.type: Easing.OutQuad
        }
    }
    
    // Functions
    function selectAll() {
        textInput.selectAll()
    }
    
    function clear() {
        textInput.text = ""
    }
    
    function paste() {
        textInput.paste()
    }
}