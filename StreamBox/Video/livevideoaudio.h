#ifndef LIVEVIDEOAUDIO_H
#define LIVEVIDEOAUDIO_H
extern "C"
{
    #include "libavcodec/avcodec.h"      //编解码库
    #include "libavdevice/avdevice.h"     //输入输出设备，读取摄像头的
    #include "libavfilter/avfilter.h"     //音视频滤镜库，进行音视频处理和编辑
    #include "libavformat/avformat.h"     //文件格式和协议库
    #include "libavutil/avutil.h"         //音视频处理
    #include "libswresample/swresample.h" //音频重采样
    #include "libswscale/swscale.h"       //图像进行格式转换
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
}
#include <QIODevice>
#include <QAudioFormat>
#include <QAudioOutput> //这俩个是QT自带的音视频工具 所以需要在.pro中加入 multimediawidgets multimedia
#include <QMutex>
#include <QWaitCondition>
#include <QString>

#include <QString>
#include <QObject>
#include <QThread>
#include <QImage>

#include <QStringList>
#include <QProcess>
#include <QDebug>
class liveVideoAudio : public QThread
{
    Q_OBJECT
public:
    liveVideoAudio();
    void h264AndAacToMp4(QString accName, QString h264Name, QString outputName);
};

#endif // LIVEVIDEOAUDIO_H
