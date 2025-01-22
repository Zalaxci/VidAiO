#include "web/web.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QtWebView>
#include <QNetworkProxy>

int main(int argc, char *argv[]) {
    // Serve the web server in a seperate goroutine
    Serve();

    // Initialize webview
    QtWebView::initialize();

    // Set the graphics API to OgenGl (needed to get MPV to work)
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

    // Create a QGuiApplication object
    QGuiApplication app(argc, argv);

    // Connect our app to the QML file
    QQmlApplicationEngine engine(&app);
    const QUrl url(QStringLiteral("qrc:/Main.qml"));
    auto onObjectCreated = [url](const QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            QCoreApplication::exit(-1);
        }
    };
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, onObjectCreated, Qt::QueuedConnection);
    engine.loadFromModule("ga.calyx.VidAiO", "Main");

    // Finally, execute the app, and return when it is closed
    return app.exec();
}
