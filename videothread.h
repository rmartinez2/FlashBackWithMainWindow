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
    AVFrame *vFrame, *vFrameRGB, *bgrFrame, *bgrFrame2, *vFrame2;
    AVFormatContext *formCtx,*formatCtx2;
    AVStream *videostream1,*videostream2;
    AVCodecContext *vCodecCtx1, *vCodecCtx2;
    AVCodec *vCodec, *vCodec2;
    AVDictionary *cDict;
    SwsContext *imgConvertCtx, *imgConvertCtx2;
    int vidStream,vidStream2;
    int destWidth, destHeight, destWidth2, destHeight2;


    bool breaker;



    byte* sampleSize;
    int bufferSize;

    byte* video_dst_data[4] = {NULL};
    int video_dst_linesize[4];




    
signals:

    void sendData1(byte* data);
    void sendData2(byte* data);


    
public slots:
    void initVideoFrame();
    void findVideoStream();
    bool checkCodec();
    void initConverter();
    void setDstInts();
    void allocRGBPic();
    void vThreadBreak(bool breaker);





    
};

#endif // VIDEOTHREAD_H
