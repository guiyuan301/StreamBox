#include "videocontroller.h"
#include "MODEL/videomodel.h"
#include <QDebug>

//静态成员初始化
VideoController* VideoController::videocontroller = nullptr;

VideoController::VideoController()
{
}

VideoController* VideoController::getInstance()
{
    if(videocontroller == nullptr)
    {
        videocontroller = new VideoController();
    }
    return videocontroller;
}

void VideoController::destroyInstance()
{
    if(videocontroller != nullptr)
    {
        delete videocontroller;
        videocontroller = nullptr;
        qDebug() << "[信息] VideoController实例已销毁";
    }
}

bool VideoController::getVideoById(int videoId, VideoInfo& info)
{
    if(videoId <= 0)
    {
        qDebug() << "[错误] 视频ID无效";
        return false;
    }

    VideoModel* model = VideoModel::getInstance();
    return model->getVideoById(videoId, info);
}

int VideoController::getVideoCount()
{
    VideoModel* model = VideoModel::getInstance();
    return model->getVideoCount();
}

bool VideoController::addVideo(const QString& videoPath, const QString& coverPath)
{
    if(videoPath.isEmpty())
    {
        qDebug() << "[错误] 视频路径不能为空";
        return false;
    }

    VideoModel* model = VideoModel::getInstance();
    return model->addVideo(videoPath, coverPath);
}

bool VideoController::addVideoWithName(const QString& videoName, const QString& videoPath, const QString& coverPath)
{
    if(videoPath.isEmpty() || videoName.isEmpty())
    {
        qDebug() << "[错误] 视频名称和路径不能为空";
        return false;
    }

    VideoModel* model = VideoModel::getInstance();
    return model->addVideoWithName(videoName, videoPath, coverPath);
}

bool VideoController::deleteVideo(int videoId)
{
    if(videoId <= 0)
    {
        qDebug() << "[错误] 视频ID无效";
        return false;
    }

    VideoModel* model = VideoModel::getInstance();
    return model->deleteVideo(videoId);
}

QList<VideoInfo> VideoController::getAllVideos()
{
    VideoModel* model = VideoModel::getInstance();
    return model->getAllVideos();
}

bool VideoController::deleteVideoByPath(const QString& videoPath)
{
    if(videoPath.isEmpty())
    {
        qDebug() << "[错误] 视频路径不能为空";
        return false;
    }

    VideoModel* model = VideoModel::getInstance();
    return model->deleteVideoByPath(videoPath);
}

bool VideoController::updateVideoName(int videoId, const QString& videoName)
{
    if(videoId <= 0 || videoName.isEmpty())
    {
        qDebug() << "[错误] 参数无效";
        return false;
    }

    VideoModel* model = VideoModel::getInstance();
    return model->updateVideoName(videoId, videoName);
}

bool VideoController::updateVideoCover(int videoId, const QString& coverPath)
{
    if(videoId <= 0)
    {
        qDebug() << "[错误] 视频ID无效";
        return false;
    }

    VideoModel* model = VideoModel::getInstance();
    return model->updateVideoCover(videoId, coverPath);
}

bool VideoController::isVideoExist(const QString& videoPath)
{
    VideoModel* model = VideoModel::getInstance();
    return model->isVideoExist(videoPath);
}

QPixmap VideoController::loadCover(const QString& coverPath)
{
    VideoModel* model = VideoModel::getInstance();
    return model->loadCover(coverPath);
}
