#include <QFile>
#include <QtNetwork/QNetworkReply>
#include <QTextStream>
#include <QUrl>

#include "parserurl.h"
#include "ui_parserurl.h"

const QString CNT_BEGIN_LINK    = "http://";
const QString CNT_END_LINK      = "\"";

ParserUrl::ParserUrl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParserUrl)
{
    ui->setupUi(this);

    m_manager = new QNetworkAccessManager();

    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

ParserUrl::~ParserUrl()
{
    delete ui;
}

void ParserUrl::slotStartParsing()
{
    Fetch();
}

void ParserUrl::replyFinished(QNetworkReply* pReply)
{
   QByteArray   _byteTextPage = pReply->readAll();
   m_queueContentPage.push_back(_byteTextPage);
}

void ParserUrl::Fetch()
{
    for (int i = 0; i < m_queueLinkPage.size(); i++)
    {
        m_manager->get(QNetworkRequest(QUrl(m_queueLinkPage[i])));          // need create new thread
    }
    m_queueLinkPage.clear();

    SearchLinkOnPage();

    emit slotStartParsing();
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
