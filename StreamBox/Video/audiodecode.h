#ifndef AUDIODECODE_H
#define AUDIODECODE_H
//兼容C语法的框架
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
#include <QThread>
#include <QAudioFormat>
#include <QAudioOutput> //这俩个是QT自带的音视频工具 所以需要在.pro中加入 multimediawidgets multimedia
#include <QMutex>
#include <QWaitCondition>
#include <QString>
class AudioDecode : public QThread
{
    Q_OBJECT
public:
    AudioDecode();
    ~AudioDecode();
    void initDAudio(const QString &filepath);
    void initDAudioWithDevice();
    void run() override;
    void cleanup();
    void startDevice();
    void pause();
    void resume();
    void stop();
    void seekTo(int seconds);
    void restart();
    bool isPaused();
    bool isFinished();
    double getCurrentTime();
    void setFilePath(const QString& path) { m_filePath = path; }
    void setVolume(int volume);
    //因为有俩个区域使用这个功能所以设置一个判断标准
    static int m_Wichchannel; //0:影视 1:直播
signals:
    void audioFinished();

private:
    AVFormatContext* fomatcontext;
    int audioIndex;
    AVCodec* codec;
    AVCodecContext* codeContext;
    AVPacket *pkt;
    AVFrame *picture;
    SwrContext *swrCtx;
    enum AVSampleFormat in_sample_fmt; //输入的采样格式
    enum AVSampleFormat out_sample_fmt ; //输出的采样格式 16bit PCM
    int in_sample_rate; //输入的采样格式
    int out_sample_rate; //输出的采样率
    uint64_t in_ch_layout ;//输入的声道布局
    uint64_t out_ch_layout ;//输出的声道布局
    int out_channel_nb; //获取输出的声道个数
    uint8_t *out_buffer;
    QAudioFormat format;
    QAudioOutput *audio;
    QIODevice *out;
    QString m_filePath;
    FILE* fp_audio;

    bool m_isPaused;
    bool m_isFinished;
    bool m_stopThread;
    bool m_needRestart;
    bool m_needSeek;
    int m_seekTarget;
    double m_currentTime;

};

#endif // AUDIODECODE_H
