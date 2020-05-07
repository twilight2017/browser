#include "webpage.h"
#include "ui_certificateerrordialog.h"
#include "ui_passworddialog.h"
#include<QAuthenticator>
#include<QMessageBox>
#include<QWebEngineCertificateError>
#include<QStyle>
#include<QMessageBox>

WebPage::WebPage(QWebEngineProfile *profile,QObject  *parent):QWebEnginePage(profile,parent)
{
     connect(this,&QWebEnginePage::authenticationRequired,this,&WebPage::handleAuthenticationRequired);
     connect(this,&QWebEnginePage::proxyAuthenticationRequired,this,&WebPage::handleProxyAuthenticationRequired);
}

bool WebPage::certificateError(const QWebEngineCertificateError &error)
{
  QWidget *mainWindow=view()->window();

  if(error.isOverridable()){
      QDialog dialog(mainWindow);
      dialog.setModal(true);
      dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

      Ui::CertificateErrorDialog certificateDialog;
      certificateDialog.setupUi(&dialog);
      certificateDialog.m_iconLabel->setText(QString());
      QIcon icon(mainWindow->style()->standardIcon(QStyle ::SP_MessageBoxWarning,0,mainWindow));
      //手动缩放
      certificateDialog.m_iconLabel->setPixmap(icon.pixmap(32,32));
      certificateDialog.m_iconLabel->setText(error.errorDescription());
      dialog.setWindowTitle(tr("Certificate Error"));
      return dialog.exec() == QDialog::Accepted;

  }
  QMessageBox::critical(mainWindow,tr("Certificate Error!"),error.errorDescription());
  return false;
}

//HTTP请求需要认证处理
void WebPage::handleAuthenticationRequired(const QUrl &requestUrl,QAuthenticator *auth)
{
    QWidget *mainWindow=view()->window();
    QDialog dialog(mainWindow);
    dialog.setModal(true);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    Ui::PasswordDialog passwordDialog;
    passwordDialog.setupUi(&dialog);

    passwordDialog.m_iconLabel->setText(QString());
    QIcon icon(mainWindow->style()->standardIcon(QStyle::SP_MessageBoxQuestion,0,mainWindow));
    passwordDialog.m_iconLabel->setPixmap(icon.pixmap(32,32));

    QString introMessage(tr("Enter username and password for \"%1\" at %2").arg (auth->realm().arg(requestUrl.toString().toHtmlEscaped())));
    passwordDialog.m_iconLabel->setText(introMessage);
    passwordDialog.m_iconLabel->setWordWrap(true);

    if(dialog.exec() == QDialog::Accepted)
    {
        auth->setUser(passwordDialog.m_userNameLineEdit->text());
        auth->setPassword(passwordDialog.m_passwordLineEdit->text());
    }
    else{
        //若用户选择取消对话，将验证器设置为空
        *auth = QAuthenticator();
    }


}
//HTTP请求失败处理
void WebPage::handleProxyAuthenticationRequired(const QUrl &requestUrl,QAuthenticator *auth,const QString &proxyHost)
{
    QWidget *mainWindow=view()->window();
    QDialog dialog(mainWindow);
    dialog.setModal(true);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    Ui::PasswordDialog passwordDialog;
    passwordDialog.setupUi(&dialog);

    passwordDialog.m_iconLabel->setText(QString());
    QIcon icon(mainWindow->style()->standardIcon(QStyle::SP_MessageBoxQuestion,0,mainWindow));
    passwordDialog.m_iconLabel->setPixmap(icon.pixmap(32,32));

    QString introMessage(tr("Connect to proxy \"%1\" using:"));
    introMessage = introMessage.arg(proxyHost.toHtmlEscaped());
    passwordDialog.m_infoLabel->setText(introMessage);
    passwordDialog.m_infoLabel->setWordWrap(true);

    if(dialog.exec() == QDialog::Accepted)
    {
        auth->setUser(passwordDialog.m_userNameLineEdit->text());
        auth->setPassword(passwordDialog.m_passwordLineEdit->text());
    }
    else
    {
        //如用户选择取消对话，将验证器设置为空
        *auth = QAuthenticator();
    }
}
