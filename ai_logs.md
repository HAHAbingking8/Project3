# AI使用日志（课程大作业：轻量级多任务调度器）
## 2025年1月1日 星期三
### 使用时间：10:00-11:30
### AI工具：豆包
### 使用场景/目的：明确项目3核心架构与技术栈，梳理轻量级多任务调度器的实现思路
### 输入关键词/问题：C++轻量级多任务调度器设计方案，基于MFC，需支持一次性、延迟、周期任务，运用Factory、Command、Observer设计模式，包含任务管理、日志记录、跨线程UI更新
### AI输出核心内容：
1. 架构设计建议：采用三层结构，UI层负责交互、核心层（TaskScheduler+ITask）处理调度逻辑、工具层（LogWriter）负责日志；
2. 核心类设计（关键代码片段）：
```cpp
// ITask抽象接口（Command模式）
class ITask {
public:
    enum TaskType { ONETIME, DELAYED, PERIODIC };
    virtual ~ITask() = default;
    virtual std::string GetName() const = 0;
    virtual void Execute() = 0;
    virtual TaskType GetType() const = 0;
};

// TaskScheduler单例核心类
class TaskScheduler {
private:
    static std::unique_ptr<TaskScheduler> instance;
    std::priority_queue<std::shared_ptr<ScheduledTask>> taskQueue;
    std::mutex mtx;
    std::condition_variable cv;
    std::thread workerThread;
    bool isRunning = false;

    // 私有构造函数（单例）
    TaskScheduler() {
        isRunning = true;
        workerThread = std::thread(&TaskScheduler::WorkerLoop, this);
    }
public:
    static TaskScheduler& GetInstance() {
        static TaskScheduler instance; // 懒汉式线程安全单例
        return instance;
    }

    // 禁用拷贝
    TaskScheduler(const TaskScheduler&) = delete;
    TaskScheduler& operator=(const TaskScheduler&) = delete;

    void AddTask(std::shared_ptr<ScheduledTask> task);
    void WorkerLoop();
    void Stop();
};
```
3. 设计模式应用：TaskFactory创建ITask实例（Factory模式），ScheduledTask封装执行命令（Command模式），UI层订阅任务状态（Observer模式）；
4. 关键技术点提示：使用mutex+condition_variable保证线程安全，priority_queue按执行时间排序任务，跨线程UI更新通过消息映射实现。
### 实际应用与反思：
基于AI输出确定项目整体架构，核心类设计与课程要求一致，后续需重点聚焦线程安全和任务调度时序逻辑；AI推荐的设计模式应用场景清晰，可直接作为代码实现的核心框架，但需补充异常处理和资源释放的细节规划。

## 2025年1月2日 星期四
### 使用时间：14:00-15:40
### AI工具：ChatGPT
### 使用场景/目的：编写ITask抽象接口及核心实现类代码，搭建任务基础框架
### 输入关键词/问题：C++实现多任务调度器核心类，包含ITask抽象接口、ScheduledTask包装类、TaskFactory工厂类，需兼容一次性、延迟、周期任务，代码符合MFC规范
### AI输出核心内容：
1. ITask抽象接口与ScheduledTask包装类（完整代码）：
```cpp
#pragma once
#include <string>
#include <chrono>
#include <memory>

class ITask {
public:
    enum TaskType { ONETIME, DELAYED, PERIODIC };
    virtual ~ITask() = default;
    virtual std::string GetName() const = 0;
    virtual void Execute() = 0;
    virtual TaskType GetType() const = 0;
};

// 封装任务执行信息
class ScheduledTask {
private:
    std::shared_ptr<ITask> task;
    std::chrono::system_clock::time_point executeTime;
    std::chrono::seconds interval; // 周期任务间隔
    ITask::TaskType type;
    std::string taskId; // 任务唯一标识

public:
    ScheduledTask(std::shared_ptr<ITask> t, ITask::TaskType tp, 
                  std::chrono::system_clock::time_point execTime,
                  std::chrono::seconds intv = std::chrono::seconds(0))
        : task(t), type(tp), executeTime(execTime), interval(intv) {
        // 生成唯一任务ID
        taskId = std::to_string(std::hash<std::string>{}(t->GetName() + 
                std::to_string(std::chrono::system_clock::now().time_since_epoch().count())));
    }

    bool IsDue() const {
        return std::chrono::system_clock::now() >= executeTime;
    }

    void Execute() {
        if (task) task->Execute();
        // 周期任务更新执行时间
        if (type == ITask::PERIODIC) {
            executeTime += interval;
        }
    }

    // 重载比较运算符，用于优先队列排序（时间早的优先）
    bool operator<(const ScheduledTask& other) const {
        return executeTime > other.executeTime;
    }

    // Getter方法
    std::string GetTaskId() const { return taskId; }
    ITask::TaskType GetType() const { return type; }
    std::chrono::system_clock::time_point GetExecuteTime() const { return executeTime; }
};

// TaskFactory工厂类
class TaskFactory {
public:
    static std::shared_ptr<ITask> CreateTask(ITask::TaskType type, const std::string& taskName) {
        switch (type) {
            case ITask::ONETIME:
                return std::make_shared<OneTimeTask>(taskName);
            case ITask::DELAYED:
                return std::make_shared<DelayedTask>(taskName);
            case ITask::PERIODIC:
                return std::make_shared<PeriodicTask>(taskName);
            default:
                throw std::invalid_argument("Invalid task type");
        }
    }
};

// 具体任务实现示例
class OneTimeTask : public ITask {
private:
    std::string name;
public:
    OneTimeTask(const std::string& n) : name(n) {}
    std::string GetName() const override { return name; }
    void Execute() override { /* 一次性任务逻辑 */ }
    TaskType GetType() const override { return ONETIME; }
};

class DelayedTask : public ITask { /* 延迟任务实现 */ };
class PeriodicTask : public ITask { /* 周期任务实现 */ };
```
2. 代码注释：标注关键函数功能、成员变量含义，提示线程安全注意事项。
### 实际应用与反思：
直接复用AI生成的接口与工厂类代码框架，补充具体任务类的头文件声明，调整ScheduledTask的时间计算逻辑以适配Windows系统时间格式（将std::chrono替换为CTime）；AI生成的代码结构规范，但未处理任务取消和状态更新的逻辑，需后续扩展；添加Doxygen注释以满足项目可维护性要求。

## 2025年1月3日 星期五
### 使用时间：09:30-11:00
### AI工具：讯飞星火
### 使用场景/目的：解决TaskScheduler单例实现与任务调度逻辑，确保线程安全
### 输入关键词/问题：C++ MFC环境下TaskScheduler单例模式实现，使用priority_queue管理任务，通过mutex和condition_variable实现线程同步，支持任务添加、执行、状态监听
### AI输出核心内容：
1. TaskScheduler完整实现代码：
```cpp
#include "TaskScheduler.h"
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>

// 懒汉式单例初始化
std::unique_ptr<TaskScheduler> TaskScheduler::instance = nullptr;
std::once_flag TaskScheduler::initFlag;

TaskScheduler& TaskScheduler::GetInstance() {
    std::call_once(initFlag, []() {
        instance.reset(new TaskScheduler());
    });
    return *instance;
}

TaskScheduler::TaskScheduler() {
    isRunning = true;
    // 启动工作线程
    workerThread = std::thread(&TaskScheduler::WorkerLoop, this);
}

TaskScheduler::~TaskScheduler() {
    Stop();
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void TaskScheduler::AddTask(std::shared_ptr<ScheduledTask> task) {
    std::lock_guard<std::mutex> lock(mtx);
    taskQueue.push(task);
    cv.notify_one(); // 唤醒工作线程
    std::cout << "Task added: " << task->GetTaskId() << std::endl;
}

void TaskScheduler::WorkerLoop() {
    while (isRunning) {
        std::unique_lock<std::mutex> lock(mtx);
        
        // 等待有任务或停止信号
        cv.wait(lock, [this]() {
            return !taskQueue.empty() || !isRunning;
        });

        if (!isRunning && taskQueue.empty()) {
            break;
        }

        auto currentTask = taskQueue.top();
        if (currentTask->IsDue()) {
            taskQueue.pop();
            lock.unlock(); // 解锁后执行任务，避免阻塞其他操作
            
            try {
                currentTask->Execute();
                std::cout << "Task executed: " << currentTask->GetTaskId() << std::endl;
                
                // 周期任务重新加入队列
                if (currentTask->GetType() == ITask::PERIODIC) {
                    lock.lock();
                    taskQueue.push(currentTask);
                    lock.unlock();
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Task execution failed: " << e.what() << std::endl;
            }
        }
        else {
            // 等待到任务执行时间或新任务添加
            cv.wait_until(lock, currentTask->GetExecuteTime());
        }
    }
}

void TaskScheduler::Stop() {
    std::lock_guard<std::mutex> lock(mtx);
    isRunning = false;
    cv.notify_all(); // 唤醒所有等待的线程
}

// 任务取消接口
bool TaskScheduler::CancelTask(const std::string& taskId) {
    std::lock_guard<std::mutex> lock(mtx);
    // 优先队列无法直接删除元素，需重建队列
    std::priority_queue<std::shared_ptr<ScheduledTask>> newQueue;
    bool found = false;

    while (!taskQueue.empty()) {
        auto task = taskQueue.top();
        taskQueue.pop();
        if (task->GetTaskId() != taskId) {
            newQueue.push(task);
        }
        else {
            found = true;
        }
    }

    taskQueue.swap(newQueue);
    if (found) cv.notify_one();
    return found;
}
```
2. 线程安全保障：所有访问优先队列的操作均通过std::lock_guard<mutex>加锁，避免数据竞争；
3. 任务添加接口：AddOneTimeTask()、AddDelayedTask()、AddPeriodicTask()，自动计算执行时间并加入队列。
### 实际应用与反思：
参考AI代码实现TaskScheduler核心逻辑，将饿汉式单例改为懒汉式（线程安全版）以优化内存占用；测试发现周期任务执行后未重新加入队列，补充任务重置逻辑；AI提供的线程同步方案可靠，但需添加任务取消接口以满足实际需求，后续需结合Observer模式实现状态通知。

## 2025年1月4日 星期六
### 使用时间：15:00-16:30
### AI工具：文心一言
### 使用场景/目的：实现LogWriter日志类（RAII）与具体任务功能（压缩、矩阵乘法、网络请求）
### 输入关键词/问题：C++ RAII模式实现LogWriter类（自动打开/关闭日志文件），编写CompressTask（压缩文件）、MatrixMultiplyTask（矩阵乘法）、GithubZenTask（网络请求）的Execute()实现，兼容MFC环境
### AI输出核心内容：
1. LogWriter类（RAII）实现：
```cpp
#pragma once
#include <fstream>
#include <string>
#include <chrono>
#include <mutex>
#include <sstream>

class LogWriter {
private:
    std::ofstream logFile;
    std::mutex logMtx;
    std::string fileName;

    // 私有构造函数，禁止外部实例化（可选单例）
    LogWriter(const std::string& fn) : fileName(fn) {
        // 打开日志文件，追加模式
        logFile.open(fileName, std::ios::out | std::ios::app);
        if (!logFile.is_open()) {
            throw std::runtime_error("Failed to open log file: " + fileName);
        }
        WriteLog("LogWriter initialized");
    }

public:
    // 禁用拷贝
    LogWriter(const LogWriter&) = delete;
    LogWriter& operator=(const LogWriter&) = delete;

    // 单例获取
    static LogWriter& GetInstance(const std::string& fileName = "task_scheduler.log") {
        static LogWriter instance(fileName);
        return instance;
    }

    // 析构函数自动关闭文件（RAII核心）
    ~LogWriter() {
        if (logFile.is_open()) {
            WriteLog("LogWriter closing");
            logFile.close();
        }
    }

    // 写日志方法，线程安全
    void WriteLog(const std::string& content) {
        std::lock_guard<std::mutex> lock(logMtx);
        auto now = std::chrono::system_clock::now();
        auto timeT = std::chrono::system_clock::to_time_t(now);
        // 格式化时间戳
        std::stringstream ss;
        ss << "[" << std::ctime(&timeT) << "] " << content << std::endl;
        logFile << ss.str();
        logFile.flush(); // 立即刷盘
    }
};

// 宏定义简化日志调用
#define LOG(content) LogWriter::GetInstance().WriteLog(content)
```
2. 具体任务实现代码：
```cpp
// 压缩任务
class CompressTask : public ITask {
private:
    std::string name;
    std::string filePath;
public:
    CompressTask(const std::string& n, const std::string& path) : name(n), filePath(path) {}
    std::string GetName() const override { return name; }
    TaskType GetType() const override { return ONETIME; }

    void Execute() override {
        LOG("CompressTask start: " + filePath);
        // Windows压缩API调用
        HANDLE hFile = CreateFileA(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, 
                                  NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            LOG("CompressTask failed: file not found - " + filePath);
            return;
        }

        // 简化实现，实际需调用ICompressArchive接口
        LOG("CompressTask success: " + filePath);
        CloseHandle(hFile);
    }
};

// 矩阵乘法任务
class MatrixMultiplyTask : public ITask {
private:
    std::string name;
    const int SIZE = 200;
    using Matrix = std::vector<std::vector<int>>;

    Matrix GenerateRandomMatrix() {
        Matrix mat(SIZE, std::vector<int>(SIZE));
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                mat[i][j] = rand() % 100;
            }
        }
        return mat;
    }

public:
    MatrixMultiplyTask(const std::string& n) : name(n) {}
    std::string GetName() const override { return name; }
    TaskType GetType() const override { return ONETIME; }

    void Execute() override {
        LOG("MatrixMultiplyTask start");
        auto mat1 = GenerateRandomMatrix();
        auto mat2 = GenerateRandomMatrix();
        Matrix result(SIZE, std::vector<int>(SIZE, 0));

        // 矩阵乘法计算
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < SIZE; ++i) {
            for (int j = 0; j < SIZE; ++j) {
                for (int k = 0; k < SIZE; ++k) {
                    result[i][j] += mat1[i][k] * mat2[k][j];
                }
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        LOG("MatrixMultiplyTask success: " + std::to_string(duration) + "ms");
    }
};

// Github Zen网络请求任务
class GithubZenTask : public ITask {
private:
    std::string name;
public:
    GithubZenTask(const std::string& n) : name(n) {}
    std::string GetName() const override { return name; }
    TaskType GetType() const override { return PERIODIC; }

    void Execute() override {
        LOG("GithubZenTask start");
        // WinINet网络请求
        HINTERNET hInternet = InternetOpenA("TaskScheduler/1.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!hInternet) {
            LOG("GithubZenTask failed: InternetOpen failed");
            return;
        }

        HINTERNET hConnect = InternetOpenUrlA(hInternet, "https://api.github.com/zen", 
                                             NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE, 0);
        if (!hConnect) {
            InternetCloseHandle(hInternet);
            LOG("GithubZenTask failed: InternetOpenUrl failed");
            return;
        }

        // 读取响应
        char buffer[1024];
        DWORD bytesRead = 0;
        std::string zenContent;
        while (InternetReadFile(hConnect, buffer, sizeof(buffer)-1, &bytesRead) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            zenContent += buffer;
        }

        // 写入文件
        std::ofstream zenFile("zen.txt", std::ios::app);
        if (zenFile.is_open()) {
            zenFile << zenContent << std::endl;
            zenFile.close();
        }

        LOG("GithubZenTask success: " + zenContent);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
    }
};
```
### 实际应用与反思：
复用LogWriter类代码，补充日志格式标准化（去除ctime的换行符，统一时间格式为YYYY-MM-DD HH:MM:SS）；修改CompressTask的路径处理逻辑，避免硬编码，通过MFC CFileDialog获取用户指定路径；MatrixMultiplyTask添加矩阵数据结构的智能指针封装，防止内存泄漏；AI生成的网络请求代码未处理编码问题，补充UTF-8转GBK逻辑以适配Windows中文环境。

## 2025年1月5日 星期日
### 使用时间：10:30-12:00
### AI工具：豆包
### 使用场景/目的：实现Observer模式的任务状态通知与跨线程UI更新，解决多线程交互问题
### 输入关键词/问题：C++ MFC多任务调度器实现Observer模式，UI层订阅任务状态变化（开始、完成、异常），通过跨线程消息映射更新UI，避免线程安全问题
### AI输出核心内容：
1. Observer模式核心代码：
```cpp
// ITaskObserver.h
#pragma once
#include <string>

class ITaskObserver {
public:
    virtual ~ITaskObserver() = default;
    virtual void OnTaskStart(const std::string& taskId, const std::string& taskName) = 0;
    virtual void OnTaskComplete(const std::string& taskId, const std::string& taskName) = 0;
    virtual void OnTaskError(const std::string& taskId, const std::string& taskName, const std::string& errorMsg) = 0;
    virtual void OnTaskProgress(const std::string& taskId, int progress) = 0; // 扩展进度通知
};

// 扩展TaskScheduler支持观察者
// TaskScheduler.h补充代码
#include <vector>
#include <algorithm>
#include "ITaskObserver.h"

class TaskScheduler {
    // ... 原有成员 ...
private:
    std::vector<ITaskObserver*> observers;
public:
    void AttachObserver(ITaskObserver* observer) {
        if (observer && std::find(observers.begin(), observers.end(), observer) == observers.end()) {
            observers.push_back(observer);
        }
    }

    void DetachObserver(ITaskObserver* observer) {
        auto it = std::find(observers.begin(), observers.end(), observer);
        if (it != observers.end()) {
            observers.erase(it);
        }
    }

    // 通知观察者
    void NotifyTaskStart(const std::string& taskId, const std::string& taskName) {
        for (auto obs : observers) {
            obs->OnTaskStart(taskId, taskName);
        }
    }

    void NotifyTaskComplete(const std::string& taskId, const std::string& taskName) {
        for (auto obs : observers) {
            obs->OnTaskComplete(taskId, taskName);
        }
    }

    void NotifyTaskError(const std::string& taskId, const std::string& taskName, const std::string& errorMsg) {
        for (auto obs : observers) {
            obs->OnTaskError(taskId, taskName, errorMsg);
        }
    }

    void NotifyTaskProgress(const std::string& taskId, int progress) {
        for (auto obs : observers) {
            obs->OnTaskProgress(taskId, progress);
        }
    }
};

// 修改WorkerLoop中的任务执行逻辑，添加通知
void TaskScheduler::WorkerLoop() {
    while (isRunning) {
        // ... 原有逻辑 ...
        if (currentTask->IsDue()) {
            taskQueue.pop();
            lock.unlock();
            
            try {
                std::string taskId = currentTask->GetTaskId();
                std::string taskName = currentTask->GetTask()->GetName();
                NotifyTaskStart(taskId, taskName); // 任务开始通知
                
                currentTask->Execute();
                NotifyTaskComplete(taskId, taskName); // 任务完成通知
                
                if (currentTask->GetType() == ITask::PERIODIC) {
                    lock.lock();
                    taskQueue.push(currentTask);
                    lock.unlock();
                }
            }
            catch (const std::exception& e) {
                std::string errMsg = e.what();
                NotifyTaskError(currentTask->GetTaskId(), currentTask->GetTask()->GetName(), errMsg);
                std::cerr << "Task execution failed: " << errMsg << std::endl;
            }
        }
        // ... 原有逻辑 ...
    }
}
```
2. MFC跨线程UI更新代码（MainFrame.h/cpp）：
```cpp
// MainFrame.h
#include "ITaskObserver.h"

#define WM_TASK_UPDATE (WM_USER + 101) // 自定义消息

class CMainFrame : public CFrameWnd, public ITaskObserver {
    DECLARE_MESSAGE_MAP()
public:
    CMainFrame();
    ~CMainFrame() override;

    // ITaskObserver接口实现
    void OnTaskStart(const std::string& taskId, const std::string& taskName) override;
    void OnTaskComplete(const std::string& taskId, const std::string& taskName) override;
    void OnTaskError(const std::string& taskId, const std::string& taskName, const std::string& errorMsg) override;
    void OnTaskProgress(const std::string& taskId, int progress) override;

    // 消息处理函数
    afx_msg LRESULT OnTaskUpdate(WPARAM wParam, LPARAM lParam);

private:
    // UI控件指针
    CListCtrl* m_pTaskList;
    CStatic* m_pStatusLabel;
};

// MainFrame.cpp
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_MESSAGE(WM_TASK_UPDATE, &CMainFrame::OnTaskUpdate)
END_MESSAGE_MAP()

CMainFrame::CMainFrame() {
    // 初始化UI控件
    m_pTaskList = (CListCtrl*)GetDlgItem(IDC_LIST_TASKS);
    m_pStatusLabel = (CStatic*)GetDlgItem(IDC_STATIC_STATUS);
    
    // 注册为观察者
    TaskScheduler::GetInstance().AttachObserver(this);
}

// 观察者接口实现（任务线程中调用）
void CMainFrame::OnTaskStart(const std::string& taskId, const std::string& taskName) {
    // 封装数据，通过PostMessage发送到主线程
    std::tuple<std::string, std::string, std::string> data = {taskId, taskName, "start"};
    PostMessage(WM_TASK_UPDATE, 0, (LPARAM)new decltype(data)(data));
}

void CMainFrame::OnTaskComplete(const std::string& taskId, const std::string& taskName) {
    std::tuple<std::string, std::string, std::string> data = {taskId, taskName, "complete"};
    PostMessage(WM_TASK_UPDATE, 0, (LPARAM)new decltype(data)(data));
}

void CMainFrame::OnTaskError(const std::string& taskId, const std::string& taskName, const std::string& errorMsg) {
    std::tuple<std::string, std::string, std::string> data = {taskId, taskName, errorMsg};
    PostMessage(WM_TASK_UPDATE, 0, (LPARAM)new decltype(data)(data));
}

void CMainFrame::OnTaskProgress(const std::string& taskId, int progress) {
    std::tuple<std::string, std::string, int> data = {taskId, "progress", progress};
    PostMessage(WM_TASK_UPDATE, 1, (LPARAM)new decltype(data)(data));
}

// 主线程UI更新处理
LRESULT CMainFrame::OnTaskUpdate(WPARAM wParam, LPARAM lParam) {
    if (wParam == 0) {
        // 状态更新
        auto* data = (std::tuple<std::string, std::string, std::string>*)lParam;
        auto [taskId, taskName, status] = *data;
        delete data;

        // 更新列表控件
        int nItem = m_pTaskList->InsertItem(0, CString(taskId.c_str()));
        m_pTaskList->SetItemText(nItem, 1, CString(taskName.c_str()));
        m_pTaskList->SetItemText(nItem, 2, CString(status.c_str()));

        // 更新状态标签
        m_pStatusLabel->SetWindowText(CString(("Task " + taskName + " " + status).c_str()));
    }
    else if (wParam == 1) {
        // 进度更新
        auto* data = (std::tuple<std::string, std::string, int>*)lParam;
        auto [taskId, _, progress] = *data;
        delete data;

        // 查找任务并更新进度
        for (int i = 0; i < m_pTaskList->GetItemCount(); ++i) {
            if (m_pTaskList->GetItemText(i, 0) == CString(taskId.c_str())) {
                m_pTaskList->SetItemText(i, 3, CString(std::to_string(progress).c_str()));
                break;
            }
        }
    }
    return 0;
}
```
### 实际应用与反思：
按AI方案实现Observer模式，在MainFrame类中实现观察者接口，成功实现任务状态的实时UI反馈；优化消息映射宏定义，确保自定义消息正确注册（补充DECLARE_MESSAGE_MAP()和BEGIN_MESSAGE_MAP()）；AI提供的跨线程更新方案有效，避免了直接操作UI导致的崩溃问题；补充任务进度显示功能，扩展OnTaskProgress()接口以提升用户体验，在MatrixMultiplyTask中添加进度更新逻辑（每完成10%矩阵计算调用NotifyTaskProgress）。

## 2025年1月6日 星期一
### 使用时间：13:00-15:00
### AI工具：ChatGPT
### 使用场景/目的：编写单元测试（GoogleTest）、修复代码缺陷、完善项目文档
### 输入关键词/问题：C++ 多任务调度器单元测试（GoogleTest），覆盖TaskFactory、TaskScheduler、LogWriter核心类，修复线程安全与异常处理缺陷，生成Doxygen文档注释
### AI输出核心内容：
1. GoogleTest单元测试代码（TaskSchedulerTest.cpp）：
```cpp
#include <gtest/gtest.h>
#include "TaskScheduler.h"
#include "TaskFactory.h"
#include "LogWriter.h"
#include <chrono>
#include <thread>

// TaskFactory测试用例
TEST(TaskFactoryTest, CreateValidTask) {
    // 测试创建一次性任务
    auto task = TaskFactory::CreateTask(ITask::ONETIME, "TestOneTimeTask");
    ASSERT_NE(task, nullptr);
    EXPECT_EQ(task->GetType(), ITask::ONETIME);
    EXPECT_EQ(task->GetName(), "TestOneTimeTask");

    // 测试创建周期任务
    auto periodicTask = TaskFactory::CreateTask(ITask::PERIODIC, "TestPeriodicTask");
    ASSERT_NE(periodicTask, nullptr);
    EXPECT_EQ(periodicTask->GetType(), ITask::PERIODIC);
}

TEST(TaskFactoryTest, CreateInvalidTask) {
    // 测试无效任务类型
    EXPECT_THROW(TaskFactory::CreateTask((ITask::TaskType)99, "InvalidTask"), std::invalid_argument);
}

// TaskScheduler测试用例
TEST(TaskSchedulerTest, AddAndExecuteOneTimeTask) {
    auto& scheduler = TaskScheduler::GetInstance();
    bool taskExecuted = false;

    // 自定义测试任务
    class TestTask : public ITask {
    public:
        bool* flag;
        TestTask(bool* f) : flag(f) {}
        std::string GetName() const override { return "TestTask"; }
        TaskType GetType() const override { return ONETIME; }
        void Execute() override { *flag = true; }
    };

    auto testTask = std::make_shared<ScheduledTask>(
        std::make_shared<TestTask>(&taskExecuted),
        ITask::ONETIME,
        std::chrono::system_clock::now()
    );

    scheduler.AddTask(testTask);
    std::this_thread::sleep_for(std::chrono::seconds(1)); // 等待任务执行
    EXPECT_TRUE(taskExecuted);
}

TEST(TaskSchedulerTest, PeriodicTaskExecution) {
    auto& scheduler = TaskScheduler::GetInstance();
    int executeCount = 0;

    class PeriodicTestTask : public ITask {
    public:
        int* count;
        PeriodicTestTask(int* c) : count(c) {}
        std::string GetName() const override { return "PeriodicTestTask"; }
        TaskType GetType() const override { return PERIODIC; }
        void Execute() override { (*count)++; }
    };

    auto testTask = std::make_shared<ScheduledTask>(
        std::make_shared<PeriodicTestTask>(&executeCount),
        ITask::PERIODIC,
        std::chrono::system_clock::now(),
        std::chrono::seconds(1) // 1秒周期
    );

    scheduler.AddTask(testTask);
    std::this_thread::sleep_for(std::chrono::seconds(3)); // 等待3秒
    scheduler.CancelTask(testTask->GetTaskId()); // 取消任务
    EXPECT_GE(executeCount, 2); // 至少执行2次
}

// LogWriter测试用例
TEST(LogWriterTest, WriteLogToFile) {
    std::string logFileName = "test_log.log";
    auto& logWriter = LogWriter::GetInstance(logFileName);
    
    // 写入测试日志
    std::string testContent = "Test log message";
    logWriter.WriteLog(testContent);

    // 验证文件存在且内容正确
    std::ifstream logFile(logFileName);
    ASSERT_TRUE(logFile.is_open());
    
    std::string line;
    bool found = false;
    while (std::getline(logFile, line)) {
        if (line.find(testContent) != std::string::npos) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
    logFile.close();

    // 清理测试文件
    remove(logFileName.c_str());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```
2. 缺陷修复代码（补充异常处理与资源释放）：
```cpp
// TaskScheduler.cpp补充异常处理
void TaskScheduler::WorkerLoop() {
    while (isRunning) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]() { return !taskQueue.empty() || !isRunning; });

        if (!isRunning && taskQueue.empty()) break;

        auto currentTask = taskQueue.top();
        if (currentTask->IsDue()) {
            taskQueue.pop();
            lock.unlock();
            
            try {
                std::string taskId = currentTask->GetTaskId();
                std::string taskName = currentTask->GetTask()->GetName();
                NotifyTaskStart(taskId, taskName);
                
                currentTask->Execute();
                NotifyTaskComplete(taskId, taskName);
                
                if (currentTask->GetType() == ITask::PERIODIC) {
                    lock.lock();
                    taskQueue.push(currentTask);
                    lock.unlock();
                }
            }
            catch (const std::exception& e) {
                std::string errMsg = e.what();
                NotifyTaskError(currentTask->GetTaskId(), currentTask->GetTask()->GetName(), errMsg);
                LOG("Task error: " + errMsg);
            }
            catch (...) { // 捕获所有异常
                NotifyTaskError(currentTask->GetTaskId(), currentTask->GetTask()->GetName(), "Unknown error");
                LOG("Task unknown error");
            }
        }
        else {
            cv.wait_until(lock, currentTask->GetExecuteTime());
        }
    }
}

// LogWriter.cpp补充线程安全写入
void LogWriter::WriteLog(const std::string& content) {
    std::lock_guard<std::mutex> lock(logMtx);
    if (!logFile.is_open()) {
        // 重新打开文件（处理文件意外关闭）
        logFile.open(fileName, std::ios::out | std::ios::app);
        if (!logFile.is_open()) return;
    }

    // 格式化时间戳（修复换行问题）
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    tm tmBuf;
    localtime_s(&tmBuf, &timeT); // 线程安全的时间转换
    
    char timeStr[32];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &tmBuf);
    
    logFile << "[" << timeStr << "] " << content << std::endl;
    logFile.flush();
}
```
3. Doxygen文档注释示例（ITask.h）：
```cpp
/**
 * @file ITask.h
 * @brief 任务抽象接口定义（Command设计模式）
 * @author TaskScheduler Dev Team
 * @date 2025-01-02
 * 
 * 该头文件定义了所有任务的基类ITask，以及任务类型枚举，
 * 所有具体任务类需继承该接口并实现纯虚函数。
 */

/**
 * @class ITask
 * @brief 任务抽象接口
 * 
 * 定义了任务的核心行为，包括获取任务名称、执行任务、获取任务类型，
 * 遵循Command设计模式，封装不同类型任务的执行逻辑。
 */
class ITask {
public:
    /**
     * @enum TaskType
     * @brief 任务类型枚举
     * @var ITask::ONETIME 一次性任务，执行一次后结束
     * @var ITask::DELAYED 延迟任务，指定时间后执行一次
     * @var ITask::PERIODIC 周期任务，按指定间隔重复执行
     */
    enum TaskType { ONETIME, DELAYED, PERIODIC };

    /**
     * @brief 虚析构函数，确保子类析构函数被正确调用
     */
    virtual ~ITask() = default;

    /**
     * @brief 获取任务名称
     * @return 任务名称字符串
     */
    virtual std::string GetName() const = 0;

    /**
     * @brief 执行任务核心逻辑
     * @note 该方法会在任务调度器的工作线程中执行，需保证线程安全
     * @throw std::exception 任务执行过程中可能抛出的异常
     */
    virtual void Execute() = 0;

    /**
     * @brief 获取任务类型
     * @return 任务类型枚举值
     */
    virtual TaskType GetType() const = 0;
};
```
### 实际应用与反思：
基于AI生成的单元测试框架，补充测试用例至覆盖率≥70%，新增“任务取消功能测试”和“跨线程UI更新测试”，发现并修复了周期任务重复添加时的时间计算错误（原逻辑未考虑系统时间偏差）；按建议完善异常处理逻辑，添加catch(...)捕获未知异常，确保所有GDI句柄、文件句柄通过RAII正确释放；完成Doxygen注释与项目文档（README.md、Architecture.md），其中README包含编译步骤、功能说明、测试方法，Architecture.md包含类图和核心流程；AI在测试用例设计和缺陷排查上提供了高效支持，但需结合项目实际场景调整测试逻辑（如缩短周期任务测试的等待时间），避免过度依赖通用方案。

### 总结
1. 本次AI使用覆盖了多任务调度器从架构设计、核心类实现、线程安全保障到UI交互、单元测试的全流程，每个环节均生成了可直接复用的代码片段，且代码符合C++/MFC开发规范；
2. AI输出的代码需结合项目实际场景调整（如Windows系统API适配、编码转换、UI控件交互），并补充异常处理、资源释放等工程化细节；
3. 单元测试和文档注释部分AI提供了标准化模板，显著提升了项目的可测试性和可维护性，核心缺陷（如线程安全、跨线程UI崩溃）均通过AI提示得到解决。