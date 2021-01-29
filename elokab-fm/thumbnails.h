#ifndef THUMBNAILS_H
#define THUMBNAILS_H

#include <QObject>
#include <QMap>
#include <QFileInfo>
#include <QThread>

//*********************THUMBNAILS**************************
class Thread;

class Thumbnails : public QObject
{
    Q_OBJECT
public:
    explicit Thumbnails(QObject *parent = nullptr);
    //!
   ~Thumbnails();

    enum class Size {
        Normal,
        Large,
        XLarge,
        XXLarge,
        Fail
    };

    enum class CacheType {
        Personal,
        Shared
    };

    QIcon getThumbnail(const QFileInfo& fi, Size size = Size::Normal);

signals:
    void updateThumbnail(const QString &path);

public slots:
    void addFileName(const QFileInfo &info);
    void directoryChanged(const QString &path);
    void createInSharedCache(bool shared);

private slots:
    void startRender();
    void startNewThread();
    void setLisExclude(const QString &path){mListExclude.append(path);}

private:
    Thread   *mThread;

     QStringList mListExclude;

    QString   mCurentPath;
    QString   mCurType;

    bool      canReadPdf;
    bool      canReadVideo;

    CacheType mCacheType;
    QMap<QString,QString>myMap;
};

//*********************THREAD**************************

class Thread : public QThread
{
    Q_OBJECT

public:
    Thread(){}

    void setFile(const QFileInfo &info,const QString &type, Thumbnails::CacheType cacheType);
    QString curentPath(){return mInfo.filePath();}

signals:
    void terminated(const QString &path);
    void excluded(const QString &path);


protected:
    void run();

private:
    QFileInfo   mInfo;
    QString     mType;
    Thumbnails::CacheType mWriteToCacheType;

    void createImageThumbnail(const QString &filePath, QImage &image);
    void createPdfThumbnailGS(const QString& filePath, QImage &image);
    void createVideoThumbnail(const QString& filePath, QImage &image);
    QMap<QString,QString> videoInfo();
};


#endif // THUMBNAILS_H
