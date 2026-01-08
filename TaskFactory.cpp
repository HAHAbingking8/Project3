#include "TaskFactory.h"
#include "Tasks.h"
#include <filesystem>

static std::filesystem::path PickDataDir() {
    std::filesystem::path c = "C:\\Data";
    if (std::filesystem::exists(c) && std::filesystem::is_directory(c)) return c;

    auto local = std::filesystem::current_path() / "Data";
    std::filesystem::create_directories(local);
    return local;
}

static std::filesystem::path PickBackupDir() {
    std::filesystem::path d = "D:\\Backup";
    try {
        std::filesystem::create_directories(d);
        return d;
    }
    catch (...) {
        auto local = std::filesystem::current_path() / "Backup";
        std::filesystem::create_directories(local);
        return local;
    }
}

std::shared_ptr<ITask> TaskFactory::CreateFileBackupTask() {
    return std::make_shared<FileBackupTask>(PickDataDir(), PickBackupDir());
}

std::shared_ptr<ITask> TaskFactory::CreateMatrixMultiplyTask() {
    return std::make_shared<MatrixMultiplyTask>();
}

std::shared_ptr<ITask> TaskFactory::CreateHttpGetTask() {
    return std::make_shared<HttpGetZenTask>(std::filesystem::current_path() / "zen.txt");
}

std::shared_ptr<ITask> TaskFactory::CreateRandomStatsTask() {
    return std::make_shared<RandomStatsTask>();
}