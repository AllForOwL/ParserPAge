#include <QFile>
#include <QtNetwork/QNetworkReply>
#include <QTextStream>
#include <QUrl>
#include <QThread>
#include <QMessageBox>
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

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    m_modelTable = new QStandardItemModel(0, 2);
    m_modelTable->setHorizontalHeaderItem(0, new QStandardItem("URL"));
    m_modelTable->setHorizontalHeaderItem(1, new QStandardItem("Статус"));

    ui->tableView->setModel(m_modelTable);

    m_blockSignal       = false;
    m_findText          = "";
    m_quentityLink      = 0;
    m_quentityThread    = 0;   
    m_quentityCompleteScanning = 0;

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

    connect(_pThread,       SIGNAL(started()),                  _downloadPage, SLOT(StartDownload()));
    connect(_downloadPage,  SIGNAL(finishedDownload(QString)),  this,          SLOT(GetContentPage(QString)));
    connect(_downloadPage,  SIGNAL(finishedDownload(QString)),  _pThread,      SLOT(quit()));
    connect(_downloadPage,  SIGNAL(finishedDownload(QString)),  _downloadPage, SLOT(deleteLater()));
    connect(_pThread,       SIGNAL(finished()),                 this,          SLOT(AddNewThreadAfterDestroyPrevious()));
    connect(_pThread,       SIGNAL(finished()),                 _pThread,      SLOT(deleteLater()));

    _pThread->start();
}

void ParserUrl::AddNewThreadAfterDestroyPrevious()
{
    if (!m_blockSignal)
    {
        if (m_queueLinkPage.size())
        {
            AddThread(m_queueLinkPage.front());
            m_queuLinkForChangeStatus.push_back(m_queueLinkPage.front());
            m_queueLinkPage.pop_front();
        }

        ui->progressBar->setValue(m_quentityLinkBegin - m_quentityLink);

        if (!--m_quentityLink)
        {
            SearchLinkOnPage();
        }
    }
}

void ParserUrl::slotStartParsing()
{
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(m_queueLinkPage.size());

    m_quentityLink = m_queueLinkPage.size();
    m_quentityLinkBegin = m_quentityLink;

    if (m_quentityLink >= m_quentityThread)
    {
        for (int i = 0; i < m_quentityThread; i++)
        {
            AddThread(m_queueLinkPage.front());
            m_queuLinkForChangeStatus.push_back(m_queueLinkPage.front());
            m_queueLinkPage.pop_front();
        }
    }
    else
    {
        for (int i = 0; i < m_quentityLink; i++)
        {
            AddThread(m_queueLinkPage.front());
            m_queuLinkForChangeStatus.push_back(m_queueLinkPage.front());
            m_queueLinkPage.pop_front();
        }
    }
}

void ParserUrl::StopScanningComplete()
{
    QMessageBox::information(this, "Інформація", "Сканування завершено!!!");
}

void ParserUrl::StopScanningUser()
{
   m_blockSignal = true;

   QMessageBox::information(this, "Інформація", "Сканування припинено користувачем!!!");
}

void ParserUrl::replyFinished(QNetworkReply* pReply)
{
   QByteArray   _byteTextPage = pReply->readAll();
   m_queueContentPage.push_back(_byteTextPage);
}

void ParserUrl::SearchLinkOnPage()
{
    ui->progressBar_2->setMinimum(0);
    ui->progressBar_2->setMaximum(m_queueContentPage.size());

    m_quentityContentLink = m_queueContentPage.size();
    m_quentityContentLinkBegin = m_quentityContentLink;

    while (!m_queueContentPage.empty())
    {
        if (m_quentityCompleteScanning >= m_quentityMaxScanningLink)
        {
            emit StopScanningComplete();
            return;
        }

          ++m_quentityCompleteScanning;
          --m_quentityContentLink;
          ui->progressBar_2->setValue(m_quentityContentLinkBegin - m_quentityContentLink);

          QString _contentPage = m_queueContentPage.front();
          m_queueContentPage.pop_front();

          int _indexBeginLink   = 0;
          int _indexEndLink     = 0;
          int _indexFindText    = 0;
          bool _findText = false;
          while (_indexBeginLink != -1)
          {
            _indexBeginLink = _contentPage.indexOf(CNT_BEGIN_LINK, _indexEndLink);
            _indexFindText  = _contentPage.indexOf(m_findText, 0);

            if (_indexFindText != -1 && !_findText)
            {
                _findText = true;
                QList <QStandardItem*> _listItem = m_modelTable->findItems(m_queuLinkForChangeStatus.back());
                m_queuLinkForChangeStatus.pop_back();
                if (_listItem.size())
                {
                    for (int i = 0; i < _listItem.size(); i++)
                    {
                        m_modelTable->setItem(_listItem[i]->row(), 1, new QStandardItem("Знайдено"));
                    }
                }
           }

            if (_indexBeginLink != -1)
            {   
                _indexEndLink = _contentPage.indexOf(CNT_END_LINK, _indexBeginLink);

                QString _link = _contentPage.mid(_indexBeginLink, _indexEndLink - _indexBeginLink);
                m_queueLinkPage.push_back(_link);
                m_modelTable->appendRow(new QStandardItem(m_queueLinkPage[m_queueLinkPage.size()-1]));
                m_modelTable->setItem(m_modelTable->rowCount()-1, 1, new QStandardItem("Закачка"));
            }
            else
            {
                if (!_findText)
                {
                    QList <QStandardItem*> _listItem = m_modelTable->findItems(m_queuLinkForChangeStatus.back());
                    m_queuLinkForChangeStatus.pop_back();
                    if (_listItem.size())
                    {
                        for (int i = 0; i < _listItem.size(); i++)
                        {
                            m_modelTable->setItem(_listItem[i]->row(), 1, new QStandardItem("Не знайдено"));
                        }
                    }
                }
            }
          }
     }
    emit slotStartParsing();
}

void ParserUrl::on_pushButton_clicked()
{
    m_queueLinkPage.push_back(ui->lineEdit->text());
    m_quentityThread = ui->lineEdit_2->text().toInt();
    m_findText       = ui->lineEdit_3->text();
    m_quentityMaxScanningLink = ui->lineEdit_4->text().toInt();

    m_modelTable->appendRow(new QStandardItem(ui->lineEdit->text()));
    m_modelTable->setItem(0, 1, new QStandardItem("Закачка"));

    emit slotStartParsing();
}

void ParserUrl::on_pushButton_2_clicked()
{
    emit StopScanningUser();
}
