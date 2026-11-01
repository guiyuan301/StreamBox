#ifndef CLEANUP_H
#define CLEANUP_H

/**
 * @brief 清理所有单例实例
 *
 * 在程序退出时调用，按依赖关系逆序销毁
 * Controller -> Model -> Database
 */
class CleanUp
{
public:
    CleanUp();
    static void cleanupAllSingletons();
};

#endif // CLEANUP_H
