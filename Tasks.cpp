#include "Tasks.h"
#include <chrono>
#include <thread>
#include <random>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <iostream>

// 辅助函数：获取当前日期时间
static std::string GetCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_s(&tm, &time);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
    return std::string(buffer);
}

static std::string GetCurrentDate() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_s(&tm, &time);

    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y%m%d", &tm);
    return std::string(buffer);
}

// -------------------- TaskA: 文件备份 --------------------
std::string FileBackupTask::Execute(const CancellationTokenPtr& token) {
    std::cout << "FileBackupTask::Execute started" << std::endl;

    // 模拟工作
    for (int i = 0; i < 5; i++) {
        if (token && token->IsCancelled()) {
            std::cout << "FileBackupTask cancelled" << std::endl;
            return "Backup cancelled at step " + std::to_string(i);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    try {
        // 确保目录存在
        std::filesystem::create_directories(dstDir_);

        // 创建备份文件
        std::string backupName = "backup_" + GetCurrentDate() + ".txt";
        std::filesystem::path backupPath = dstDir_ / backupName;

        std::ofstream ofs(backupPath);
        if (ofs) {
            ofs << "=== File Backup ===\n";
            ofs << "Time: " << GetCurrentDateTime() << "\n";
            ofs << "Source: " << src_.string() << "\n";
            ofs << "Destination: " << dstDir_.string() << "\n";
            ofs << "Backup file: " << backupName << "\n";
            ofs << "Status: SUCCESS\n";
            ofs << "===================\n";
        }

        std::cout << "FileBackupTask completed: " << backupPath.string() << std::endl;
        return "Backup created: " + backupPath.string();
    }
    catch (const std::exception& e) {
        std::cout << "FileBackupTask error: " << e.what() << std::endl;
        return "Backup error: " + std::string(e.what());
    }
}

// -------------------- TaskB: 矩阵乘法 --------------------
std::string MatrixMultiplyTask::Execute(const CancellationTokenPtr& token) {
    std::cout << "MatrixMultiplyTask::Execute started" << std::endl;

    // 使用较小的矩阵确保快速完成
    const int N = 100;
    std::vector<double> A(N * N), B(N * N), C(N * N, 0.0);

    std::mt19937 rng(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    // 初始化矩阵
    for (int i = 0; i < N * N; ++i) {
        if (token && token->IsCancelled()) {
            std::cout << "MatrixMultiplyTask cancelled during initialization" << std::endl;
            return "Matrix calculation cancelled";
        }
        A[i] = dist(rng);
        B[i] = dist(rng);
    }

    auto start = std::chrono::high_resolution_clock::now();

    // 矩阵乘法
    for (int i = 0; i < N; ++i) {
        if (token && token->IsCancelled()) {
            std::cout << "MatrixMultiplyTask cancelled during calculation" << std::endl;
            return "Matrix calculation cancelled";
        }

        for (int j = 0; j < N; ++j) {
            double sum = 0.0;
            for (int k = 0; k < N; ++k) {
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }

        // 每10行检查一次取消
        if (i % 10 == 0 && token && token->IsCancelled()) {
            std::cout << "MatrixMultiplyTask cancelled during row " << i << std::endl;
            return "Matrix calculation cancelled at row " + std::to_string(i);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // 计算结果
    double trace = 0.0;
    for (int i = 0; i < N; ++i) {
        trace += C[i * N + i];
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "Matrix " << N << "x" << N << " multiply completed in "
        << duration.count() << "ms. Trace = " << trace;

    std::cout << "MatrixMultiplyTask completed: " << oss.str() << std::endl;
    return oss.str();
}

// -------------------- TaskC: HTTP请求 --------------------
std::string HttpGetZenTask::Execute(const CancellationTokenPtr& token) {
    std::cout << "HttpGetZenTask::Execute started" << std::endl;

    // 模拟网络延迟
    for (int i = 0; i < 3; i++) {
        if (token && token->IsCancelled()) {
            std::cout << "HttpGetZenTask cancelled" << std::endl;
            return "HTTP request cancelled";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    try {
        // 模拟GitHub Zen API响应
        const char* zenQuotes[] = {
            "Simplicity is prerequisite for reliability.",
            "It's not fully shipped until it's fast.",
            "Practicality beats purity.",
            "Avoid administrative distraction.",
            "Mind your words, they are important.",
            "Non-blocking is better than blocking.",
            "Design for failure.",
            "Half measures are as bad as nothing at all.",
            "Favor focus over features.",
            "Approachable is better than simple."
        };

        std::mt19937 rng(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
        std::uniform_int_distribution<size_t> dist(0, sizeof(zenQuotes) / sizeof(zenQuotes[0]) - 1);

        std::string zenQuote = zenQuotes[dist(rng)];

        // 写入文件
        std::ofstream ofs(outFile_);
        if (ofs) {
            ofs << "=== GitHub Zen ===\n";
            ofs << "Time: " << GetCurrentDateTime() << "\n";
            ofs << "Quote: " << zenQuote << "\n";
            ofs << "==================\n";
        }

        std::cout << "HttpGetZenTask completed: " << zenQuote << std::endl;
        return "Zen quote saved to " + outFile_.string() + ": " + zenQuote;
    }
    catch (const std::exception& e) {
        std::cout << "HttpGetZenTask error: " << e.what() << std::endl;
        return "HTTP error: " + std::string(e.what());
    }
}

// -------------------- TaskE: 随机统计 --------------------
std::string RandomStatsTask::Execute(const CancellationTokenPtr& token) {
    std::cout << "RandomStatsTask::Execute started" << std::endl;

    const int N = 500; // 减少数量确保快速完成
    std::vector<int> numbers;
    numbers.reserve(N);

    std::mt19937 rng(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<int> dist(0, 100);

    double sum = 0.0;
    for (int i = 0; i < N; ++i) {
        if (token && token->IsCancelled()) {
            std::cout << "RandomStatsTask cancelled" << std::endl;
            return "Random stats calculation cancelled";
        }

        int num = dist(rng);
        numbers.push_back(num);
        sum += num;

        // 每100个数检查一次取消
        if (i % 100 == 0 && token && token->IsCancelled()) {
            std::cout << "RandomStatsTask cancelled at iteration " << i << std::endl;
            return "Random stats calculation cancelled at iteration " + std::to_string(i);
        }
    }

    double mean = sum / N;

    double variance = 0.0;
    for (int i = 0; i < N; ++i) {
        if (token && token->IsCancelled()) {
            std::cout << "RandomStatsTask cancelled during variance calculation" << std::endl;
            return "Random stats calculation cancelled";
        }

        double diff = numbers[i] - mean;
        variance += diff * diff;
    }
    variance /= N;

    double stddev = std::sqrt(variance);

    // 写入文件
    std::ofstream ofs("random_stats.txt", std::ios::app);
    if (ofs) {
        ofs << "=== Random Statistics ===\n";
        ofs << "Time: " << GetCurrentDateTime() << "\n";
        ofs << "Count: " << N << "\n";
        ofs << std::fixed << std::setprecision(4);
        ofs << "Mean: " << mean << "\n";
        ofs << "Variance: " << variance << "\n";
        ofs << "Standard Deviation: " << stddev << "\n";
        ofs << "Min: " << *std::min_element(numbers.begin(), numbers.end()) << "\n";
        ofs << "Max: " << *std::max_element(numbers.begin(), numbers.end()) << "\n";
        ofs << "=========================\n";
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4);
    oss << "Generated " << N << " random numbers. ";
    oss << "Mean: " << mean << ", Variance: " << variance << ", StdDev: " << stddev;

    std::cout << "RandomStatsTask completed: " << oss.str() << std::endl;
    return oss.str();
}