import QtQuick
import QtQuick.Controls

Button {
    id: control
    
    property color primaryColor: window ? window.primaryColor : "#2E86AB"
    property color hoverColor: Qt.lighter(primaryColor, 1.1)
    property color pressedColor: Qt.darker(primaryColor, 1.2)
    property color disabledColor: "#CCCCCC"
    property int fontSize: 16
    property bool useAccentColor: false
    
    // Make touch-friendly
    implicitHeight: 50
    
    background: Rectangle {
        color: control.enabled ? 
                  (control.pressed ? pressedColor : 
                   control.hovered ? hoverColor : 
                   (useAccentColor ? (window ? window.accentColor : "#F18F01") : primaryColor)) :
                  disabledColor
        radius: 8
        border.width: control.enabled ? (control.activeFocus ? 2 : 0) : 0
        border.color: control.enabled ? Qt.lighter(color, 1.3) : "transparent"
        
        // Ripple effect
        Rectangle {
            id: ripple
            anchors.centerIn: parent
            width: 0
            height: 0
            radius: width / 2
            color: Qt.rgba(255, 255, 255, 0.3)
            visible: control.pressed
            
            PropertyAnimation on width {
                running: control.pressed
                from: 0
                to: Math.max(control.width, control.height) * 1.5
                duration: 300
                easing.type: Easing.OutQuad
            }
            
            PropertyAnimation on height {
                running: control.pressed
                from: 0
                to: Math.max(control.width, control.height) * 1.5
                duration: 300
                easing.type: Easing.OutQuad
            }
        }
        
        // Subtle shadow
        Rectangle {
            anchors.fill: parent
            anchors.topMargin: 2
            color: Qt.rgba(0, 0, 0, 0.1)
            radius: parent.radius
            z: -1
            visible: control.enabled && !control.pressed
        }
    }
    
    contentItem: Text {
        text: control.text
        color: control.enabled ? "white" : "#999999"
        font.pixelSize: fontSize
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
    
    // Accessibility
    Accessible.role: Accessible.Button
    Accessible.name: text
    Accessible.description: "Touch button: " + text
    
    // Touch feedback
    scale: pressed ? 0.98 : 1.0
    Behavior on scale {
        NumberAnimation {
            duration: 100
            easing.type: Easing.OutQuad
        }
    }
}