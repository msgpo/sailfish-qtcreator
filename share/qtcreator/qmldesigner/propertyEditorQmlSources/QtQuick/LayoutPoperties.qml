/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

import QtQuick 2.0
import HelperWidgets 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 1.0 as Controls

SectionLayout {
    property bool isInModel: backendValue.isInModel;
    property bool isInSubState: backendValue.isInSubState;
    property variant valueFromBackend: backendValue.value
    property variant backendValue: backendValues.Layout_alignment
    property bool blockAlignment: false

    onIsInModelChanged: evaluateAlignment()
    onIsInSubStateChanged: evaluateAlignment()
    onBackendValueChanged: evaluateAlignment()
    onValueFromBackendChanged: evaluateAlignment()

    property int spinBoxWidth: 62

    Connections {
        target: modelNodeBackend
        onSelectionChanged: {
            evaluateAlignment()
        }
    }

    id: root

    Component.onCompleted: evaluateAlignment()

    function indexOfVerticalAlignment() {
        if (backendValue.expression === undefined)
            return 0;

        if (backendValue.expression.indexOf("AlignVCenter") !== -1)
            return 0;

        if (backendValue.expression.indexOf("AlignTop") !== -1)
            return 1;

        if (backendValue.expression.indexOf("AlignBottom") !== -1)
            return 2;

        if (backendValue.expression.indexOf("AlignBaseline") !== -1)
            return 3;

        return 0;

    }

    function indexOfHorizontalAlignment() {

        if (backendValue.expression === undefined)
            return 0;

        if (backendValue.expression.indexOf("AlignLeft") !== -1)
            return 0;

        if (backendValue.expression.indexOf("AlignHCenter") !== -1)
            return 1;

        if (backendValue.expression.indexOf("AlignRight") !== -1)
            return 2;

        return 0;

    }

    function evaluateAlignment() {

        blockAlignment = true

        verticalAlignmentComboBox.currentIndex = indexOfVerticalAlignment();
        horizontalAlignmentComboBox.currentIndex = indexOfHorizontalAlignment();

        blockAlignment = false
    }

    function composeExpressionString() {
        if (blockAlignment)
            return;

        var expressionStr = "";
        if (horizontalAlignmentComboBox.currentIndex !== 0
                || verticalAlignmentComboBox.currentIndex !== 0) {
            expressionStr = "Qt." + horizontalAlignmentComboBox.currentText
                    + " | "
                    + "Qt." + verticalAlignmentComboBox.currentText

            backendValue.expression = expressionStr
        } else {
            expressionStr = "Qt.AlignLeft | Qt.AlignVCenter";
            backendValue.expression = expressionStr
            backendValue.resetValue();
        }

    }


    Label {
        text: qsTr("Alignment")
        tooltip: qsTr("Alignment of an item within the cells it occupies.")
    }

    SecondColumnLayout {

        Controls.ComboBox {
            ColorLogic {
                id: colorLogic
            }

            ExtendedFunctionButton {
                x: 2
                anchors.verticalCenter: parent.verticalCenter
                backendValue: backendValues.Layout_alignment
                visible: horizontalAlignmentComboBox.enabled
                onReseted:  {
                    horizontalAlignmentComboBox.currentIndex = 0
                    verticalAlignmentComboBox.currentIndex = 0
                }
            }

            id: horizontalAlignmentComboBox

            property bool __isCompleted: false
            property color textColor: currentIndex === 0 ? colorLogic.__defaultTextColor : colorLogic.__changedTextColor

            model: ["AlignLeft", "AlignHCenter", "AlignRight"]

            onCurrentIndexChanged: {
                if (!horizontalAlignmentComboBox.__isCompleted)
                    return;

                composeExpressionString();
            }

            style: CustomComboBoxStyle {
                textColor: horizontalAlignmentComboBox.textColor
            }

            Component.onCompleted: {
                horizontalAlignmentComboBox.__isCompleted = true;
            }

        }

        ExpandingSpacer {

        }

    }

    Label {

    }

    SecondColumnLayout {

        Controls.ComboBox {
            id: verticalAlignmentComboBox

            ExtendedFunctionButton {
                x: 2
                anchors.verticalCenter: parent.verticalCenter
                backendValue: backendValues.Layout_alignment
                visible: verticalAlignmentComboBox.enabled
                onReseted:  {
                    horizontalAlignmentComboBox.currentIndex = 0
                    verticalAlignmentComboBox.currentIndex = 0
                }
            }

            property bool __isCompleted: false
            property color textColor: currentIndex === 0 ? colorLogic.__defaultTextColor : colorLogic.__changedTextColor

            model: ["AlignVCenter", "AlignTop","AlignBottom","AlignBaseline"]

            onCurrentIndexChanged: {
                if (!verticalAlignmentComboBox.__isCompleted)
                    return;

                composeExpressionString();
            }

            style: CustomComboBoxStyle {
                textColor: verticalAlignmentComboBox.textColor
            }

            Component.onCompleted: {
                verticalAlignmentComboBox.__isCompleted = true;
            }

        }

        ExpandingSpacer {

        }

    }

    Label {
        text: qsTr("Fill layout")
        tooltip: qsTr("The item will expand as much as possible while respecting the given constraints if true.")
    }

    SecondColumnLayout {
        CheckBox {
            backendValue: backendValues.Layout_fillWidth
            text: qsTr("Fill width")
        }

        Item {
            width: 10
            height: 10

        }

        CheckBox {
            backendValue: backendValues.Layout_fillHeight
            text: qsTr("Fill height")
        }

        ExpandingSpacer {

        }
    }

    Label {
        text: qsTr("Preferred size")
        tooltip: qsTr("Preferred size of an item in a layout. If the preferred height or width is -1, it is ignored.")
    }

    SecondColumnLayout {
        Layout.fillWidth: true

        Label {
            text: "W"
            width: 28
        }

        SpinBox {
            backendValue: backendValues.Layout_preferredWidth
            maximumValue: 0xffff
            minimumValue: -1
            decimals: 0
            implicitWidth: root.spinBoxWidth
        }

        Item {
            width: 4
            height: 4
        }

        Label {
            text: "H"
            width: 28
        }

        SpinBox {
            backendValue: backendValues.Layout_preferredHeight
            maximumValue: 0xffff
            minimumValue: -1
            decimals: 0
            implicitWidth: root.spinBoxWidth
        }
        ExpandingSpacer {

        }
    }

    Label {
        text: qsTr("Minimum size")
        tooltip: qsTr("Minimum size of an item in a layout.")
    }

    SecondColumnLayout {
        Layout.fillWidth: true

        Label {
            text: "W"
            width: 28
        }

        SpinBox {
            backendValue: backendValues.Layout_minimumWidth
            maximumValue: 0xffff
            minimumValue: 0
            decimals: 0
            implicitWidth: root.spinBoxWidth
        }

        Item {
            width: 4
            height: 4
        }

        Label {
            text: "H"
            width: 28
        }

        SpinBox {
            backendValue: backendValues.Layout_minimumHeight
            maximumValue: 0xffff
            minimumValue: 0
            decimals: 0
            implicitWidth: root.spinBoxWidth
        }
        ExpandingSpacer {

        }
    }

    Label {
        text: qsTr("Maximum size")
        tooltip: qsTr("Maximum size of an item in a layout.")
    }

    SecondColumnLayout {
        Layout.fillWidth: true

        Label {
            text: "W"
            width: 28
        }

        SpinBox {
            backendValue: backendValues.Layout_maximumWidth
            maximumValue: 0xffff
            minimumValue: 0
            decimals: 0
            implicitWidth: root.spinBoxWidth
        }

        Item {
            width: 4
            height: 4
        }

        Label {
            text: "H"
             width: 28
        }

        SpinBox {
            backendValue: backendValues.Layout_maximumHeight
            maximumValue: 0xffff
            minimumValue: 0
            decimals: 0
            implicitWidth: root.spinBoxWidth
        }
        ExpandingSpacer {

        }
    }

    Label {
        text: qsTr("Margins")
    }

    SecondColumnLayout {
        Layout.fillWidth: true

        Label {
            text: "Top"
            width: 28
        }

        SpinBox {
            backendValue: backendValues.Layout_topMargin
            maximumValue: 0xffff
            minimumValue: 0
            decimals: 0
            implicitWidth: root.spinBoxWidth
        }

        Item {
            width: 4
            height: 4
        }

        Label {
            text: "Bottom"
            width: 28
        }

        SpinBox {
            backendValue: backendValues.Layout_bottomMargin
            maximumValue: 0xffff
            minimumValue: 0
            decimals: 0
            implicitWidth: root.spinBoxWidth
        }
        ExpandingSpacer {

        }
    }

    Label {
        text: ("")
    }

    SecondColumnLayout {
        Layout.fillWidth: true

        Label {
            text: "Left"
            width: 28
        }

        SpinBox {
            backendValue: backendValues.Layout_leftMargin
            maximumValue: 0xffff
            minimumValue: 0
            decimals: 0
            implicitWidth: root.spinBoxWidth
        }

        Item {
            width: 4
            height: 4
        }

        Label {
            text: "Right"
            width: 28
        }

        SpinBox {
            backendValue: backendValues.Layout_rightMargin
            maximumValue: 0xffff
            minimumValue: 0
            decimals: 0
            implicitWidth: root.spinBoxWidth
        }
        ExpandingSpacer {

        }
    }

    Label {
        text: qsTr("Row span")
        tooltip: qsTr("Row span of an item in a GridLayout.")
    }

    SecondColumnLayout {
        Layout.fillWidth: true

        Item {
            height: 4
             width: 28
        }

        SpinBox {
            backendValue: backendValues.Layout_rowSpan
            maximumValue: 0xffff
            minimumValue: 0
            decimals: 0
            implicitWidth: root.spinBoxWidth
        }

        ExpandingSpacer {

        }
    }

    Label {
        text: qsTr("Column span")
        tooltip: qsTr("Column span of an item in a GridLayout.")
    }

    SecondColumnLayout {
        Layout.fillWidth: true

        Item {
            height: 4
             width: 28
        }

        SpinBox {
            backendValue: backendValues.Layout_columnSpan
            maximumValue: 0xffff
            minimumValue: 0
            decimals: 0
            implicitWidth: root.spinBoxWidth
        }

        ExpandingSpacer {

        }
    }
}
