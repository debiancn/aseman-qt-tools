import QtQuick 2.0
import QtQuick.Controls.Styles 1.1
import QtGraphicalEffects 1.0
import AsemanTools 1.0

CheckBoxStyle {

    property color backgroundColor: masterPalette.base

    SystemPalette {
        id: masterPalette
    }

    indicator: Item {
        implicitWidth: 24*Devices.density
        implicitHeight: 24*Devices.density

        Item {
            id: frame
            anchors.fill: parent
            anchors.margins: 1

            Rectangle {
                anchors.fill: parent
                anchors.margins: 1*Devices.density
                radius: 5*Devices.density
                color: backgroundColor
            }
        }

        InnerShadow {
            anchors.fill: source
            source: frame
            radius: 4.0*Devices.density
            samples: 16
            horizontalOffset: 0
            verticalOffset: 1*Devices.density
            opacity: control.pressed? 0.6 : 0.4
            color: "#000000"
        }

        Image {
            anchors.fill: parent
            anchors.margins: 5*Devices.density
            source: "images/yes.png"
            sourceSize: Qt.size(width,height)
            fillMode: Image.PreserveAspectCrop
            visible: control.checked
        }
    }

    label: Text {
        color: masterPalette.windowText
        text: control.text
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }
}

