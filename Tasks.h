#pragma once
#include <filesystem>
#include <string>
#include "ITask.h"
#include "CancellationToken.h"

// TaskA: 文件备份
class FileBackupTask : public ITask {
public:
    FileBackupTask(std::filesystem::path src, std::filesystem::path dstDir)
        : src_(std::move(src)), dstDir_(std::move(dstDir)) {
    }

    std::string GetName() const override { return "TaskA File Backup"; }
    std::string Execute(const CancellationTokenPtr& token) override;

private:
    std::filesystem::path src_;
    std::filesystem::path dstDir_;
};

// TaskB: 矩阵乘法
class MatrixMultiplyTask : public ITask {
public:
    std::string GetName() const override { return "TaskB Matrix Multiply"; }
    std::string Execute(const CancellationTokenPtr& token) override;
};

// TaskC: HTTP请求
class HttpGetZenTask : public ITask {
public:
    explicit HttpGetZenTask(std::filesystem::path outFile)
        : outFile_(std::move(outFile)) {
    }

    std::string GetName() const override { return "TaskC HTTP GET Zen"; }
    std::string Execute(const CancellationTokenPtr& token) override;

private:
    std::filesystem::path outFile_;
};

// TaskE: 随机统计
class RandomStatsTask : public ITask {
public:
    std::string GetName() const override { return "TaskE Random Stats"; }
    std::string Execute(const CancellationTokenPtr& token) override;
};