#include "mainwindow.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QDateTime>
#include <cstdlib>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // ===== 窗口基本设置 =====
    setWindowTitle("卡机砖家");
    resize(520, 500);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 16, 16, 16);

    // ===== 标题 =====
    titleLabel = new QLabel("💀 卡机砖家");
    titleLabel->setStyleSheet("font-size: 26px; font-weight: bold; color: #c00;");
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    // ===== 版本信息 =====
    infoLabel = new QLabel(
        "版本: v0.0.1-rc1\n"
        "最后更新: 2026/09/03\n"
        "开发者: Byjsmc"
    );
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setStyleSheet("color: #666; font-size: 13px;");
    layout->addWidget(infoLabel);

    // ===== 免责声明 =====
    QGroupBox *warnGroup = new QGroupBox("⚠ 免责声明");
    QVBoxLayout *warnLayout = new QVBoxLayout(warnGroup);
    warnLabel = new QLabel(
        "本作品仅供学习研究使用，对造成的后果开发者不承担任何责任！\n\n"
        "该程序会模拟大量占用系统资源，可能导致系统卡顿甚至无响应。\n"
        "请问您是否同意以上免责声明？"
    );
    warnLabel->setWordWrap(true);
    warnLabel->setStyleSheet("font-size: 14px; line-height: 1.6;");
    warnLayout->addWidget(warnLabel);
    layout->addWidget(warnGroup);

    // ===== 按钮行（同意/拒绝）=====
    QHBoxLayout *btnLayout = new QHBoxLayout();
    agreeBtn = new QPushButton("我同意");
    declineBtn = new QPushButton("我拒绝");
    agreeBtn->setMinimumHeight(40);
    declineBtn->setMinimumHeight(40);
    agreeBtn->setStyleSheet("background: #5b8def; color: white; border-radius: 8px; font-size: 15px;");
    declineBtn->setStyleSheet("background: #999; color: white; border-radius: 8px; font-size: 15px;");
    btnLayout->addWidget(agreeBtn);
    btnLayout->addWidget(declineBtn);
    layout->addLayout(btnLayout);

    // ===== 确认执行按钮（初始隐藏）=====
    confirmBtn = new QPushButton("⚡ 我已知晓风险，立即执行");
    confirmBtn->setMinimumHeight(44);
    confirmBtn->setStyleSheet(
        "background: #c00; color: white; border-radius: 8px; font-size: 16px; font-weight: bold;"
    );
    confirmBtn->hide();
    layout->addWidget(confirmBtn);

    // ===== 倒计时标签 =====
    countdownLabel = new QLabel("");
    countdownLabel->setAlignment(Qt::AlignCenter);
    countdownLabel->setStyleSheet("font-size: 18px; color: #e67e22; font-weight: bold;");
    countdownLabel->hide();
    layout->addWidget(countdownLabel);

    // ===== 运行日志 =====
    logView = new QTextEdit();
    logView->setReadOnly(true);
    logView->setPlaceholderText("运行日志将显示在这里...");
    logView->setStyleSheet("background: #1e1e1e; color: #0f0; font-family: Consolas, monospace; font-size: 13px;");
    logView->hide();
    layout->addWidget(logView);

    // ===== 定时器 =====
    countdownTimer = new QTimer(this);
    countdownTimer->setInterval(1000);
    occupyTimer = new QTimer(this);
    occupyTimer->setInterval(62);  // 约每62ms刷新一次

    connect(agreeBtn, &QPushButton::clicked, this, &MainWindow::onAgreeClicked);
    connect(declineBtn, &QPushButton::clicked, this, &MainWindow::onDeclineClicked);
    connect(confirmBtn, &QPushButton::clicked, this, &MainWindow::onConfirmClicked);
    connect(countdownTimer, &QTimer::timeout, this, &MainWindow::onCountdownTick);
    connect(occupyTimer, &QTimer::timeout, this, &MainWindow::onOccupyTick);
}

MainWindow::~MainWindow() {}

void MainWindow::onAgreeClicked() {
    // 隐藏同意/拒绝，显示确认按钮
    agreeBtn->hide();
    declineBtn->hide();
    warnLabel->setText(
        "⚠ 最后一次警告 ⚠\n\n"
        "您即将执行此程序，程序会模拟大量占用内存。\n"
        "执行期间您仍可以通过关闭窗口来终止程序。\n\n"
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
    logView->append("==============================");
    logView->append("  卡机砖家 启动序列");
    logView->append("==============================");

    countdownValue = 10;
    onCountdownTick();  // 立即显示第一次
    countdownTimer->start();
}

void MainWindow::onCountdownTick() {
    if (countdownValue > 0) {
        countdownLabel->setText(QString("程序将在 %1 秒后执行...\n期间可关闭窗口终止！").arg(countdownValue));
        logView->append(QString("[倒计时] %1 秒...").arg(countdownValue));
        countdownValue--;
    } else {
        countdownTimer->stop();
        countdownLabel->setText("你已经被警告过了...... 💀");
        logView->append("");
        logView->append(">> 执行开始 <<");

        // 开始"强占内存"
        coins = 0;
        runTicks = 0;
        occupyTimer->start();
    }
}

void MainWindow::onOccupyTick() {
    // 每次申请 50MB，并且全部写入 0xAA（强制占用物理内存）
    size_t mb = 50;
    size_t bytes = mb * 1024 * 1024;

    char *p = new char[bytes];
    // 写入数据，确保物理内存真的被分配
    for (size_t i = 0; i < bytes; i += 4096) {
        p[i] = 0xAA;
    }

    coins += mb;
    runTicks++;

    QDateTime now = QDateTime::currentDateTime();
    QString timeStr = now.toString("hh:mm:ss");

    QString msg = QString("[%1] 已强占 %2 MB 物理内存（累计 %3 MB）")
                      .arg(timeStr)
                      .arg(mb)
                      .arg(coins);

    logView->append(msg);

    // 自动滚动
    QScrollBar *sb = logView->verticalScrollBar();
    sb->setValue(sb->maximum());
}
