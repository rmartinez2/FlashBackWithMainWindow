#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include <QtCore>
#include <QDebug>
#include <QThread>
#include <QByteArray>
#include <QBuffer>
#include <QPixmap>
#include <QRgb>
#include <QVector>
#include <QMutexLocker>
#include <QImageReader>
#include <QtOpenGL/QGLWidget>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QByteArray>



extern "C"{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libavutil/imgutils.h"
    #include "libavutil/opt.h"
    #include "libswscale/swscale.h"
    #include "libavformat/avio.h"
    #include "libavutil/avstring.h"
    #include "libavutil/time.h"
    #include "libswresample/swresample.h"
}

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv/cv.h"

#include "mat2qimage.h"
#include "cutratedetectionthread.h"
#include "playbackthread.h"

#include <QWaitCondition>
#include <QMutex>


using namespace cv;

class VideoThread : public QThread
{
    Q_OBJECT

public:
    explicit VideoThread(AVFormatContext *formCtx, AVFormatContext *formCtx2 , QObject *parent = 0);

    void run();

    ~VideoThread();

    AVPacket packet;
    AVFrame *vFrame, *vFrameRGB, *bgrFrame, *bgrFrame2, *vFrame2, *rFrame, *rBGRFrame, *aFrame;
    AVFormatContext *formCtx,*formatCtx2, *recFormCtx;
    QBuffer vBuf;
    QByteArray vData;
    AVStream *videostream1,*videostream2, *recVidStream, *audioStream1;
    AVCodecContext *vCodecCtx1, *vCodecCtx2, *rCodecCtx, *aCodecCtx1;
    AVCodec *vCodec, *vCodec2, *rCodec, *aCodec;
    AVDictionary *cDict, *aDict;
    SwsContext *imgConvertCtx, *imgConvertCtx2, *rImgCovtCtx;
    int vidStream,vidStream2, rStream, aStream1;
    int destWidth, destHeight, destFmt, destWidth2, destHeight2, rDestWidth, rDestHeight;
    QVector <QImage>  qVec;
    QVector <QPixmap> pVec;

    QAudioFormat aFormat;
    QAudioOutput *output;

    QBuffer aBuf;

    QByteArray arry;

    QByteArray vArry1;
    QByteArray vArry2;

    bool audio;

    QString aFile_name1, vFile_name1;

    FILE *aFile1;
    FILE *vFile1;

    bool fOpened;


    QMutex mutex;
    QWaitCondition cond;


    bool AToggle;

    bool breaker;
    bool recPlayBack;

    int FPS;

    CutRateDetectionThread *HCRateThread;

    Mat myFrame;
    Mat myAnalyticFrame;
    QVector<Mat> myMats;
    QVector<Mat> analytics;

    playBackThread *playBack;

    cv::Size nSz;

    byte* sampleSize;
    int bufferSize;

    byte* video_dst_data[4] = {NULL};
    int video_dst_linesize[4];




    
signals:
    void sendPix(QPixmap);
    void tFinished();
    void sendQImg(QImage);
    void sendMat(Mat);
    void sendData1(byte* data);
    void sendData2(byte* data);
    void sendAO(QAudioFormat format);
    void sendBuffA(QByteArray data);
    void sendVBuff1(QByteArray,int);
    void sendVBuff2(QByteArray,int);
    
public slots:
    void initVideoFrame();
    void findVideoStream();
    bool checkCodec();
    void initConverter();
    void setDstInts();
    void allocRGBPic();
    void sendPixVec();
    void sendPixHS(QPixmap pix);
    void sendMatsPB(Mat mat);
    void vThreadBreak(bool breaker);


    bool recordContext(AVFormatContext *recFrmCtx);
    void initRecordFrame();
    void findRecordStream();
    bool checkRecCodec();
    void initRecConverter();
    void setRecDstInts();
    void allocBGRPic();

    bool findAudioStream();
    void fillAudioFrame();
    void initAudioFrame();
    void getBufferSize();
    void setAudioFormat();
    void setAudioDeviceInfo();
    void getFormatFromSample(const char** fmt, AVSampleFormat sfmt);
    void finishedPlaying(QAudio::State state);

    void receiveRequestforFrames(bool primary);


    
};

#endif // VIDEOTHREAD_H
