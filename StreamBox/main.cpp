#include"DBdate/dbsqlite3.h"
#include "VIEW/loginwindow.h"
#include "VIEW/mainwindow.h"
#include "VIEW/splashscreen.h"
#include <QApplication>
#include "DBdate/cleanup.h"
#include <QDebug>
#include "MODEL/usermodel.h"
#include "CONTROLLER/usercontroller.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //登录数据库：固定使用可执行文件所在目录，避免受工作目录影响
    dbSqlite3::setFileDbName(QCoreApplication::applicationDirPath() + "/StreamBox.db");
    // 防止在窗口切换的瞬间（前一个隐藏，后一个还没显示）导致程序意外退出
    a.setQuitOnLastWindowClosed(false);

    // 全局字体
    QFont font("Microsoft YaHei", 10);
    a.setFont(font);
    // 实例化三个核心页面
    SplashScreen *splash = new SplashScreen(); //启动动画
    LoginWindow *login = new LoginWindow();    //登录窗口
    MainWindow *mainWin = new MainWindow();    //主窗口
    // 启动页加载完毕 -> 销毁启动页 -> 淡入登录页
    QObject::connect(splash, &SplashScreen::finished, [&]()
    {
        splash->deleteLater();
        login->showWithAnimation();
    });
    // 登录成功 -> 销毁登录页 -> 淡入主窗口
    QObject::connect(login, &LoginWindow::loginSuccess, [&]()
    {
        login->deleteLater();
        mainWin->showWithAnimation();
    });
    // 主窗口点击关闭 -> 彻底退出程序
    QObject::connect(mainWin, &MainWindow::closedSignal, [&]()
    {
        a.quit();
    });
    // 程序开始：播放启动动画
    splash->showWithAnimation();

    //程序结束时销毁实例
    QObject::connect(&a, &QApplication::aboutToQuit,&CleanUp::cleanupAllSingletons);
    return a.exec();
}
