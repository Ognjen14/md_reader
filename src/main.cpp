#include "fontcatalog.h"
#include "singleinstance.h"

#include <QFileInfo>
#include <QGuiApplication>
#include <QIcon>
#include <QMetaObject>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QVariant>
#include <QVariantMap>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QGuiApplication::setOrganizationName(QStringLiteral("MDReader"));
    QGuiApplication::setOrganizationDomain(QStringLiteral("mdreader.local"));
    QGuiApplication::setApplicationName(QStringLiteral("MDReader"));
    QGuiApplication::setApplicationVersion(QStringLiteral(MDREADER_VERSION));

    QGuiApplication::setWindowIcon(QIcon(QStringLiteral(":/qt/qml/MDReader/icons/mdreader.ico")));

    QQuickStyle::setStyle(QStringLiteral("Basic"));

    FontCatalog::loadBundled();

    QString startupFile;
    const QStringList args = QCoreApplication::arguments();
    if (args.size() > 1) {
        const QFileInfo info(args.at(1));
        if (info.exists() && info.isFile())
            startupFile = info.absoluteFilePath();
    }

    SingleInstance instance(QStringLiteral("MDReader.%1")
                                .arg(qEnvironmentVariable("USERNAME", QStringLiteral("default"))));

    if (instance.handOff(startupFile))
        return 0;

    instance.listen();

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    QObject::connect(&instance, &SingleInstance::messageReceived, &app,
                     [&engine](const QString &path) {
                         const QList<QObject *> roots = engine.rootObjects();
                         if (roots.isEmpty())
                             return;
                         QMetaObject::invokeMethod(roots.first(), "handleSecondInstance",
                                                   Q_ARG(QVariant, QVariant(path)));
                     });

    engine.setInitialProperties({{QStringLiteral("startupFile"), startupFile}});
    engine.loadFromModule("MDReader", "Main");

    return QGuiApplication::exec();
}
