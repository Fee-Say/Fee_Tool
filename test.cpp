#include <QCoreApplication>
#include <QFile>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QUrl>

class Downloader : public QObject {
    Q_OBJECT

public:
    Downloader(const QUrl& url, const QString& localFilePath, QObject* parent = nullptr)
        : QObject(parent), url(url), localFilePath(localFilePath) {
        manager = new QNetworkAccessManager(this);
        connect(manager, &QNetworkAccessManager::finished, this, &Downloader::onDownloadFinished);
    }

    void startDownload() {
        QNetworkRequest request(url);
        manager->get(request);
    }

private slots:
    void onDownloadFinished(QNetworkReply* reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QFile localFile(localFilePath);
            if (localFile.open(QIODevice::WriteOnly)) {
                localFile.write(reply->readAll());
                localFile.close();
                qDebug() << "Datei erfolgreich heruntergeladen: " << localFilePath;
            }
            else {
                qWarning() << "Fehler beim Öffnen der lokalen Datei zum Schreiben.";
            }
        }
        else {
            qWarning() << "Fehler beim Herunterladen: " << reply->errorString();
        }

        // Aufräumen
        reply->deleteLater();
        QCoreApplication::quit();
    }

private:
    QUrl url;
    QString localFilePath;
    QNetworkAccessManager* manager;
};

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);

    // GitHub-URL zur Datei, die heruntergeladen werden soll
    QUrl githubUrl("https://github.com/KawaiiCat2/test_repo/releases");

    // Zielort für die heruntergeladene Datei
    QString localFilePath = "logos.rar";

    // Erstelle den Downloader und starte den Download
    Downloader downloader(githubUrl, localFilePath);
    downloader.startDownload();

    return a.exec();
}