#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QtCore>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QDebug>
#include <QFile>
#include <QBuffer>
#include <QVector>
#include <QTimer>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWaitCondition>
#include <QMutex>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <stdlib.h>

#include "videothread.h"
#include "recordplayback.h"

#include "cvmatviewer.h"
#include "playbackthread.h"

#include "logodetectionthread.h"
#include "cutratedetectionthread.h"
#include "bsdetectionthread.h"

#include "mainmenu.h"
#include "sidemenu.h"
#include "slidervisual.h"
#include "notificationwidget.h"
#include "recordingoptions.h"
#include "recordings.h"
#include "channelselectionview.h"
#include "frameglwidget.h"





using namespace cv;
using namespace std;

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

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QTimer *timer, *commercialTimer1, *commercialTimer2, *notesTimer;

    FILE *f;


    QString recordFolder;

    AVFormatContext *formCtx1, *formCtx2;
    int i, streamIndex, svIndex, svIndex2;
    AVStream *audioStream, *videoStream, *videoStream2;
    AVCodecContext  *pCodecCtx, *eCodecCtx, *vCodecCtx, *vCodecCtx2;
    AVCodec *pCodec, *eCodec, *vCodec, *vCodec2;
    AVFrame *frame1, *vFrame;
    AVPacket packet;
    int frame1Finished;
    int numBytes;
    int buffersize1;
    uint *samples1;


    QBuffer buf;
    QBuffer vBuf;


    QByteArray data;
    QByteArray vData;

    VideoThread *vThread;


    QGraphicsScene *myScene;

    cvMatViewer *pipViewer;
    cvMatViewer *matViewer;

    playBackThread *pbThread;

    VideoCapture cap;


    VideoWriter writer1;
    VideoWriter writer2;

    QTimer *capTimer;
    QTimer *playBackTimer;

    QVector<Mat> bsBuffer;
    QVector<Mat> ldBuffer;
    QVector<Mat> crBuffer;
    QVector<Mat> frameStore;

    QList<Mat> mainBuffer;
    QList<Mat> secondaryBuffer;

    QVector<Mat> bsBuffer2;
    QVector<Mat> ldBuffer2;
    QVector<Mat> crBuffer2;


    bool primary;

    bool ldPrimary;
    bool bsPrimary;
    bool crPrimary;

    bool ldSecondary;
    bool bsSecondary;
    bool crSecondary;

    bool primaryOnComm;
    bool secondaryOnComm;

    Size sz;

    int frameCounter;
    int frameCounter2;

    bool toggle;
    bool toggle2;
    bool toggle3;
    bool toggle4;

    int playBackIndex;


    CutRateDetectionThread *HCRateThread;
    logoDetectionThread *LDThread;
    BSDetectionThread *bsDetectThread;

    CutRateDetectionThread *HCRateThread2;
    logoDetectionThread *LDThread2;
    BSDetectionThread *bsDetectThread2;

    bool mtoggle;

    SideMenu *sMenu;
    bool stoggle;

    SliderVisual *sVis;
    bool sliderToggle;

    NotificationWidget *notes;
    bool notifs;


    RecordingOptions *recOps;
    bool recOptions;

    Recordings *recsWin;
    bool recsWindow;

    ChannelSelectionView *chanSel;
    bool chanSelWin;

    bool isPaused;

    QRect rect;
    QRect rect2;

    bool flashBackOn;

    bool isPIP;

    bool primeRec, secRec;

    int vid1FPS, vid2FPS;


public slots:



    void setUpGView();
    void drawMat(byte* data);
    void drawMat2(byte *data);
    void drawMatCV();
    void fillBuffers(Mat mat, bool primaryCh);


    void recHighCuts(bool rec);
    void recBS(bool rec);
    void recNoLogo(bool rec);

    void recHighCuts2(bool rec);
    void recBS2(bool rec);
    void recNoLogo2(bool rec);


    void addFramesToThreads(bool primaryCh);
    void addFramesToBSThread(bool primaryCh);

    void playBackFromBuf();
    void chkPrimaryForCB();
    void chkSecondaryForCB();

    void chkPrimaryCutRate();
    void chkSecondaryCutRate();

    void toggleFlashBack(bool on);

    void changeToPIP(bool inPIP);

    void recordPrimaryStream();
    void recordSecondaryStream();
    void cancelAllRecording();
    void setUpPB(const char* playBack);

    void closeNote();


protected:
    void keyPressEvent(QKeyEvent *a);

signals:
   void ldcrbufferFull(bool);
   void bsbufferFull(bool);
   void breakVthread(bool);
   void bufferReady();

    
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
