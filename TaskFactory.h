#pragma once
#include <memory>
#include <filesystem>
#include "ITask.h"

class TaskFactory {
public:
    static std::shared_ptr<ITask> CreateFileBackupTask();
    static std::shared_ptr<ITask> CreateMatrixMultiplyTask();
    static std::shared_ptr<ITask> CreateHttpGetTask();
    static std::shared_ptr<ITask> CreateRandomStatsTask();
};