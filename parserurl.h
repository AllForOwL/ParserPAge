#ifndef PARSERURL_H
#define PARSERURL_H

#include <QWidget>
#include <QtNetwork/QNetworkAccessManager>
#include <QQueue>
#include <QStandardItemModel>

namespace Ui {
class ParserUrl;
}

class ParserUrl : public QWidget
{
    Q_OBJECT

public:
    explicit ParserUrl(QWidget *parent = 0);
    ~ParserUrl();

    void AddThread(QString i_link);
    void SearchLinkOnPage();

private slots:
    void AddNewThreadAfterDestroyPrevious();
    void GetContentPage(QString i_contentPage);
    void replyFinished(QNetworkReply*);
    void on_pushButton_clicked();
    void slotStartParsing();
    void StopScanningComplete();
    void StopScanningUser();

    void on_pushButton_2_clicked();

private:
    Ui::ParserUrl *ui;

    QStandardItemModel*     m_modelTable;

    bool                    m_blockSignal;
    int                     m_quentityCompleteScanning;
    int                     m_quentityMaxScanningLink;
    QString                 m_findText;
    int                     m_quentityThread;
    int                     m_quentityLink;
    int                     m_quentityLinkBegin;
    int                     m_quentityContentLink;
    int                     m_quentityContentLinkBegin;
    QQueue<QString>         m_queuLinkForChangeStatus;
    QQueue<QString>         m_queueContentPage;
    QQueue<QString>         m_queueLinkPage;
    QString                 m_strStartUrl;
    QNetworkAccessManager*  m_manager;
};

#endif // PARSERURL_H

/* Step for complete task
 * 1)Download all page;
 * 2)Search all links and add their to vector;
 * 3)Go to one step;
*/

/* Create thread and add their working
 *
*/
