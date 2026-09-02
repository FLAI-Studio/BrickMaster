#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAgreeClicked();
    void onConfirmClicked();
    void onDeclineClicked();
    void onCountdownTick();
    void onOccupyTick();

private:
    // UI
    class QLabel *titleLabel;
    class QLabel *infoLabel;
    class QLabel *warnLabel;
    class QLabel *countdownLabel;
    class QPushButton *agreeBtn;
    class QPushButton *declineBtn;
    class QPushButton *confirmBtn;
    class QTextEdit *logView;

    // 数据
    int countdownValue;
    int coins;
    int runTicks;

    QTimer *countdownTimer;
    QTimer *occupyTimer;
};

#endif // MAINWINDOW_H