#ifndef VIDEODECODE_H
#define VIDEODECODE_H
#include <QString>
#include <QObject>
#include <QThread>
#include <QImage>
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
class AudioDecode;// 顶部加个前置声明
class VideoDecode : public QThread
{
     Q_OBJECT
public:
    VideoDecode();
    VideoDecode(const QString& mypath);
    ~VideoDecode();
    void openMvFile(const QString& mypath); //打开视频文件
    void registerDevice();
    void findVideoStream(); //查找视频流
    void findDecoder();     //查找并打开解码器
    void prepare(); //初始化解码资源
    void readFrame();//读取并解码视频帧
    void closeAll(); //关闭释放所有解码相关的内存
    // 暂停/继续控制
    void pause();
    void resume();
    bool isPaused();

    // 视频控制
    void seekTo(int seconds); // 跳转到指定时间
    void restart(); // 重新开始播放
    bool isFinished(); // 检查视频是否播放完成
    void stop(); // 停止线程

    //线程
    void run () override;
    void setAudioDecode(AudioDecode* audio) { m_audioDecode = audio; } //把 AudioDecode 的指针传给 VideoDecode
    void setFilePath(const QString& path) { m_filePath = path; }

    //因为有俩个区域使用这个功能所以设置一个判断标准
    static int m_WichChannel; //0:影视  1:直播
signals:
    void sendImg(const QImage&);
    void updateProgress(int current, int total); // 进度更新信号
    void videoFinished(); // 视频播放完成信号

private:
    AVFormatContext* pformatContext; //视频上下文封装结构体
    int videoIndex; //视频索引
    AVCodecContext* pcodecContext; //编解码器上下文封装结构体（解码器配置表）
    AVCodec* decoder; //解码器实体(真正工作的工具)
    AVPacket* packet;// 压缩数据包（存储从文件读取的编码数据）
    AVFrame*  frame;// 原始帧（存储解码后的YUV数据）
    AVFrame*  frameRGB;// RGB帧（存储转换后的RGB数据）
    SwsContext* SWS_RGB;   //图像转换上下文（YUV -> RGB32转换器）
    uint8_t* pOutbuffer;   // RGB缓冲区
    FILE *RGBfp;  // RGB文件指针（保存原始RGB数据）

    bool m_isPaused; // 暂停状态标志
    bool m_isFinished; // 视频播放完成标志
    bool m_needRestart; // 需要重新开始标志
    bool m_needSeek; // 需要跳转标志
    int m_seekTarget; // 跳转目标时间（秒）
    bool m_stopThread; // 停止线程标志
    FILE* fp_video;

    AudioDecode* m_audioDecode = nullptr; // audio的指针
    QString m_filePath;


    AVInputFormat *m_fmt;//摄像头输入

};

#endif // VIDEODECODE_H
