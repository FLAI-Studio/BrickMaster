#include "mainwindow.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QDateTime>
#include <QElapsedTimer>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("卡机砖家 BrickMaster");
    resize(560, 580);

    // ==================== 数据初始化 ====================
    countdownValue = 10;
    totalMB = 0;
    runTicks = 0;

    // ==================== 全局暗黑背景 ====================
    QWidget* central = new QWidget(this);
    central->setStyleSheet(
        "QWidget { background-color: #0f0f0f; color: #e0e0e0; }"
        );
    setCentralWidget(central);

    QVBoxLayout* layout = new QVBoxLayout(central);
    layout->setSpacing(12);
    layout->setContentsMargins(20, 16, 20, 16);

    // ---- 标题 ----
    titleLabel = new QLabel("💀 卡机砖家 BrickMaster");
    titleLabel->setStyleSheet(
        "font-size: 26px; font-weight: bold; color: #e53935;"
        "background: transparent;"
        );
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    // ---- 版本信息 ----
    infoLabel = new QLabel("版本：0.0.1\n开发者：Byjsmc\n最后更新于：2026/09/03");
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setStyleSheet("color: #666; font-size: 12px; background: transparent;");
    layout->addWidget(infoLabel);

    // ---- 免责声明 GroupBox ----
    QGroupBox* warnGroup = new QGroupBox("⚠ 免责声明");
    warnGroup->setStyleSheet(
        "QGroupBox {"
        "  background-color: #1e1e1e;"
        "  border: 1px solid #444;"
        "  border-radius: 8px;"
        "  color: #ff9800;"
        "  font-weight: bold;"
        "  font-size: 14px;"
        "  margin-top: 8px;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 14px;"
        "  padding: 0 6px;"
        "}"
        );
    QVBoxLayout* warnLayout = new QVBoxLayout(warnGroup);
    warnLayout->setContentsMargins(14, 18, 14, 14);

    warnLabel = new QLabel(
        "本作品仅供学习研究使用，对造成的后果开发者不承担任何责任！\n\n"
        "该程序会持续强制占用物理内存，直到系统资源耗尽崩溃或无响应。\n"
        "执行后系统将逐渐卡死，请问您是否同意以上免责声明？"
        );
    warnLabel->setWordWrap(true);
    warnLabel->setStyleSheet(
        "font-size: 14px;"
        "line-height: 1.8;"
        "color: #f0f0f0;"          // ← 近白色，暗底上清晰可读
        "background: transparent;"
        );
    warnLayout->addWidget(warnLabel);
    layout->addWidget(warnGroup);

    // ---- 按钮行 ----
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);
    agreeBtn = new QPushButton("我同意");
    declineBtn = new QPushButton("我拒绝");
    agreeBtn->setMinimumHeight(40);
    declineBtn->setMinimumHeight(40);
    agreeBtn->setStyleSheet(
        "QPushButton {"
        "  background: #2e7d32; color: white;"
        "  border-radius: 8px; font-size: 15px;"
        "}"
        "QPushButton:pressed { background: #1b5e20; }"
        );
    declineBtn->setStyleSheet(
        "QPushButton {"
        "  background: #555; color: white;"
        "  border-radius: 8px; font-size: 15px;"
        "}"
        "QPushButton:pressed { background: #333; }"
        );
    btnLayout->addWidget(agreeBtn);
    btnLayout->addWidget(declineBtn);
    layout->addLayout(btnLayout);

    // ---- 确认执行按钮 ----
    confirmBtn = new QPushButton("⚡ 我已知晓风险，立即执行");
    confirmBtn->setMinimumHeight(46);
    confirmBtn->setStyleSheet(
        "QPushButton {"
        "  background: #e53935; color: white;"
        "  border-radius: 10px; font-size: 17px; font-weight: bold;"
        "}"
        "QPushButton:pressed { background: #b71c1c; }"
        );
    confirmBtn->hide();
    layout->addWidget(confirmBtn);

    // ---- 倒计时 ----
    countdownLabel = new QLabel("");
    countdownLabel->setAlignment(Qt::AlignCenter);
    countdownLabel->setStyleSheet(
        "font-size: 16px; color: #ff9800; font-weight: bold;"
        "background: transparent;"
        );
    countdownLabel->hide();
    layout->addWidget(countdownLabel);

    // ---- 实时统计 ----
    statsLabel = new QLabel("");
    statsLabel->setAlignment(Qt::AlignCenter);
    statsLabel->setStyleSheet(
        "font-size: 13px; color: #00ff41;"
        "font-family: Consolas, monospace;"
        "background: transparent;"
        );
    statsLabel->hide();
    layout->addWidget(statsLabel);

    // ---- 运行日志 ----
    logView = new QTextEdit();
    logView->setReadOnly(true);
    logView->setPlaceholderText("运行日志将显示在这里...");
    logView->setStyleSheet(
        "QTextEdit {"
        "  background: #0a0a0a;"
        "  color: #00ff41;"
        "  font-family: Consolas, monospace;"
        "  font-size: 12px;"
        "  border: 1px solid #333;"
        "  border-radius: 6px;"
        "}"
        );
    logView->hide();
    layout->addWidget(logView);

    // ==================== 定时器 ====================
    countdownTimer = new QTimer(this);
    countdownTimer->setInterval(1000);
    occupyTimer = new QTimer(this);
    occupyTimer->setInterval(50);

    connect(agreeBtn, &QPushButton::clicked, this, &MainWindow::onAgreeClicked);
    connect(declineBtn, &QPushButton::clicked, this, &MainWindow::onDeclineClicked);
    connect(confirmBtn, &QPushButton::clicked, this, &MainWindow::onConfirmClicked);
    connect(countdownTimer, &QTimer::timeout, this, &MainWindow::onCountdownTick);
    connect(occupyTimer, &QTimer::timeout, this, &MainWindow::onOccupyTick);
}

MainWindow::~MainWindow() {
    for (auto p : memBlocks) delete[] p;
}

// ==================== 槽函数（逻辑不变） ====================

void MainWindow::onAgreeClicked() {
    agreeBtn->hide();
    declineBtn->hide();

    warnLabel->setText(
        "⚠ 最后一次警告 ⚠\n\n"
        "程序将开始持续强制写入物理内存。\n"
        "每次 50MB，无上限，直到系统崩溃。\n\n"
        "执行期间你仍然可以试着关闭窗口——\n"
        "如果系统还响应得过来的话。\n\n"
        "是否确认执行？"
        );
    confirmBtn->show();
}

void MainWindow::onDeclineClicked() {
    logView->show();
    logView->append("用户拒绝了免责声明，程序退出。");
    agreeBtn->setEnabled(false);
    declineBtn->setEnabled(false);
    confirmBtn->hide();
}

void MainWindow::onConfirmClicked() {
    confirmBtn->hide();
    countdownLabel->show();
    logView->show();
    statsLabel->show();

    logView->append("==============================");
    logView->append("  BrickMaster v0.0.1 启动");
    logView->append("  狂暴模式（50MB/次，无上限）");
    logView->append("==============================");

    elapsed.start();
    countdownValue = 10;
    onCountdownTick();
    countdownTimer->start();
}

void MainWindow::onCountdownTick() {
    if (countdownValue > 0) {
        countdownLabel->setText(QString("程序将在 %1 秒后执行...\n跑还来得及关窗口").arg(countdownValue));
        logView->append(QString("[倒计时] %1s...").arg(countdownValue));
        countdownValue--;
    } else {
        countdownTimer->stop();
        countdownLabel->setText("你已经被警告过了...... 💀");
        logView->append("");
        logView->append(">> 开始强占物理内存 <<");
        occupyTimer->start();
    }
}

void MainWindow::onOccupyTick() {
    const int blockMB = 50;
    size_t bytes = blockMB * 1024 * 1024;

    char* p = new char[bytes];
    for (size_t i = 0; i < bytes; i += 4096) {
        p[i] = 0xAA;
    }

    memBlocks.append(p);
    totalMB += blockMB;
    runTicks++;

    qint64 ms = elapsed.elapsed();
    double seconds = ms / 1000.0;
    double rate = (seconds > 0) ? totalMB / seconds : 0;

    statsLabel->setText(QString("已占用: %1 MB (%2 GB)  |  速率: %3 MB/s  |  申请: %4 次")
                            .arg(totalMB)
                            .arg(totalMB / 1024.0, 0, 'f', 2)
                            .arg(rate, 0, 'f', 1)
                            .arg(runTicks));

    if (runTicks % 5 == 0) {
        QString timeStr = QDateTime::currentDateTime().toString("hh:mm:ss");
        logView->append(QString("[%1] +%2MB → 累计 %3 MB (%4 GB)")
                            .arg(timeStr).arg(blockMB).arg(totalMB).arg(totalMB / 1024.0, 0, 'f', 2));
        QScrollBar* sb = logView->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
}
