#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include<QMainWindow>
#include<QWebEnginePage>

class

class BrowserWindow:public QMainWindow
{
    Q_OBJECT//提供信号槽机制

public:
    BrowserWindow(QWidget *parent = nullptr,Qt::WindowFlags flag=0);
    ~BrowserWindow();
    //设置初始窗口大小
    QSize sizeHint() const override;
};

#endif // BROWSERWINDOW_H
