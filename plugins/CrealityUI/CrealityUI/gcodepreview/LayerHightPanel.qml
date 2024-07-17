import QtQuick 2.13
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.13

import "../qml"

Rectangle {
  id: root

  readonly property string title: qsTr("Layer Hight")
  readonly property string unit: "mm"

  color: "transparent"

  StyledLabel {
    id: title_label

    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right

    text: "%1 (%2)".arg(root.title).arg(root.unit)
    color: "#FFFFFF" //Constants.textColor
    font.family: Constants.labelFontFamily
    font.weight: Constants.labelFontWeight
    font.pointSize: Constants.labelFontPointSize_9
  }
  ScrollView{
      ScrollBar.vertical.policy: contentHeight > height ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
      ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
      anchors.top: title_label.bottom
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.bottom: parent.bottom
      anchors.topMargin: 10 * screenScaleFactor
      clip: true
    ListView {
      clip: true
      model: kernel_slice_model.layerHightModel
      delegate: RowLayout {
        width: parent.width
        height: 20 * screenScaleFactor
        spacing: 5 * screenScaleFactor

        Rectangle {
          id: color_rect

          Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

          height: parent.height
          width: height
          color: model_color
          border.width: 1 * screenScaleFactor
        }

        Label {
          id: value_label

          Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

          text: String(model_value.toFixed(3))
          color: "#FFFFFF"
          font.family: Constants.labelFontFamily
          font.weight: Constants.labelFontWeight
          font.pointSize: Constants.labelFontPointSize_9
        }

        Item { Layout.fillWidth: true }
      }
    }
  }
}
