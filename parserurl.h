#ifndef PARSERURL_H
#define PARSERURL_H

#include <QWidget>
#include <QtNetwork/QNetworkAccessManager>
#include <QQueue>

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
    void GetContentPage(QString i_contentPage);
    void replyFinished(QNetworkReply*);
    void on_pushButton_clicked();
    void slotStartParsing();

private:
    Ui::ParserUrl *ui;

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
