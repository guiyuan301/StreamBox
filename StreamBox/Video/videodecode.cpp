#include "videodecode.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QImage>
#include "audiodecode.h"
int VideoDecode::m_WichChannel = -1;
VideoDecode::VideoDecode()
{
    //注册组件
    av_register_all();
    qDebug()<<"成功注册组件!";
    //初始化成员变量
    pformatContext = nullptr;
    videoIndex = -1;
    pcodecContext = nullptr;
    decoder = nullptr;
    packet = nullptr;
    frame = nullptr;
    frameRGB = nullptr;
    SWS_RGB = nullptr;
    m_isPaused = false;
    m_isFinished = false;
    m_needRestart = false;
    m_needSeek = false;
    m_seekTarget = 0;
    m_stopThread = false;
    m_fmt = nullptr;
    fp_video = nullptr;


}

VideoDecode::VideoDecode(const QString &mypath)
{
    //注册组件
    av_register_all();
    qDebug()<<"成功注册组件!";
    //初始化成员变量
    pformatContext = nullptr;
    videoIndex = -1;
    pcodecContext = nullptr;
    decoder = nullptr;
    packet = nullptr;
    frame = nullptr;
    frameRGB = nullptr;
    SWS_RGB = nullptr;
    m_isPaused = false;
    m_isFinished = false;
    m_needRestart = false;
    m_needSeek = false;
    m_seekTarget = 0;
    m_stopThread = false;
    m_WichChannel = -1;
    openMvFile(mypath);
    findVideoStream();
    findDecoder();
    prepare();
    readFrame();
    //最后手动调用closeAll()
}
// 析构函数，确保资源被正确释放
VideoDecode::~VideoDecode()
{
    closeAll();
}
void VideoDecode::openMvFile(const QString &mypath)
{
    pformatContext = avformat_alloc_context();
    int res = avformat_open_input(&pformatContext, mypath.toUtf8().data(), nullptr, nullptr);
    if(res != 0)
    {
        qDebug() << "打开视频失败，路径:" << mypath;
        if(pformatContext) {
            avformat_free_context(pformatContext);
            pformatContext = nullptr;
        }
    }
    else
    {
         qDebug() << "打开视频成功!";
    }
}

void VideoDecode::registerDevice()
{
    fp_video = fopen((QCoreApplication::applicationDirPath() + "/temp.h264").toLocal8Bit().data(), "wb+");
    avdevice_register_all();
    pformatContext = avformat_alloc_context();

    m_fmt=av_find_input_format("dshow");
    qDebug()<<"carema";
    int res=avformat_open_input(&pformatContext,"video=Integrated Camera",m_fmt,nullptr);
    if(res!=0)
    {
        qDebug()<<"摄像头打开失败";
        return;
    }
}

void VideoDecode::findVideoStream()
{
    if (pformatContext == nullptr)
    {
        return;
    }
    //查找流媒体数据
    int res = avformat_find_stream_info(pformatContext,nullptr);
    if(res <0)
    {
        qDebug()<<"没找到流媒体!";
    }
    //找到流媒体后 寻找视频流
    for (int i =0;i < pformatContext->nb_streams;i++)
    {
       if(pformatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
       {
           videoIndex = i; //设置视频索引
           break;
       }
    }
    //进行判断是否找到了视频流
    if(videoIndex == -1)
    {
        qDebug()<<"未找到视频流!";
    }
    else
    {
         qDebug() <<"找到视频流!";
    }
}

void VideoDecode::findDecoder()
{
    //查询并打开解码器
    //定义一个解码器上下文封装结构体
    pcodecContext = avcodec_alloc_context3(nullptr);
    avcodec_parameters_to_context(pcodecContext,pformatContext->streams[videoIndex]->codecpar);//将视频流的编码参数，复制到【解码器上下文（配置表）】中

    //找解码器
    decoder = avcodec_find_decoder(pcodecContext->codec_id); //按照解码器上下文找到解码器
    if(decoder == nullptr)
    {
        qDebug()<<"找解码器失败!";
    }

    //打开解码器
    int res = avcodec_open2(pcodecContext,decoder,nullptr);
    if(res !=0)
    {
        qDebug()<<"打开解码器失败!";
    }
    else {
        qDebug()<<"打开解码器成功!";
    }
}

void VideoDecode::prepare()
{
    //读取码流数据
    //数据容器初始化
    packet = av_packet_alloc();
    frame = av_frame_alloc();
    frameRGB = av_frame_alloc();

    //给缓冲区分配动态内存空间
    pOutbuffer = (uint8_t*) av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB32,
                                                               pcodecContext->width,pcodecContext->height,1)); //1字节对齐
    //绑定RGB缓冲区到frameRGB
    av_image_fill_arrays(frameRGB->data,frameRGB->linesize,pOutbuffer,AV_PIX_FMT_RGB32
                         ,pcodecContext->width,pcodecContext->height,1);

    //获取图像转换上下文 创建图像转换器：YUV原始帧 -> RGB32格式（Qt可显示）

    SWS_RGB = sws_getContext(pcodecContext->width,pcodecContext->height,pcodecContext->pix_fmt,
                             pcodecContext->width,pcodecContext->height,AV_PIX_FMT_RGB32,SWS_BICUBIC,nullptr,nullptr,nullptr);

    //读取视频帧并显示
    //RGBfp = fopen("../fileout/test.h264","wb+");
}

//读取每一帧并拼接成RGB
void VideoDecode::readFrame()
{
    //计算解码帧数
    int frameNum = 0;

    // 计算视频总时长（秒）
    int totalDuration = 0;
    if (pformatContext->duration != AV_NOPTS_VALUE)
    {
        totalDuration = pformatContext->duration / AV_TIME_BASE;
    }

     m_isFinished = false;//设置视频状态为 未完成
    //读取视频帧
    while(av_read_frame(pformatContext,packet)==0)
    {
        // 检查是否暂停
        while (m_isPaused && !m_stopThread)
        {
            QThread::msleep(100); // 暂停时等待
        }
        // 检查是否需要停止线程
        if (m_stopThread)
        {
            av_packet_unref(packet);
            return;
        }
        // 检查是否需要跳转或重新开始
        if (m_needRestart || m_needSeek)
        {
            av_packet_unref(packet);
            return;
        }
        if(packet->stream_index == videoIndex)
        {
            // 如果是直播模式，直接把裸流写入临时文件
                    if (m_WichChannel == 1 && fp_video)
                    {
                        fwrite(packet->data, 1, packet->size, fp_video);
                    }
            // 发送数据包给解码器
            int res = avcodec_send_packet(pcodecContext, packet);
            if (res == 0) // 如果发送成功，循环接收解码后的画面
            {
                while(avcodec_receive_frame(pcodecContext, frame) == 0)
                {
                    // 解码成功, 开始处理
                    frameNum++;
                    sws_scale(SWS_RGB, frame->data, frame->linesize, 0, frame->height, frameRGB->data, frameRGB->linesize);
                    QImage img = QImage(pOutbuffer, pcodecContext->width, pcodecContext->height, QImage::Format_RGB32);
                    // 获取视频帧时间戳
                    AVStream* videoStream = pformatContext->streams[videoIndex];
                    int64_t pts = frame->best_effort_timestamp;
                    if (pts == AV_NOPTS_VALUE) pts = 0;
                    double frame_time_sec = pts * av_q2d(videoStream->time_base);
                    // 【新增：音视频同步策略】
                    if (m_audioDecode && !m_audioDecode->isFinished())
                    {
                        while (!m_stopThread && !m_needSeek && !m_needRestart)
                        {
                            double audio_time = m_audioDecode->getCurrentTime();
                            double diff = frame_time_sec - audio_time;
                            if (diff > 0.01)
                            {
                                int sleep_ms = qMin(static_cast<int>(diff * 1000), 10);
                                msleep(sleep_ms);
                            } else {
                                break;
                            }
                        }
                    }
                    else
                    {
                        msleep(40);
                    }
                    int currentTime = (int)frame_time_sec;
                    emit updateProgress(currentTime, totalDuration);
                    emit sendImg(img.copy());
                }
            }
        }
        // 这里必须放在外面，无论成功失败都必须释放 packet，否则会疯狂泄漏内存导致奔溃！
        av_packet_unref(this->packet);
    }
     m_isFinished = true; //设置视频播放状态为 已完成
     emit videoFinished();
     qDebug() << "视频解码完成！总帧数：" << frameNum;
     if (fp_video) { fclose(fp_video); }
    }

// 视频控制方法
void VideoDecode::seekTo(int seconds)
{
    m_seekTarget = seconds;
    m_needSeek = true;
    m_isPaused = false;
}

void VideoDecode::restart()
{
    m_needRestart = true;
    m_isPaused = false;
}

void VideoDecode::stop()
{
    m_stopThread = true;
    m_isPaused = false;
}

bool VideoDecode::isFinished()
{
    return m_isFinished;
}

// 暂停/继续控制方法
void VideoDecode::pause()
{
    m_isPaused = true;
}

void VideoDecode::resume()
{
    m_isPaused = false;
}

bool VideoDecode::isPaused()
{
    return m_isPaused;
}


void VideoDecode::closeAll()
{
    // 释放图像转换器
   if(SWS_RGB)
   {
       sws_freeContext(SWS_RGB);
       SWS_RGB = nullptr;
   }
   // 释放RGB缓冲区
   if(pOutbuffer)
   {
       av_free(pOutbuffer);
       pOutbuffer = nullptr;
   }
   // 释放帧/数据包
   if(packet)
   {
       av_packet_free(&packet);
   }
   if(frame)
   {
       av_frame_free(&frame);
   }
   if(frameRGB) {
       av_frame_free(&frameRGB);
   }
   // 释放解码器
   if(pcodecContext)
   {
       avcodec_free_context(&pcodecContext);
   }
   // 关闭并释放视频文件上下文
   if(pformatContext)
   {
       avformat_close_input(&pformatContext);
   }
   if (fp_video) { fclose(fp_video); }

   qDebug() << " 所有资源释放完成！";
}

void VideoDecode::run()
{
    if(m_WichChannel == 0)
    {
        qDebug()<<"影视";
        openMvFile(m_filePath);
    }
    else if(m_WichChannel == 1)
    {
        qDebug()<<"直播";
        registerDevice();
    }

    if(!pformatContext)
    {
        m_isFinished = true;
        return;
    }
    findVideoStream();
    if (videoIndex == -1) return; // 防崩溃

    findDecoder();
    if (!decoder || !pcodecContext) return; // 防崩溃

    prepare();

    while (!m_stopThread)
    {
        // ==========================================
        // 先处理“重播”或“进度条跳转”指令
        // ==========================================
        if (m_needRestart)
        {
            m_needRestart = false;
            AVRational timeBase = pformatContext->streams[videoIndex]->time_base;
            int64_t timestamp = 0;
            avformat_seek_file(pformatContext, videoIndex, INT64_MIN, timestamp, INT64_MAX, AVSEEK_FLAG_BACKWARD);
            avcodec_flush_buffers(pcodecContext);
            m_isFinished = false; // 重置完成状态
            qDebug() << "视频重新开始播放";
        }
        else if (m_needSeek)
        {
            m_needSeek = false;
            AVRational timeBase = pformatContext->streams[videoIndex]->time_base;
            int64_t timestamp = av_rescale_q((int64_t)m_seekTarget * AV_TIME_BASE, AV_TIME_BASE_Q, timeBase);
            avformat_seek_file(pformatContext, videoIndex, INT64_MIN, timestamp, INT64_MAX, AVSEEK_FLAG_BACKWARD);
            avcodec_flush_buffers(pcodecContext);
            m_isFinished = false; // 重置完成状态
            qDebug() << "视频跳转到" << m_seekTarget << "秒";
        }

        // ==========================================
        // 如果没有播放完成，才去读取画面
        // ==========================================
        if (!m_isFinished)
        {
            readFrame();
        }

        // ==========================================
        // 如果视频已经播放完成，进入休眠等待用户点击重播
        // ==========================================
        while (m_isFinished && !m_needRestart && !m_needSeek && !m_stopThread)
        {
            QThread::msleep(50);
        }
    }
}
