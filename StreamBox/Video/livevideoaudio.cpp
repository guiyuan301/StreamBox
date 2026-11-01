#include "livevideoaudio.h"
#include <QCoreApplication>
#include <QDebug>

liveVideoAudio::liveVideoAudio()
{
}

void liveVideoAudio::h264AndAacToMp4(QString accName, QString h264Name, QString outputName)
{
    QStringList arguments;
    arguments << "-framerate" << "30"
              << "-i" << h264Name
              // 告诉 FFmpeg 这是一个 raw PCM 音频流
              << "-f" << "s16le"      // 16位有符号整数
              << "-ar" << "44100"     // 采样率 44.1kHz
              << "-ac" << "2"         // 双声道
              << "-i" << accName
              << "-c:v" << "libx264"
              << "-c:a" << "aac"      // 将 PCM 编码为 AAC 放进 mp4
              << "-strict" << "experimental"
              << "-y"
              << outputName;
    QString ffmpegPath = QCoreApplication::applicationDirPath() + "/ffmpeg.exe";
    QProcess process;
    process.start(ffmpegPath, arguments);
    if (!process.waitForStarted()) {
        qDebug() << "FFmpeg 启动失败! 请检查:" << ffmpegPath << " 是否存在该文件！";
        return;
    }

    if (!process.waitForFinished(-1)) {  // -1 表示无限等待
        qDebug() << "FFmpeg 进程执行错误:" << process.errorString();
        return;
    }

    QString error = process.readAllStandardError();
    if (process.exitCode() != 0) {
        qDebug() << "FFmpeg 合成警告或失败信息:" << error;
    } else {
        qDebug() << "FFmpeg 合成 MP4 成功！";
    }
}
