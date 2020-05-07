#include "webview.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WebView w;
    w.show();
    return a.exec();
}
