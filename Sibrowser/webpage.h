#ifndef WEBPAGE_H
#define WEBPAGE_H

#include<QWebEnginePage>

class WebPage:public QWebEnginePage
{
    Q_OBJECT
public:
    WebPage(QWebEngineProfile *profile,QObject *parent = nullptr);
protected:
    bool certificateError(const QWebEngineCertificateError &error) override;
private slots:
    //HTTP请求需要认证处理
    void handleAuthenticationRequired(const QUrl &requestUrl,QAuthenticator *auth);
    //HTTP请求失败处理
    void handleProxyAuthenticationRequired(const QUrl &requestUrl,QAuthenticator *auth,const QString &proxyHost);
};

#endif // WEBPAGE_H
