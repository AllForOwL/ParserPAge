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

    m_quentityLink      = 0;
    m_quentityThread    = 0;
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
    connect(_pThread, SIGNAL(finished()), this, SLOT(AddNewThreadAfterDestroyPrevious()));
    connect(_pThread, SIGNAL(finished()), _pThread, SLOT(deleteLater()));

    _pThread->start();
}

void ParserUrl::AddNewThreadAfterDestroyPrevious()
{
    if (m_queueLinkPage.size())
    {
        AddThread(m_queueLinkPage.front());
        m_queueLinkPage.pop_front();
    }

    if (!--m_quentityLink)
    {
        SearchLinkOnPage();
    }
}

void ParserUrl::slotStartParsing()
{
    m_quentityLink = m_queueLinkPage.size();

    if (m_quentityLink >= m_quentityThread)
    {
        for (int i = 0; i < m_quentityThread; i++)
        {
            AddThread(m_queueLinkPage.front());
            m_queueLinkPage.pop_front();
        }
    }
    else
    {
        for (int i = 0; i < m_quentityLink; i++)
        {
            AddThread(m_queueLinkPage.front());
            m_queueLinkPage.pop_front();
        }
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

    emit slotStartParsing();
}

void ParserUrl::on_pushButton_clicked()
{
    m_queueLinkPage.push_back(ui->lineEdit->text());
    m_quentityThread = ui->lineEdit_2->text().toInt();

    emit slotStartParsing();
}
