
#include "BloodGasAnalyzer.h"
#include "HistoricalDataModel.h"
#include "DatabaseManager.h"
#include "AuthenticationManager.h"
#include "CalibrationManager.h"
#include "HL7Manager.h"

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>
#include <QIcon>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlExtensionPlugin>
#include <QDebug>


int main(int argc, char *argv[])
{
    // Create the app
    QApplication app(argc, argv);
    app.setApplicationName("Blood Gas Analyzer");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Medical Devices Inc.");

    // Register QML types
    qmlRegisterType<HistoricalDataModel>("HistoricalDataModel", 1, 0, "HistoricalDataModel");
    qmlRegisterType<AuthenticationManager>("AuthenticationManager", 1, 0, "AuthenticationManager");
    qmlRegisterType<CalibrationManager>("CalibrationManager", 1, 0, "CalibrationManager");
    qmlRegisterType<HL7Manager>("HL7Manager", 1, 0, "HL7Manager");

    // Create main controller
    BloodGasAnalyzer analyzer;
    auto res = 1;

    // Ensure QML engine is destroyed before those C++ objects that are used in QML
    // (e.g. BloodGasAnalyzer analyzer) are destroyed, to avoid crashes during application shutdown.
    {
        QQmlApplicationEngine eng;
        
        // Expose C++ objects to QML
        eng.rootContext()->setContextProperty("bloodGasAnalyzer", &analyzer);
        eng.rootContext()->setContextProperty("historicalDataModel", analyzer.getHistoricalDataModel());
        eng.rootContext()->setContextProperty("authManager", analyzer.getAuthenticationManager());
        eng.rootContext()->setContextProperty("calibrationManager", analyzer.getCalibrationManager());
        eng.rootContext()->setContextProperty("hl7Manager", analyzer.getHL7Manager());

        Q_INIT_RESOURCE(qml);
        Q_INIT_RESOURCE(resources);
        eng.addImportPath(QCoreApplication::applicationDirPath() + "qrc:/src/qml");
        eng.addImportPath(QCoreApplication::applicationDirPath() + "qrc:/src/qml/components");

        // Load QML
        eng.load(QUrl("qrc:/src/qml/main.qml"));
        if (eng.rootObjects().isEmpty()) {
            qDebug() << "Failed to load QML";
            return 1;
        }

        // Start the app event loop
        res = app.exec();

    } // QQmlApplicationEngine is destroyed here, BloodGasAnalyzer still exists

    return res;
}
