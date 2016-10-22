#ifndef DOWNLOADTEXTPAGE_H
#define DOWNLOADTEXTPAGE_H

#include <QObject>
#include <QNetworkAccessManager>

class DownloadTextPage : public QObject
{
    Q_OBJECT
public:
    explicit DownloadTextPage(const QString i_strLink, QObject *parent = 0);

    QString GetContentPage() const;

public slots:
    void replyFinished(QNetworkReply*);
    void StartDownload();

signals:
    void finishedDownload(QString i_contentPage);

private:
    QString m_link;
    QString m_contentPage;
    QNetworkAccessManager* m_downloadManager;

public slots:
};

#endif // DOWNLOADTEXTPAGE_H
