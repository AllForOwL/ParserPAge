#include "downloadtextpage.h"
#include <QNetworkReply>
#include <QNetworkRequest>

DownloadTextPage::DownloadTextPage(const QString i_strLink, QObject *parent) : QObject(parent)
{
    m_link = i_strLink;
    m_downloadManager = new QNetworkAccessManager();

    connect(m_downloadManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

void DownloadTextPage::replyFinished(QNetworkReply* i_pReply)
{
   QByteArray _byteTextPage = pReply->readAll();
   m_contentPage = _byteTextPage;

   emit finished(m_contentPage);
}

QString DownloadTextPage::GetContentPage() const
{
    return m_contentPage;
}

void DownloadTextPage::StartDownload()
{
    m_downloadManager->get(QNetworkRequest(QUrl(m_link)));          // need create new thread
}
