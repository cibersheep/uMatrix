#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QtQml>


#include "connection.h"
#include "room.h"
#include "user.h"
#include "jobs/syncjob.h"
#include "jobs/joinroomjob.h"
#include "jobs/leaveroomjob.h"
#include "models/messageeventmodel.h"
#include "models/roomlistmodel.h"
#include "settings.h"
using namespace QMatrixClient;

Q_DECLARE_METATYPE(SyncJob*)
Q_DECLARE_METATYPE(Room*)


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QSettings::setDefaultFormat(QSettings::IniFormat);

    QQuickView view;

    if(qgetenv("QT_QUICK_CORE_PROFILE").toInt()) {
        QSurfaceFormat f = view.format();
        f.setProfile(QSurfaceFormat::CoreProfile);
        f.setVersion(4, 4);
        view.setFormat(f);
    }

    QQmlApplicationEngine engine;
//    engine.addImportPath("./lib/arm-linux-gnueabihf/QtQuick/Controls");
//    engine.addImportPath("./lib/arm-linux-gnueabihf/QtQuick/Dialogs");
    view.connect(&engine, SIGNAL(quit()), &app, SLOT(quit()));
    new QQmlFileSelector(view.engine(), &view);


    qmlRegisterType<SyncJob>();     qRegisterMetaType<SyncJob*> ("SyncJob*");
    qmlRegisterType<JoinRoomJob>(); qRegisterMetaType<JoinRoomJob*> ("JoinRoomJob*");
    qmlRegisterType<LeaveRoomJob>();qRegisterMetaType<LeaveRoomJob*> ("LeaveRoomJob*");
    qmlRegisterType<Room>();        qRegisterMetaType<Room*>    ("Room*");
    qmlRegisterType<User>();        qRegisterMetaType<User*>    ("User*");

    qmlRegisterType<Connection>        ("Matrix", 1, 0, "Connection");
    qmlRegisterType<MessageEventModel> ("Matrix", 1, 0, "MessageEventModel");
    qmlRegisterType<RoomListModel>     ("Matrix", 1, 0, "RoomListModel");
//    qmlRegisterType<Settings>          ("Matrix", 1, 0, "Settings");



    view.setSource(QUrl(QStringLiteral("qrc:///Main.qml")));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.show();
    return app.exec();
}
