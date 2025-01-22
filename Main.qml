/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QtQuick.Dialogs

import QtWebView

Window {
    width: 1000
    height: 600
    visible: true
    title: "VidAiO"

    WebView {
        id: webView
        anchors.fill: parent
        url: "http://localhost:33444/ani/search/kuragehime"
    }
}
