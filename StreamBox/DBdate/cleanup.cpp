#include "cleanup.h"
#include <QDebug>
#include "dbsqlite3.h"
#include "MODEL/usermodel.h"
#include "MODEL/videomodel.h"
#include "CONTROLLER/usercontroller.h"
#include "CONTROLLER/videocontroller.h"
CleanUp::CleanUp()
{

}

void CleanUp::cleanupAllSingletons()
{
    //Controller -> Model -> Database


    //Controller
    UserController::destroyInstance();
    VideoController::destroyInstance();

    //Model
    UserModel::destroyInstance();
    VideoModel::destoryInstance();

    //Database
    dbSqlite3::destroyInstance();
    qDebug() << "所有单例已清理";
}
