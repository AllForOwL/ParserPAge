#include <QFile>
#include <QtNetwork/QNetworkReply>
#include <QTextStream>
#include <QUrl>
#include <QThread>
#include <downloadtextpage.h>

#include "parserurl.h"
#include "ui_parserurl.h"

const QString CNT_BEGIN_LINK    = "http://";
const QString CNT_END_LINK      = "\"";

ParserUrl::ParserUrl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParserUrl)
{
    ui->setupUi(this);

    m_queueLinkPage.push_back("http://vasinnet.blogspot.com/2012/08/recommended-way-to-using-qthread.html");
    m_queueLinkPage.push_back("http://vasinnet.blogspot.com/2012/08/recommended-way-to-using-qthread.html");
    m_queueLinkPage.push_back("http://vasinnet.blogspot.com/2012/08/recommended-way-to-using-qthread.html");
    m_queueLinkPage.push_back("http://vasinnet.blogspot.com/2012/08/recommended-way-to-using-qthread.html");
    m_queueLinkPage.push_back("http://vasinnet.blogspot.com/2012/08/recommended-way-to-using-qthread.html");
}

ParserUrl::~ParserUrl()
{
    delete ui;
}

void ParserUrl::GetContentPage(QString i_contentPage)
{
    m_queueContentPage.push_back(i_contentPage);
}

void ParserUrl::AddThread(QString i_link)
{
    QThread* _pThread = new QThread();
    DownloadTextPage* _downloadPage = new DownloadTextPage(i_link);
    _downloadPage->moveToThread(_pThread);

    // Соединяем сигнал started потока, со слотом process "рабочего" класса, т.е. начинается выполнение нужной работы.
    connect(_pThread, SIGNAL(started()), _downloadPage, SLOT(StartDownload()));
    // По завершению выходим из потока, и удаляем рабочий класс
    connect(_downloadPage, SIGNAL(finishedDownload(QString)), this, SLOT(GetContentPage(QString)));
    // По завершению выходим из потока, и удаляем рабочий класс
    connect(_downloadPage, SIGNAL(finishedDownload(QString)), _pThread, SLOT(quit()));
    connect(_downloadPage, SIGNAL(finishedDownload(QString)), _downloadPage, SLOT(deleteLater()));
    // Удаляем поток, после выполнения операции
    connect(_pThread, SIGNAL(finished()), _pThread, SLOT(deleteLater()));

    _pThread->start();
}

void ParserUrl::slotStartParsing()
{
    for (int i = 0; i < 5; i++)
    {
        AddThread(m_queueLinkPage.front());
        m_queueLinkPage.pop_front();
    }
}

void ParserUrl::replyFinished(QNetworkReply* pReply)
{
   QByteArray   _byteTextPage = pReply->readAll();
   m_queueContentPage.push_back(_byteTextPage);
}

void ParserUrl::SearchLinkOnPage()
{
    while (!m_queueContentPage.empty())
    {
          QString _contentPage = m_queueContentPage.front();
          m_queueContentPage.pop_front();

          int _indexBeginLink   = 0;
          int _indexEndLink     = 0;
          while (true)
          {
            _indexBeginLink = _contentPage.indexOf(CNT_BEGIN_LINK, _indexEndLink);

            if (_indexBeginLink != -1)
            {
                _indexEndLink = _contentPage.indexOf(CNT_END_LINK, _indexBeginLink);

                QString _link = _contentPage.mid(_indexBeginLink, _indexEndLink - _indexBeginLink);
                m_queueLinkPage.push_back(_link);
            }
            else
            {
                break;
            }
          }
     }
}

void ParserUrl::on_pushButton_clicked()
{
    m_queueLinkPage.push_back(ui->lineEdit->text());

    emit slotStartParsing();
}
