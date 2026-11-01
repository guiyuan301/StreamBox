#include "audiodecode.h"
#include <QCoreApplication>
#include <QDebug>
int AudioDecode::m_Wichchannel = -1;
AudioDecode::AudioDecode()
{
    audioIndex = -1;
    fomatcontext = nullptr;
    codec  = nullptr;
    codeContext = nullptr;
    pkt = nullptr;
    picture = nullptr;
    out_buffer = nullptr;
    swrCtx = nullptr;
    audio = nullptr;
    out = nullptr;
    m_isPaused = false;
    m_isFinished = false;
    m_stopThread = false;
    m_needRestart = false;
    m_needSeek = false;
    m_seekTarget = 0;
    m_currentTime = 0.0;
    fp_audio = nullptr;
    m_filePath.clear();

}

AudioDecode::~AudioDecode()
{
    stop();
    wait(3000);
    if (isRunning())
    {
        terminate();
        wait();
    }
    cleanup();
}

void AudioDecode::initDAudio(const QString &filepath)
{
    int res = avformat_open_input(&this->fomatcontext, filepath.toLocal8Bit().data(), nullptr, nullptr);
    if(res != 0)
    {
        qDebug() << "音频打开失败，路径:" << filepath;
        return; // 防奔溃
    }
    else
    {
        qDebug() << "音频打开成功";
    }
    //查找流媒体
    res = avformat_find_stream_info(this->fomatcontext,nullptr);
    if(res < 0)
    {
        qDebug() << "avformat_find_stream_info失败";
        return;
    }
    qDebug() << "avformat_find_stream_info成功";

    for (int i = 0; i < fomatcontext->nb_streams; i++)
    {
        if (fomatcontext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            audioIndex = i;
            break;
        }
    }
    if (audioIndex == -1)
    {
        qDebug() << "未找到音频流";
        return;
    }
    qDebug() << "获取音频流索引成功:" << audioIndex;

    codeContext = avcodec_alloc_context3(nullptr);
    avcodec_parameters_to_context(codeContext, fomatcontext->streams[audioIndex]->codecpar);
    codec = avcodec_find_decoder(fomatcontext->streams[audioIndex]->codecpar->codec_id);
    if (!codec)
    {
        qDebug() << "获取解码器失败";
        return;
    }
    qDebug() << "获取解码器成功";

    res = avcodec_open2(codeContext, codec, nullptr);
    if (res != 0)
    {
        qDebug() << "打开解码器失败";
        return;
    }
    qDebug() << "打开解码器成功";

    pkt = av_packet_alloc();
    picture = av_frame_alloc();

    swrCtx = swr_alloc();
    in_sample_fmt = codeContext->sample_fmt;
    out_sample_fmt = AV_SAMPLE_FMT_S16;
    in_sample_rate = codeContext->sample_rate;
    out_sample_rate = 44100;
    in_ch_layout = codeContext->channel_layout;
    if (in_ch_layout == 0)
    {
        in_ch_layout = av_get_default_channel_layout(codeContext->channels);
    }
    out_ch_layout = AV_CH_LAYOUT_STEREO;
    out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);
    swr_alloc_set_opts(swrCtx, out_ch_layout, out_sample_fmt, out_sample_rate,
                       in_ch_layout, in_sample_fmt, in_sample_rate, 0, nullptr);
    swr_init(swrCtx);
    out_buffer = (uint8_t *)av_malloc(2 * 44100 * 2);
    qDebug() << "音频初始化完成!";
}

void AudioDecode::initDAudioWithDevice()
{
    qDebug()<<"打开1111111111111111111111111111";
    fp_audio = fopen((QCoreApplication::applicationDirPath() + "/temp.aac").toLocal8Bit().data(), "wb+");
    AVInputFormat* inputFormat = av_find_input_format("dshow");
    if (inputFormat == nullptr)
    {
        qDebug() << "No suitable input format found"; // 如果找不到合适的输入格式，输出错误
        return;
    }
    // 2. 为解码上下文分配内存
        this->fomatcontext = avformat_alloc_context();


        // 3. 设置设备名称，这里指定使用 "麦克风阵列 (Realtek(R) Audio)" 作为音频输入设备
        const char* deviceName = "audio=麦克风阵列 (Realtek(R) Audio)";


        // 4. 创建并设置一些选项，用于配置音频捕获的参数
        AVDictionary *opts = nullptr;
        av_dict_set(&opts, "buffer_size", "30410000g", 0);  // 设置缓冲区大小
        av_dict_set(&opts, "rtsp_transport", "udp", 0);      // 设置RTSP传输协议为UDP
        av_dict_set(&opts, "stimeout", "5000880", 0);        // 设置超时参数


        // 5. 打开音频设备并传递配置选项
        int res = avformat_open_input(&this->fomatcontext, deviceName, inputFormat, &opts);
        if (res == 0) {
            qDebug() << "Audio device opened successfully";  // 成功打开设备
        } else {
            qDebug() << "Failed to open audio device";  // 失败，输出错误信息
            return;
        }
        //查找流媒体
        res = avformat_find_stream_info(this->fomatcontext,nullptr);
        if(res < 0)
        {
            qDebug() << "avformat_find_stream_info失败";
            return;
        }
        qDebug() << "avformat_find_stream_info成功";

        for (int i = 0; i < fomatcontext->nb_streams; i++)
        {
            if (fomatcontext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
            {
                audioIndex = i;
                break;
            }
        }
        if (audioIndex == -1)
        {
            qDebug() << "未找到音频流";
            return;
        }
        qDebug() << "获取音频流索引成功:" << audioIndex;

        codeContext = avcodec_alloc_context3(nullptr);
        avcodec_parameters_to_context(codeContext, fomatcontext->streams[audioIndex]->codecpar);
        codec = avcodec_find_decoder(fomatcontext->streams[audioIndex]->codecpar->codec_id);
        if (!codec)
        {
            qDebug() << "获取解码器失败";
            return;
        }
        qDebug() << "获取解码器成功";

        res = avcodec_open2(codeContext, codec, nullptr);
        if (res != 0)
        {
            qDebug() << "打开解码器失败";
            return;
        }
        qDebug() << "打开解码器成功";

        pkt = av_packet_alloc();
        picture = av_frame_alloc();

        swrCtx = swr_alloc();
        in_sample_fmt = codeContext->sample_fmt;
        out_sample_fmt = AV_SAMPLE_FMT_S16;
        in_sample_rate = codeContext->sample_rate;
        out_sample_rate = 44100;
        in_ch_layout = codeContext->channel_layout;
        if (in_ch_layout == 0)
        {
            in_ch_layout = av_get_default_channel_layout(codeContext->channels);
        }
        out_ch_layout = AV_CH_LAYOUT_STEREO;
        out_channel_nb = av_get_channel_layout_nb_channels(out_ch_layout);
        swr_alloc_set_opts(swrCtx, out_ch_layout, out_sample_fmt, out_sample_rate,
                           in_ch_layout, in_sample_fmt, in_sample_rate, 0, nullptr);
        swr_init(swrCtx);
        out_buffer = (uint8_t *)av_malloc(2 * 44100 * 2);
        qDebug() << "音频初始化完成!";

}
void AudioDecode::startDevice()
{
    format.setSampleRate(44100);
    format.setChannelCount(2);
    format.setCodec("audio/pcm");
    format.setSampleType(QAudioFormat::SignedInt);
    format.setSampleSize(16);
    format.setByteOrder(QAudioFormat::LittleEndian);
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format))
    {
        qDebug() << "音频格式不支持";
        return;
    }
    audio = new QAudioOutput(format);
    out = audio->start();
    qDebug() << "音频设备启动成功";
}

void AudioDecode::run()
{
    if(m_Wichchannel == 0)
    {
        initDAudio(m_filePath);
    }
    else if(m_Wichchannel == 1)
    {
        initDAudioWithDevice();
    }
    // 【防奔溃保护】
    if(!fomatcontext || audioIndex == -1)
    {
        qDebug() << "无音频流或文件打开失败，音频线程退出";
        m_isFinished = true;
        return;
    }
    if (audioIndex == -1)
    {
        qDebug() << "无音频流，音频线程退出";
        return;
    }
    startDevice();

    while (!m_stopThread)
    {
        m_isFinished = false;

        while (av_read_frame(fomatcontext, pkt) >= 0)
        {
            while (m_isPaused && !m_stopThread)
            {
                QThread::msleep(50);
            }
            if (m_stopThread)
            {
                av_packet_unref(pkt);
                goto done;
            }
            if (m_needRestart || m_needSeek)
            {
                av_packet_unref(pkt);
                break;
            }

            if (pkt->stream_index == audioIndex)
            {
                // 记录麦克风裸流数据
                        if (m_Wichchannel == 1 && fp_audio)
                        {
                            fwrite(pkt->data, 1, pkt->size, fp_audio);
                        }
                int res = avcodec_send_packet(codeContext, pkt);
                if (res < 0)
                {
                    av_packet_unref(pkt);
                    continue;
                }
                while (avcodec_receive_frame(codeContext, picture) == 0)
                {
                    int converted_samples = swr_convert(swrCtx, &out_buffer, 2 * 44100,
                        (const uint8_t **)(picture->data), picture->nb_samples);
                    int out_buffer_size = converted_samples * out_channel_nb * av_get_bytes_per_sample(out_sample_fmt);

                    AVStream *audioStream = fomatcontext->streams[audioIndex];
                    int64_t pts = picture->best_effort_timestamp;
                    m_currentTime = pts * av_q2d(audioStream->time_base);

                    if (out)
                    {
                        char* data_ptr = reinterpret_cast<char*>(out_buffer);
                        int remain_size = out_buffer_size;

                        // 循环写入，配合音频设备的播放速度
                        while (remain_size > 0 && !m_stopThread && !m_needSeek && !m_needRestart) {
                            if (m_isPaused) {
                                QThread::msleep(10);
                                continue;
                            }
                            int free_bytes = audio->bytesFree(); // 获取设备可用缓冲大小
                            if (free_bytes > 0) {
                                int write_size = qMin(free_bytes, remain_size);
                                int written = out->write(data_ptr, write_size);
                                if (written > 0) {
                                    data_ptr += written;
                                    remain_size -= written;
                                }
                            } else {
                                QThread::msleep(1); // 缓冲满了，等待音频设备播放以腾出空间
                            }
                        }
                    }
                    av_frame_unref(picture);
                }
            }
            av_packet_unref(pkt);
        }

        if (m_needRestart)
        {
            m_needRestart = false;
            AVRational timeBase = fomatcontext->streams[audioIndex]->time_base;
            int64_t timestamp = 0;
            avformat_seek_file(fomatcontext, audioIndex, INT64_MIN, timestamp, INT64_MAX, AVSEEK_FLAG_BACKWARD);
            avcodec_flush_buffers(codeContext);
            m_isFinished = false;
            qDebug() << "音频重新开始播放";
            continue;
        }

        if (m_needSeek)
        {
            m_needSeek = false;
            AVRational timeBase = fomatcontext->streams[audioIndex]->time_base;
            // 同样强制转换为 int64_t
            int64_t timestamp = av_rescale_q((int64_t)m_seekTarget * AV_TIME_BASE, AV_TIME_BASE_Q, timeBase);
            int result = avformat_seek_file(fomatcontext, audioIndex, INT64_MIN, timestamp, INT64_MAX, AVSEEK_FLAG_BACKWARD);
            if (result >= 0)
            {
                avcodec_flush_buffers(codeContext);
                m_isFinished = false;
                m_currentTime = m_seekTarget; // 最重要的!!!!!必须更新当前音频时间，否则会导致视频同步逻辑异常卡死！
                qDebug() << "音频跳转到" << m_seekTarget << "秒";
            }
            continue;
        }

        m_isFinished = true;
        emit audioFinished();
        qDebug() << "音频播放完成";

        while (!m_needRestart && !m_needSeek && !m_stopThread)
        {
            QThread::msleep(50);
        }
    }
    if (fp_audio) { fclose(fp_audio); }
done:
    cleanup();
}

void AudioDecode::cleanup()
{
    if (audio)
    {
        audio->stop();
        delete audio;
        audio = nullptr;
        out = nullptr;
    }
    if (pkt) { av_packet_free(&pkt); pkt = nullptr; }
    if (picture) { av_frame_free(&picture); picture = nullptr; }
    if (out_buffer) { av_free(out_buffer); out_buffer = nullptr; }
    if (swrCtx) { swr_free(&swrCtx); swrCtx = nullptr; }
    if (codeContext) { avcodec_free_context(&codeContext); codeContext = nullptr; }
    if (fomatcontext) { avformat_close_input(&fomatcontext); fomatcontext = nullptr; }
    qDebug() << "音频资源释放完成";
}

void AudioDecode::pause() { m_isPaused = true; }
void AudioDecode::resume() { m_isPaused = false; }
void AudioDecode::stop() { m_stopThread = true; m_isPaused = false; }
void AudioDecode::seekTo(int seconds) { m_seekTarget = seconds; m_needSeek = true; m_isPaused = false; }
void AudioDecode::restart() { m_needRestart = true; m_isPaused = false; }
bool AudioDecode::isPaused() { return m_isPaused; }
bool AudioDecode::isFinished() { return m_isFinished; }
double AudioDecode::getCurrentTime() { return m_currentTime; }

void AudioDecode::setVolume(int volume)
{
    if (audio)
    {
        audio->setVolume(qreal(volume) / 100.0);
    }
}
