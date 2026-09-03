#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <QElapsedTimer>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onAgreeClicked();
    void onDeclineClicked();
    void onConfirmClicked();
    void onCountdownTick();
    void onOccupyTick();

private:
    // ---- UI ----
    class QLabel* titleLabel;
    class QLabel* infoLabel;
    class QLabel* warnLabel;
    class QLabel* countdownLabel;
    class QLabel* statsLabel;
    class QPushButton* agreeBtn;
    class QPushButton* declineBtn;
    class QPushButton* confirmBtn;
    class QTextEdit* logView;

    // ---- 数据 ----
    int countdownValue;
    qint64 totalMB;
    int runTicks;
    QVector<char*> memBlocks;
    QElapsedTimer elapsed;

    // ---- 定时器 ----
    QTimer* countdownTimer;
    QTimer* occupyTimer;
};

#endif // MAINWINDOW_H
