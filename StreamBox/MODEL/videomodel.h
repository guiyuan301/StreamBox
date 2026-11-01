#ifndef VIDEOMODEL_H
#define VIDEOMODEL_H
#include <QString>
#include <QList>
#include <QPixmap>

/**
 * @brief The VideoInfo struct
 * 储存单个视频的完整信息
 */
struct VideoInfo
{
    int videoId;        //视频ID
    QString videoName;  //视频名称
    QString videoPath;  //视频路径
    QString coverPath;  //封面路径
};
/**
 * @brief 视频业务模型类
 *
 * 该类封装视频相关的数据库操作逻辑，包括：
 * 视频添加（支持选择视频文件和封面图片）
 * 视频删除（同时删除封面文件）
 * 视频查询（支持ID查询和列表查询）
 * 视频路径获取（用于播放）
 *
 * 使用流程：
 * 1. 调用 getInstance() 获取单例实例
 * 2. 调用相关业务方法进行操作
 * 3. 程序结束时调用 destroyInstance() 销毁实例
 *
 * 播放视频：getVideoById()  获取videoPath  传给播放器
 * 添加视频：addVideo()  保存到数据库
 * 删除视频：deleteVideo()  删除记录和封面文件
 * 展示列表：getAllVideos()  遍历显示封面
 */
class VideoModel
{
private:
    VideoModel();
    ~VideoModel();
    VideoModel(const VideoModel& ) = delete;
    VideoModel& operator=(const VideoModel&) = delete;

    static VideoModel* videomodel;

public:
    static VideoModel* getInstance();
    static void destoryInstance();

    /**
     * @brief 添加视频到数据库
     *
     * 将视频信息保存到数据库，支持自定义封面路径
     *
     * @param videoPath 视频文件完整路径
     * @param coverPath 封面图片路径（可为空，使用默认封面）
     * @return bool true=添加成功，false=添加失败或已存在
     *
     * @note 视频名称自动从文件名提取（不含扩展名）
     * @note 如果视频路径已存在，则不会重复添加
     */
    bool addVideo(const QString& videoPath, const QString& coverPath = "");
    bool addVideoWithName(const QString& videoName, const QString& videoPath, const QString& coverPath = ""); //视频名称自定义
    /**
     * @brief 根据视频ID删除视频
     *
     * 删除流程：
     * 1. 获取视频信息（用于删除封面文件）
     * 2. 删除数据库记录
     * 3. 删除本地封面图片文件
     * 4. 观看历史自动删除（外键约束）
     *
     * @param videoId 视频ID
     * @return bool true=删除成功，false=删除失败
     *
     * @note 会同时删除本地封面文件
     * @note 不会删除本地视频文件
     * @note 观看历史会自动清理（数据库外键约束）
     */
    bool deleteVideo(int videoId);
    bool deleteVideoByPath(const QString& videoPath);

    bool isVideoExist(const QString& videoPath); //检查视频是否已经存在 (避免反复添加同一个视频路径下的视频)
    /**
     * @brief 获取所有视频列表
     *
     * 用于在UI界面展示所有视频封面
     *
     * @return QList<VideoInfo> 视频信息列表
     *
     * @note 返回列表按ID降序排列（最新添加的在前面）
     */
    QList<VideoInfo> getAllVideos();
    /**
     * @brief 根据视频ID获取视频信息
     *
     * 主要用于播放视频时获取视频路径
     *
     * @param videoId 视频ID
     * @param info 输出：视频信息
     * @return bool true=获取成功，false=未找到
     *
     * @note 点击封面后，通过视频ID获取完整信息，然后播放
     */
    bool getVideoById(int videoId,VideoInfo& info);
    bool getVideoByCoverPath(const QString& coverPath, VideoInfo& info);
    int getVideoCount(); //获取视频总数

    bool updateVideoName(int videoId, const QString& videoName); //更新视频名称
    /**
    * @brief 更新视频封面
    *
    * @param videoId 视频ID
    * @param coverPath 新封面路径
    * @param deleteOld 是否删除旧封面文件
    * @return bool true=成功，false=失败
    */
    bool updateVideoCover(int videoId, const QString& coverPath, bool deleteOld = true);

    /**
     * @brief 加载封面图片
     *
     * 用于在UI界面显示封面
     *
     * @param coverPath 封面路径
     * @param defaultPixmap 默认图片（封面不存在时使用）
     * @return QPixmap 封面图片
     */
    QPixmap loadCover(const QString& coverPath, const QPixmap& defaultPixmap = QPixmap());
    QString extractVideoName(const QString& videoPath); //从视频路径获取视频名称
};

#endif // VIDEOMODEL_H
