#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>
#include <QIcon>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlExtensionPlugin>

#include "BloodGasAnalyzer.h"
#include "HistoricalDataModel.h"
#include "DatabaseManager.h"
#include "AuthenticationManager.h"
#include "CalibrationManager.h"
#include "HL7Manager.h"


int main(int argc, char *argv[])
{
    // Create the app
    QApplication app(argc, argv);

    app.setApplicationName("Blood Gas Analyzer");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Medical Devices Inc.");
    
    // Register QML types
    qmlRegisterType<HistoricalDataModel>("BloodGasAnalyzer", 1, 0, "HistoricalDataModel");
    qmlRegisterType<AuthenticationManager>("BloodGasAnalyzer", 1, 0, "AuthenticationManager");
    qmlRegisterType<CalibrationManager>("BloodGasAnalyzer", 1, 0, "CalibrationManager");
    qmlRegisterType<HL7Manager>("BloodGasAnalyzer", 1, 0, "HL7Manager");
    
    // Create main controller
    BloodGasAnalyzer analyzer;
    
    // Create QML engine
    QQmlApplicationEngine eng;
    
    // Expose C++ objects to QML
    eng.rootContext()->setContextProperty("bloodGasAnalyzer", &analyzer);
    eng.rootContext()->setContextProperty("historicalDataModel", analyzer.getHistoricalDataModel());
    eng.rootContext()->setContextProperty("authManager", analyzer.getAuthenticationManager());
    eng.rootContext()->setContextProperty("calibrationManager", analyzer.getCalibrationManager());
    eng.rootContext()->setContextProperty("hl7Manager", analyzer.getHL7Manager());

    // Create QML application engine
    QQmlApplicationEngine engine;
    Q_INIT_RESOURCE(qml);

    // Add import paths
    eng.addImportPath(QCoreApplication::applicationDirPath() + "qrc:/src/qml");
    eng.addImportPath(QCoreApplication::applicationDirPath() + "qrc:/src/qml/components");

    // Load QML
    eng.load(QUrl("qrc:/src/qml/main.qml"));
    if (eng.rootObjects().isEmpty()) {
        qDebug() << "Failed to load QML";
        return 1;
    }

    // Start the app event loop
    const auto res = app.exec();

    return res;
}
