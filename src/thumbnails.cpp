#include "thumbnails.h"
#include "defines.h"
#include <QFileInfo>
#include <QDebug>
#include <QtConcurrent>
#include <QImage>
#include <QMessageAuthenticationCode>
#include <EMimIcon>
#include <QPainter>
#include <QImageReader>
#include <QCryptographicHash>

namespace {
    QMutex _mutex;

    QString md5EncodedName(const QString& file) {
        if (file.isEmpty())
            return QString();

        Q_ASSERT(file.startsWith("file:/"));
        const QByteArray hash = QCryptographicHash::hash(file.toUtf8(), QCryptographicHash::Md5);
        const QString s {hash.toHex()};
        return s;
    }

    QString thumbnailFilePath(const QString& basePath, const QString& file, Thumbnails::Size size) {
        QString tnFilePath {basePath};
        if( !tnFilePath.endsWith('/')) tnFilePath.append("/");

        if (size == Thumbnails::Size::Normal)
            tnFilePath.append("normal/");
        else if( size == Thumbnails::Size::Large)
            tnFilePath.append("large/");
        else if( size == Thumbnails::Size::XLarge)
            tnFilePath.append("x-large/");
        else if( size == Thumbnails::Size::XXLarge)
            tnFilePath.append("xx-large/");
        else if( size == Thumbnails::Size::Fail)
            tnFilePath.append("fail/");

        tnFilePath.append(md5EncodedName(file));
        tnFilePath.append(".png");

        return tnFilePath;
    }

    QIcon readFromCache(const QFileInfo& file, Thumbnails::CacheType cache, Thumbnails::Size size, bool dehydrated) {

        QString cacheBasePath {Edir::personalThumbnailsCacheDir()};
        QString fileName = QStringLiteral("file://")+file.absoluteFilePath();

        if (cache == Thumbnails::CacheType::Shared) {
            cacheBasePath = file.absolutePath();
            if( !cacheBasePath.endsWith('/')) cacheBasePath.append("/");
            cacheBasePath.append(QStringLiteral(".sh_thumbnails/"));
            fileName = QStringLiteral("file:/") + file.fileName(); // the shared cache only uses the filename
        }
        const QString cacheFile = thumbnailFilePath(cacheBasePath, fileName, size);

        QString iconFile;
        if (QFile::exists(cacheFile)){
            iconFile = cacheFile;

            // for real files check if the Modtime is fine.
            if(!dehydrated) {
                QImage reader(cacheFile);
                QStringList keys = reader.textKeys();
                const QString fModified = reader.text(THUMB_LAST_MODIFIED);

                if( fModified.isEmpty() ||
                        fModified != QString::number(file.lastModified().toSecsSinceEpoch())) {
                    QFile::remove(iconFile);
                    iconFile.clear(); // Do not try to load further down.
                }
            }
        } else {
            // there is no thumbnail file in personal cache. Check if the file is small
            // enough to read it directly.
            QImageReader reader(file.absoluteFilePath());
            if(reader.canRead()){
                if(qMax(reader.size().width(),reader.size().height())<=128){
                    // hasImage=   image.load((file));
                    iconFile = file.absoluteFilePath();
                }
            }
        }
        // if the fileIcon is set, load the image in there and be done.
        QIcon icon;
        if( !iconFile.isEmpty() ) {
            icon.addFile(iconFile, QSize(128,128)); // FIXME handle size properly.
        }
        return icon;
    }
}

//***********************  Thumbnails ***********************
Thumbnails *Thumbnails::_instance = nullptr;

Thumbnails::Thumbnails(QObject *parent) : QObject(parent)
  , mCacheType{CacheType::Personal}
{
    canReadPdf=EMimIcon::findProgram("gs");
   // canReadVideo=EMimIcon::findProgram("ffmpeg");

    mThread=new Thread;
    // connect(mThread,SIGNAL(canceled(QString)),this,SLOT(cancel(QString)));
    connect(mThread,SIGNAL(finished())         ,this,SLOT(startNewThread()));
    connect(mThread,SIGNAL(terminated(QString)),this,SIGNAL(updateThumbnail(QString)));
    connect(mThread,SIGNAL(excluded(QString))  ,this,SLOT(setLisExclude(QString)));

}

Thumbnails::~Thumbnails()
{
    delete mThread;
}


Thumbnails *Thumbnails::instance()
{
    if (!_instance) {
        _instance = new Thumbnails;
    }
    return _instance;
}

//_____________________________________________________________
void Thumbnails::directoryChanged(const QString &path)
{
    // qDebug()<<__FILE__<<__FUNCTION__<<mCurentPath<<path;
    if(mCurentPath!=path){

        mCurentPath=path;

        while (myMap.count()>10) {
            QString   filename = myMap.firstKey();
            myMap.remove(filename);
        }

    }

}

//_____________________________________________________________
void Thumbnails::addFileName(const QFileInfo &info)
{

   if(mListExclude.contains(info.filePath())) { return; }
   QMimeDatabase db;
   QMimeType mime = db.mimeTypeForFile(info);
   const QString mimeType = mime.name();

   if(mime.inherits("application/pdf") && !canReadPdf ) { return; }

   if(mimeType.startsWith("video/") && !canReadVideo  ) { return; }

   qDebug()<<__FILE__<<__FUNCTION__<<info.fileName()<< mimeType;

    while (myMap.count()>50) {
        QString   filename = myMap.firstKey();
        myMap.remove(filename);
    }

    myMap[info.filePath()] = mimeType;
    //myMap.insert(myMap.constBegin() ,info.filePath(),type);
    if(!mThread->isRunning())
    { startRender(); }

}

//_____________________________________________________________
void Thumbnails::startNewThread()
{
    myMap.remove(mThread->curentPath());
    // qDebug()<<__FILE__<<"finiched>>>>>>>>>>"<<mThread->curentPath();
    startRender();
}

//_____________________________________________________________
QIcon Thumbnails::getThumbnail( const QFileInfo& fi, Size size, bool dehydrated)
{
    // Read from personal cache first, if that does not return anything,
    // check the shared one.
    QIcon icon = readFromCache(fi, CacheType::Personal, size, dehydrated);

    if (icon.isNull()) {
        icon = readFromCache(fi, CacheType::Shared, size, dehydrated);
        if (!icon.isNull())
            qDebug() << "Read thumbnail from shared thumbnail cache for" << fi.filePath();
    } else {
        qDebug() << "Read thumbnail from personal thumbnail cache for" << fi.filePath();
    }

    if (icon.isNull() && !dehydrated) {
        // add the file to start the render thread
        qDebug() << "Could not read file thumbnail from cache, creating" << fi.filePath();
        addFileName(fi);
    }

    return icon;
}

//_____________________________________________________________
void Thumbnails::startRender()
{
    if(myMap.count()<1) {return;}
    if(mThread->isRunning()){return;}
    // isRunning=true;
    QString   filename = myMap.firstKey();
    QString   type = myMap.first();
    QFileInfo info(filename);

    mThread->setFile(info, type, mCacheType);
    mThread->start();
}

void Thumbnails::createInSharedCache(bool shared)
{
    if (shared)
        mCacheType = Thumbnails::CacheType::Shared;
    else
        mCacheType = Thumbnails::CacheType::Personal; // the default
}

//***********************  THREAD ******************************

void Thread::run()
{
    // Get the input data out of the critical variables
    _mutex.lock();
    QString md5FilePath = QStringLiteral("file://")+mInfo.filePath();
    const QString lastMod = QString::number(mInfo.lastModified().toSecsSinceEpoch());
    quint64 fileSize = mInfo.size();
    Thumbnails::CacheType cacheType = mWriteToCacheType;
    _mutex.unlock();

    QString cacheBasePath {Edir::personalThumbnailsCacheDir()};
    if (cacheType == Thumbnails::CacheType::Shared) {
        cacheBasePath = mInfo.absolutePath();
        if( !cacheBasePath.endsWith('/')) cacheBasePath.append("/");
        cacheBasePath.append(QStringLiteral(".sh_thumbnails/"));
        md5FilePath = QStringLiteral("file:/") + mInfo.fileName(); // the shared cache only uses the filename
    }

    const QString fileThumbnail = thumbnailFilePath(cacheBasePath, md5FilePath, Thumbnails::Size::Normal);
    qDebug() << "Got thumbFile " << fileThumbnail << "from" << md5FilePath << "in" << cacheBasePath;
    // create an empty image to fill in the thumbnail. It is passed to the
    // creator functions by reference, so they fill it.
    QImage image;

    if(mType.startsWith(D_IMAGE_TYPE))  {
        createImageThumbnail(mInfo.filePath(), image);
    } else if(mType.endsWith(D_PDF_TYPE))  {
        createPdfThumbnailGS(mInfo.filePath(), image);
    } else if(mType.startsWith(D_VIDEO_TYPE)) {
        createVideoThumbnail(mInfo.filePath(), image);
    }

    // if there is a resulting image, it is saved with some metadata.
    if (!image.isNull()) {
        image.setText(THUMB_LAST_MODIFIED, lastMod);
        image.setText(THUMB_URI, md5FilePath);
        // image.setText(THUMB_MIMETYPE, ) FIXME
        image.setText(THUMB_SIZE, QString::number(fileSize));

        if(image.save(fileThumbnail)) {
            qDebug()<<__FILE__<<__FUNCTION__<<"image saved"<< md5FilePath;
        }
    }
    emit terminated(mInfo.filePath());
}

void Thread::setFile(const QFileInfo &info,const QString &mimeType, Thumbnails::CacheType cacheType)
{
    QMutexLocker locker(&_mutex);
    mInfo = info;
    mType = mimeType;
    mWriteToCacheType = cacheType;
}

//***************************************************************
//*                  Generate Images thumbnails                 *
//***************************************************************
void Thread::createImageThumbnail(const QString& filePath, QImage &image)
{
    QImageReader reader(filePath);
    if(!reader.canRead()){
        return;
    }

    if(qMax(reader.size().width(),reader.size().height())<=128) {
        return;
    }

    if( image.load(filePath)) {
        // FIXME: Consider size parameter here
        image= image.scaled(QSize(128,128), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    //---------------------end

}

//***************************************************************
//*                  Generate PDF thumbnails                    *
//***************************************************************

void Thread::createPdfThumbnailGS(const QString& filePath, QImage &image)
{
    // gs -dNOPAUSE -dBATCH -sDEVICE=png16m -r96 -dLastPage=1 -sOutputFile=- -sstdout=%stderr -g120x120 -dPDFFitPage=true
    QProcess p;
    QStringList args;

    args << "-dNOPAUSE";
    args << "-DBATCH";
    args << "-sDEVICE=png16m";
    args << "-r96";
    args << "-dLastPage=1";
    args << "-sOutputFile=-";
    args << "-sstdout=%stderr";
    args << "-g120x120";
    args << "-dPDFFitPage=true";
    args << filePath;

    p.start("/usr/bin/gs", args);
    QByteArray ba;
    if (p.waitForStarted(-1)) {
        while(p.waitForReadyRead(-1)) {
            ba += p.readAllStandardOutput();
        }
    }
    if (!ba.isEmpty())
        image.loadFromData(ba);
}

//***************************************************************
//*                  Generate VIDEO thumbnails                 *
//***************************************************************
void Thread::createVideoThumbnail(const QString& filePath, QImage &image)
{
    QTemporaryFile fi;
    QString fileName;
    if (fi.open()) {
        fileName = fi.fileName();
        fi.close();
    }
    QMap<QString, QString> map=  videoInfo();
    QString pos=map.value("Pos");
    QString vtime=map.value("Time");

    qDebug()<<"thumb"<<pos<<vtime;

    QStringList list;
    //ffmpeg -i ./kofar-bi-amirica.mp4 -y -ss 10.0 -vframes 1 -vf  scale="'if(gt(a,1/1),128,-1)':'if(gt(a,1/1),-1,128)'"   out.png
    //        list<<"-i"<<mInfo.filePath()<<"-y"<<"-t"<<"1"<<"-r"<<"1"
    //           <<"-ss"<<pos<<"-s"<<"128x128"<<"-f"<<"image2"<<fileThumbnail+".video";
    list<<"-i"<<filePath         /*Input File Name*/
       <<"-y"                    /*Overwrite*/
      <<"-ss"<<pos              /* seeks in this position*/
     <<"-vframes"<<"1"         /* Num Frames */
    <<"-f"<<"image2"          /* file format.  */
    <<"-s"<<"128x128"         /*<<"-vf"<<scal*/
    << fileName; /*output file Name */

    QProcess p;
    //-------------------------------------------

    //-------------------------------------------
    p.start("ffmpeg",list);

    if (!p.waitForStarted()) {   return ;  }

    if (!p.waitForFinished()){   return ;  }

    QString err=p.readAllStandardError();
    QString read=p.readAll();
    if(err.contains("not contain any stream"))
        emit excluded(filePath);

    if( image.load(fileName)) {

      //  imagevideo= imagevideo.scaled(QSize(128,128),Qt::KeepAspectRatio,Qt::SmoothTransformation);

        QImage imageIcon;
        imageIcon.load(":/icons/video.svg");
        QPainter p(&image);
        int imX=(image.width()-imageIcon.width())/2;
        int imY=(image.height()-imageIcon.height())/2;
        p.drawImage(imX,imY,imageIcon);
        QRect rect(0,imY+imageIcon.height(),image.width(),image.height()-(imY+imageIcon.height()));
        p.setPen(QColor(Qt::black));
        p.drawText(rect,Qt::AlignHCenter|Qt::AlignVCenter,vtime);
        rect.adjust(1,1,1,1);
        p.setPen(QColor(Qt::white));
        p.drawText(rect,Qt::AlignHCenter|Qt::AlignVCenter,vtime);

        // imagevideo.setText(D_KEY_DATETIME,mInfo.lastModified().toString("dd MM yyyy hh:mm:ss"));
        QByteArray text=mInfo.filePath().toUtf8();
        // imagevideo.setText(D_KEY_FILEPATH,text.toHex());

        QFile::remove(fileName);
        emit terminated(mInfo.filePath());
    }

    //---------------------end

}

QMap<QString, QString> Thread::videoInfo()
{

    QMap<QString, QString> map;
    QStringList list2;
    list2<<"-i"<<mInfo.filePath();
    QString ret="5.0";
    QString time=QString();
    map["Pos"]=ret;
    map["Time"]=time;
    QProcess p;
    //-------------------------------------------
    p.start("ffmpeg",list2);
    if (!p.waitForStarted()) {   return map ;  }

    if (!p.waitForFinished()){   return map;   }

    QString error=p.readAllStandardError();

    if(error.isEmpty())return map;

    QStringList list=error.split("\n");

    foreach (QString s, list) {
        if(s.trimmed().startsWith("Duration")){
            s=s.remove("Duration:");
            QString name=s.section(",",0,0);

            QStringList listtime=name.trimmed().split(":");
            if(listtime.count()>=3){
                QString h=listtime.at(0);
                QString m=listtime.at(1);
                QString s=listtime.at(2);

                if     (h.toFloat()>0  )    {ret= "15.0"; time+=h+":"; }
                else if(m.toFloat()>0  )    {ret=  "7.0"; }
                else if(s.toFloat()<=1 )    ret=  "0.1";
                else if(s.toFloat()<=5 )    ret=  "1.0";
                else if(s.toFloat()<=10)    ret=  "3.0";
                else if(s.toFloat()>10 )    ret=    ret;


               time+=m+":";
               time+=s.leftRef(2);

                qDebug()<<"info"<<ret+"|"+time;;

map["Pos"]=ret;
map["Time"]=time;
                return map;
            }

        }

    }
    return map;

}
