#ifndef VIDEOCONTROLLER_H
#define VIDEOCONTROLLER_H
#include <QString>
#include <QList>
#include "MODEL/videomodel.h"


/**
 * @brief 视频控制器类
 *
 * 使用流程：
 * 1. 调用 getInstance() 获取单例实例
 * 2. 调用相关方法处理视频操作
 * 3. 程序结束时调用 destroyInstance() 销毁实例
 */
class VideoController
{
private:
    VideoController();
    VideoController(const VideoController&) = delete;
    VideoController& operator=(const VideoController&) = delete;

    static VideoController* videocontroller;
public:
    static VideoController* getInstance();
    static void destroyInstance();
    QList<VideoInfo> getAllVideos();
    bool getVideoById(int videoId, VideoInfo& info);
    int getVideoCount();
    bool addVideo(const QString& videoPath, const QString& coverPath = "");
    bool addVideoWithName(const QString& videoName, const QString& videoPath, const QString& coverPath = "");
    bool deleteVideo(int videoId);
    bool deleteVideoByPath(const QString& videoPath);
    bool updateVideoName(int videoId, const QString& videoName);
    bool updateVideoCover(int videoId, const QString& coverPath);
    bool isVideoExist(const QString& videoPath);
    QPixmap loadCover(const QString& coverPath);
};

#endif // VIDEOCONTROLLER_H
