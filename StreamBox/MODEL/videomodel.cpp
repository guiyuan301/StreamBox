#include "videomodel.h"
#include "DBdate/dbsqlite3.h"
#include <QDebug>
#include <QFileInfo>
#include <QFile>

//静态成员初始化
VideoModel* VideoModel::videomodel = nullptr;
VideoModel::VideoModel()
{
}
VideoModel::~VideoModel()
{
}

VideoModel* VideoModel::getInstance()
{
    if(videomodel==nullptr)
    {
        VideoModel::videomodel = new VideoModel();
    }
    return VideoModel::videomodel;
}

void VideoModel::destoryInstance()
{
    if(videomodel !=nullptr)
    {
        delete videomodel;
        videomodel = nullptr;
        qDebug() << "[信息] VideoModel实例已销毁";
    }
}

bool VideoModel::addVideo(const QString &videoPath, const QString &coverPath)
{
    // 检查视频路径是否为空
    if(videoPath.isEmpty())
    {
        qDebug() << "[错误] 视频路径不能为空";
        return false;
    }
    // 检查视频是否已存在
    if(isVideoExist(videoPath))
    {
        qDebug() << "[失败] 视频已存在:" << videoPath;
        return false;
    }

    //获取视频名称
    QString videoName = extractVideoName(videoPath);

    //接入数据库
    dbSqlite3* db = dbSqlite3::getInstance();
    QString strsql =QString("INSERT INTO local_video(video_name, video_path, cover_path) "
                            "VALUES('%1', '%2', '%3');")
                        .arg(videoName)
                        .arg(videoPath)
                        .arg(coverPath);
    char** presult = nullptr;
    int nrow = 0, ncolumn = 0;
    int res = db->doSql(strsql, presult, nrow, ncolumn);
    db->freeResult(presult);
    if(res>=0)
    {
        qDebug() << "[成功] 视频添加成功:" << videoName;
        return true;
    }

    qDebug() << "[失败] 视频添加失败";
    return false;
}

bool VideoModel::addVideoWithName(const QString &videoName, const QString &videoPath, const QString &coverPath)
{
    // 参数检查
    if(videoPath.isEmpty() || videoName.isEmpty())
    {
        qDebug() << "[错误] 视频名称和路径不能为空";
        return false;
    }

    // 检查是否已存在
    if(isVideoExist(videoPath))
    {
        qDebug() << "[失败] 视频已存在:" << videoPath;
        return false;
    }

    // 插入数据库
    dbSqlite3* db = dbSqlite3::getInstance();
    QString strsql = QString("INSERT INTO local_video(video_name, video_path, cover_path) "
                             "VALUES('%1', '%2', '%3');")
                         .arg(videoName)
                         .arg(videoPath)
                         .arg(coverPath);

    char** presult = nullptr;
    int nrow = 0, ncolumn = 0;
    int res = db->doSql(strsql, presult, nrow, ncolumn);
    db->freeResult(presult);

    if(res >= 0)
    {
        qDebug() << "[成功] 视频添加成功:" << videoName;
        return true;
    }

    qDebug() << "[失败] 视频添加失败";
    return false;
}

bool VideoModel::deleteVideo(int videoId)
{
    //获取视频信息
    VideoInfo info;
    if(!getVideoById(videoId,info))
    {
        qDebug() << "[失败] 未找到视频，ID:" << videoId;
        return false;
    }

    //删除本地封面文件
    if(!info.coverPath.isEmpty()&&QFile::exists(info.coverPath))
    {
        if(QFile::remove(info.coverPath))
        {
            qDebug() << "[信息] 封面文件已删除:" << info.coverPath;
        }
        else
        {
            qDebug() << "[警告] 封面文件删除失败:" << info.coverPath;
        }
    }
    //删除数据库纪录(观看纪录会自动删除)
    dbSqlite3* db = dbSqlite3::getInstance();
    QString strsql = QString("DELETE FROM local_video WHERE video_id = %1;").arg(videoId);

    char** presult = nullptr;
    int nrow = 0, ncolumn = 0;
    int res = db->doSql(strsql, presult, nrow, ncolumn);
    db->freeResult(presult);

    if(res >= 0)
    {
        qDebug() << "[成功] 视频删除成功:" << info.videoName;
        return true;
    }

    qDebug() << "[失败] 视频删除失败";
    return false;
}

bool VideoModel::deleteVideoByPath(const QString &videoPath)
{
    //根据路径查找视频ID
    dbSqlite3* db = dbSqlite3::getInstance();
    QString strsql = QString("SELECT video_id FROM local_video WHERE video_path = '%1';").arg(videoPath);

    char** presult = nullptr;
    int nrow = 0, ncolumn = 0;
    int res = db->doSql(strsql, presult, nrow, ncolumn);
    if(res <= 0)
    {
        qDebug() << "[失败] 未找到视频:" << videoPath;
        db->freeResult(presult);
        return false;
    }

    //获取视频ID
    int videoId = QString(presult[ncolumn]).toInt();
    db->freeResult(presult);

    //调用deleteideo()
    return deleteVideo(videoId);
}

bool VideoModel::isVideoExist(const QString &videoPath)
{
    dbSqlite3* db = dbSqlite3::getInstance();
    QString strsql = QString("SELECT video_id FROM local_video WHERE video_path = '%1';").arg(videoPath);

    char** presult = nullptr;
    int nrow = 0, ncolumn = 0;
    int res = db->doSql(strsql, presult, nrow, ncolumn);
    db->freeResult(presult);

    return res > 0;
}

QList<VideoInfo> VideoModel::getAllVideos()
{
    QList<VideoInfo> videoList;
    dbSqlite3* db = dbSqlite3::getInstance();

    // 按ID降序排列，最新添加的在前面
    QString strsql = "SELECT video_id, video_name, video_path, cover_path "
                     "FROM local_video ORDER BY video_id DESC;";

    char** presult = nullptr;
    int nrow = 0, ncolumn = 0;
    int res = db->doSql(strsql, presult, nrow, ncolumn);

    if(res > 0)
    {
        // presult结构：
        // [0-3] 列名
        // [4-7] 第一行数据
        // [8-11] 第二行数据
        for(int i = 0; i < nrow; i++)
        {
            VideoInfo info;
            int index = (i + 1) * ncolumn;  // 跳过列名

            info.videoId = QString(presult[index]).toInt();
            info.videoName = QString(presult[index + 1]);
            info.videoPath = QString(presult[index + 2]);
            info.coverPath = QString(presult[index + 3]);

            videoList.append(info);
        }

        qDebug() << "[成功] 查询到" << nrow << "个视频";
    }
    else
    {
        qDebug() << "[信息] 暂无视频数据";
    }

    db->freeResult(presult);
    return videoList;
}

bool VideoModel::getVideoById(int videoId, VideoInfo &info)
{
    dbSqlite3* db = dbSqlite3::getInstance();
    QString strsql = QString("SELECT video_id, video_name, video_path, cover_path "
                             "FROM local_video WHERE video_id = %1;").arg(videoId);

    char** presult = nullptr;
    int nrow = 0, ncolumn = 0;
    int res = db->doSql(strsql, presult, nrow, ncolumn);

    if(res>0)
    {
        int index = ncolumn; //跳过列名
        info.videoId = QString(presult[index]).toInt();
        info.videoName = QString(presult[index + 1]);
        info.videoPath = QString(presult[index + 2]);
        info.coverPath = QString(presult[index + 3]);
        qDebug() << "[成功] 获取视频信息:" << info.videoName;
        db->freeResult(presult);
        return true;
    }
    qDebug() << "[失败] 未找到视频，ID:" << videoId;
    db->freeResult(presult);
    return false;
}

bool VideoModel::getVideoByCoverPath(const QString &coverPath, VideoInfo &info)
{
    dbSqlite3* db = dbSqlite3::getInstance();
    QString strsql = QString("SELECT video_id, video_name, video_path, cover_path "
                             "FROM local_video WHERE cover_path = '%1';").arg(coverPath);

    char** presult = nullptr;
    int nrow = 0, ncolumn = 0;
    int res = db->doSql(strsql, presult, nrow, ncolumn);

    if(res > 0)
    {
        int index = ncolumn;
        info.videoId = QString(presult[index]).toInt();
        info.videoName = QString(presult[index + 1]);
        info.videoPath = QString(presult[index + 2]);
        info.coverPath = QString(presult[index + 3]);

        qDebug() << "[成功] 通过封面找到视频:" << info.videoName;
        db->freeResult(presult);
        return true;
    }

    qDebug() << "[失败] 未找到对应视频，封面路径:" << coverPath;
    db->freeResult(presult);
    return false;
}

int VideoModel::getVideoCount()
{
    dbSqlite3* db = dbSqlite3::getInstance();
    QString strsql = "SELECT COUNT(*) FROM local_video ;";

    char** presult = nullptr;
    int nrow = 0, ncolumn = 0;
    int res = db->doSql(strsql, presult, nrow, ncolumn);

    int count = 0;
    if(res > 0)
    {
        count = QString(presult[ncolumn]).toInt();
    }

    db->freeResult(presult);
    return count;
}

bool VideoModel::updateVideoName(int videoId, const QString &videoName)
{
    dbSqlite3* db = dbSqlite3::getInstance();
    QString strsql = QString("UPDATE local_video SET video_name = '%1' WHERE video_id = %2;")
                         .arg(videoName)
                         .arg(videoId);

    char** presult = nullptr;
    int nrow = 0, ncolumn = 0;
    int res = db->doSql(strsql, presult, nrow, ncolumn);
    db->freeResult(presult);

    if(res >= 0)
    {
        qDebug() << "[成功] 视频名称更新为:" << videoName;
        return true;
    }

    qDebug() << "[失败] 视频名称更新失败";
    return false;
}

bool VideoModel::updateVideoCover(int videoId, const QString &coverPath, bool deleteOld)
{
    // 如果需要删除旧封面，先获取旧封面路径
    QString oldCoverPath;
    if(deleteOld)
    {
        VideoInfo info;
        if(getVideoById(videoId, info))
        {
            oldCoverPath = info.coverPath;
        }
    }

    // 更新数据库
    dbSqlite3* db = dbSqlite3::getInstance();
    QString strsql = QString("UPDATE local_video SET cover_path = '%1' WHERE video_id = %2;")
                         .arg(coverPath)
                         .arg(videoId);

    char** presult = nullptr;
    int nrow = 0, ncolumn = 0;
    int res = db->doSql(strsql, presult, nrow, ncolumn);
    db->freeResult(presult);

    if(res >= 0)
    {
        // 删除旧封面文件
        if(deleteOld && !oldCoverPath.isEmpty() && QFile::exists(oldCoverPath))
        {
            QFile::remove(oldCoverPath);
            qDebug() << "[信息] 旧封面已删除:" << oldCoverPath;
        }

        qDebug() << "[成功] 视频封面更新成功";
        return true;
    }

    qDebug() << "[失败] 视频封面更新失败";
    return false;
}

QPixmap VideoModel::loadCover(const QString &coverPath, const QPixmap &defaultPixmap)
{
    QPixmap pixmap;

    //检查封面路径是否为空
    if(coverPath.isEmpty())
    {
        qDebug() << "[信息] 封面路径为空，使用默认图片";
        return defaultPixmap;
    }
    //检查文件是否存在
    if(!QFile::exists(coverPath))
    {
        qDebug() << "[警告] 封面文件不存在:" << coverPath;
        return defaultPixmap;
    }

    //加载图片
    if(pixmap.load(coverPath))
    {
        qDebug() << "[成功] 封面加载成功";
        return pixmap;
    }
    qDebug() << "[失败] 封面加载失败:" << coverPath;
     return defaultPixmap;
}

QString VideoModel::extractVideoName(const QString& videoPath)
{
    QFileInfo fileInfo(videoPath);
    return fileInfo.completeBaseName();  // 返回不含扩展名的文件名
}
















