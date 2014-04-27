#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->window()->setWindowTitle("FlashBack");

    av_register_all();


    setUpGView();

    frameCounter = 0;
    frameCounter2 = 0;

    toggle = true;
    toggle3 = true;

    mtoggle = false;
    stoggle = false;
    sliderToggle = false;
    notifs = false;
    isPaused = false;

    primary = true;

    isPIP = false;

    ldPrimary = false;
    bsPrimary = false;
    crPrimary = false;

    ldSecondary = false;
    bsSecondary = false;
    crSecondary = false;

    primaryOnComm = false;
    secondaryOnComm = false;

    playBackIndex = 0;

    ui->graphicsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    timer = new QTimer(this);
    timer->setInterval(0.1);

    commercialTimer1 = new QTimer(this);
    commercialTimer1->setInterval(30000);

    commercialTimer2 = new QTimer(this);
    commercialTimer2->setInterval(30000);

    notesTimer = new QTimer(this);
    notesTimer->setSingleShot(true);
    notesTimer->setInterval(5000);

    formCtx1 = NULL;
    formCtx2 = NULL;

    const char* logo1 = "C:\\Users\\Rene\\Desktop\\LogoTheMiddleLg.bmp";
    const char* logo2 = "C:\\Users\\Rene\\Desktop\\LogoCW.bmp";

    recordFolder = "C:/FlashBackRecording";

    if(avformat_open_input(&formCtx1,"C:\\Users\\Rene\\Desktop\\L&O.mpg",NULL,NULL) != 0){
        qDebug() << "Did not open video";
    }


    if(avformat_find_stream_info(formCtx1,NULL) < 0){
        qDebug() << "Couldn't find stream info";
    }

    av_dump_format(formCtx1,0,"C:\\Users\\Rene\\Desktop\\L&O.mpg",0);

    if(avformat_open_input(&formCtx2,"C:\\Users\\Rene\\Desktop\\EyeForEye.mpg",NULL,NULL) != 0){
        qDebug() << "Did not open vide2";
    }


    if(avformat_find_stream_info(formCtx2,NULL) < 0){
        qDebug() << "Couldn't find stream info2";
    }

    av_dump_format(formCtx2,0,"C:\\Users\\Rene\\Desktop\\EyeForEye.mpg",0);


   streamIndex = -1;
   svIndex = -1;

   streamIndex = av_find_best_stream(formCtx1,AVMEDIA_TYPE_AUDIO,-1,-1,&pCodec,0);
   svIndex = av_find_best_stream(formCtx1,AVMEDIA_TYPE_VIDEO,-1,-1,&vCodec,0);
   svIndex2 = av_find_best_stream(formCtx2,AVMEDIA_TYPE_VIDEO,-1,-1,&vCodec2,0);



    if(streamIndex > 0){

          audioStream = formCtx1->streams[streamIndex];
          pCodecCtx = audioStream->codec;
          pCodecCtx->codec = pCodec;



    }

    if(svIndex >= 0 && svIndex2 >= 0){

        videoStream = formCtx1->streams[svIndex];
        vCodecCtx = videoStream->codec;
        vCodecCtx->codec = vCodec;

        rect.setHeight(vCodecCtx->height/2);
        rect.setWidth(vCodecCtx->width/2);

        videoStream2 = formCtx2->streams[svIndex2];
        vCodecCtx2 = videoStream2->codec;
        vCodecCtx2->codec = vCodec;

        rect2.setHeight(vCodecCtx2->height);
        rect2.setWidth(vCodecCtx2->width);

       AVRational rat1 = videoStream->r_frame_rate;
       AVRational rat2  = videoStream2->r_frame_rate;

       vid1FPS = rat1.num/rat1.den;
       vid2FPS = rat2.num/rat2.den;


        QRect rect3;
        rect3.setHeight(rect.height() + 7);
        rect3.setWidth(rect.width() + 7);
        ui->graphicsView->setGeometry(rect3);
        this->setGeometry(rect3);


    }else{
        qDebug() << "Could not find video streams" << svIndex << " " << svIndex2;
    }

    vThread = new VideoThread(formCtx1,formCtx2);


    capTimer = new QTimer(this);


    playBackTimer = new QTimer(this);
    playBackTimer->setInterval(29.97);



    HCRateThread = new CutRateDetectionThread();
    bsDetectThread = new BSDetectionThread();
    LDThread = new logoDetectionThread();



    HCRateThread2 = new CutRateDetectionThread();
    bsDetectThread2 = new BSDetectionThread();
    LDThread2 = new logoDetectionThread();

    Point p11((int)900, (int)500);
    Point p12((int)1100, (int)700);
    Point p21((int)825, (int)575);
    Point p22((int)1015, (int)650);


    LDThread->setLogoLoc(p11,p12);
    LDThread->setLogo(logo1);

    LDThread2->setLogoLoc(p21,p22);
    LDThread2->setLogo(logo2);

    bsDetectThread->readInPointVals(p11,p12);
    bsDetectThread2->readInPointVals(p21,p22);


    //Intialize Widgets for OSD
    sMenu = new SideMenu();
    sVis = new SliderVisual();
    notes = new NotificationWidget();
    recOps = new RecordingOptions();
    recsWin = new Recordings();
    chanSel = new ChannelSelectionView();

    int menuY = 0;
    int menuX = 0;

    int menuW = vCodecCtx->width/2;
    int menuH = vCodecCtx->height/2;

    recsWin->setXYWH(menuX,menuY,menuW,menuH);
    recsWin->setRecordDir(recordFolder);

    int menu2W = menuW/4;
    sMenu->setXYWH(menuX,menuY,menu2W,menuH);

    int menu3H = menuH/4;
    int menu3Y = 275;

    sVis->setXYWH(menuX,menu3Y,menuW,menu3H);

    recOps->setXYWH(menuX,menuY,menuW,menu3H);

    notes->setXYWH(400,15,200,100);

    chanSel->setXYWH(225,75,200,200);

    connect(sMenu,SIGNAL(toggleFB(bool)),this,SLOT(toggleFlashBack(bool)));

    connect(HCRateThread,SIGNAL(highCuts(bool)),this,SLOT(recHighCuts(bool)));
    connect(bsDetectThread,SIGNAL(isBlack(bool)),this,SLOT(recBS(bool)));
    connect(LDThread,SIGNAL(noLogo(bool)),this,SLOT(recNoLogo(bool)));

    connect(HCRateThread2,SIGNAL(highCuts(bool)),this,SLOT(recHighCuts2(bool)));
    connect(bsDetectThread2,SIGNAL(isBlack(bool)),this,SLOT(recBS2(bool)));
    connect(LDThread2,SIGNAL(noLogo(bool)),this,SLOT(recNoLogo2(bool)));

    connect(playBackTimer,SIGNAL(timeout()),this,SLOT(playBackFromBuf()));

    connect(this,SIGNAL(ldcrbufferFull(bool)),this,SLOT(addFramesToThreads(bool)));
    connect(this,SIGNAL(bsbufferFull(bool)),this,SLOT(addFramesToBSThread(bool)));

    connect(vThread,SIGNAL(sendData1(byte*)),this,SLOT(drawMat(byte*)));
    connect(vThread,SIGNAL(sendData2(byte*)),this,SLOT(drawMat2(byte*)));
    connect(this,SIGNAL(breakVthread(bool)),vThread,SLOT(vThreadBreak(bool)));

    connect(timer,SIGNAL(timeout()),this,SLOT(chkPrimaryForCB()));
    connect(timer,SIGNAL(timeout()),this,SLOT(chkSecondaryForCB()));

    connect(commercialTimer1,SIGNAL(timeout()),this,SLOT(chkPrimaryCutRate()));
    connect(commercialTimer2,SIGNAL(timeout()),this,SLOT(chkSecondaryCutRate()));
    connect(notesTimer,SIGNAL(timeout()),this,SLOT(closeNote()));


    connect(sMenu,SIGNAL(showRecordings(bool)),recsWin,SLOT(setVisible(bool)));
    connect(sMenu,SIGNAL(showRecordings(bool)),this,SLOT(changeToPIP(bool)));
    connect(recsWin,SIGNAL(outofPIP(bool)),this,SLOT(changeToPIP(bool)));
    connect(recsWin,SIGNAL(sendForPB(const char*)),this,SLOT(setUpPB(const char*)));

    connect(sMenu,SIGNAL(showRecordingOpts(bool)),recOps,SLOT(setVisible(bool)));
    connect(recOps,SIGNAL(primaryRecord(bool)),this,SLOT(recordPrimaryStream()));
    connect(recOps,SIGNAL(secondaryRecord(bool)),this,SLOT(recordSecondaryStream()));
    connect(recOps,SIGNAL(cancelRecording()),this,SLOT(cancelAllRecording()));

    connect(capTimer,SIGNAL(timeout()),this,SLOT(drawMatCV()));

    connect(sMenu,SIGNAL(showChannelSelWin(bool)),chanSel,SLOT(setVisible(bool)));


   matViewer = new cvMatViewer();
   pipViewer = new cvMatViewer();
   recsWin->setPIP(pipViewer);


   ui->graphicsView->scene()->addWidget(matViewer);
   ui->graphicsView->scene()->addWidget(sMenu);
   ui->graphicsView->scene()->addWidget(sVis);
   ui->graphicsView->scene()->addWidget(notes);
   ui->graphicsView->scene()->addWidget(recsWin);
   ui->graphicsView->scene()->addWidget(recOps);
   ui->graphicsView->scene()->addWidget(chanSel);


   connect(sVis,SIGNAL(sendPause(bool)),this,SLOT(setPaused(bool)));
   connect(sVis,SIGNAL(sendPlay(bool)),this,SLOT(setPlay(bool)));

   notes->setTopText("Test Top");
   notes->setBottomText("Test Bottom");
   notes->updateTime();

   matViewer->show();

   flashBackOn = true;

   vThread->start();
   playBackTimer->start();
   timer->start();


}




MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setUpGView()
{
    myScene = new QGraphicsScene();

    ui->graphicsView->setScene(myScene);
}

void MainWindow::drawMat(byte *data)
{

    Mat myFrame(rect.height()*2,rect.width()*2,CV_8UC3, data);
    fillBuffers(myFrame,true);
    mainBuffer.append(myFrame);
}

void MainWindow::drawMat2(byte *data)
{
    Mat myFrame(rect.height()*2,rect.width()*2,CV_8UC3, data);
    fillBuffers(myFrame,false);
    secondaryBuffer.append(myFrame);
}

void MainWindow::drawMatCV()
{
    Mat frame;

    cap >> frame;



    if(!isPIP){
        Mat temp;

        Size reSiz;
        reSiz.height = rect.height();
        reSiz.width = rect.width();

        cv::resize(frame,temp,reSiz);
        matViewer->showImg(temp);

    }else{
        Mat temp;

        Size reSiz;
        reSiz.height = 720/8;
        reSiz.width = 1280/8;

        cv::resize(frame,temp,reSiz);
        pipViewer->showImg(temp);
    }
}


void MainWindow::fillBuffers(Mat mat, bool primaryCh)
{
    if(primaryCh){
    if(toggle){

        Mat reSiz;
        sz.height = mat.rows/8;
        sz.width = mat.cols/8;
        cv::resize(mat,reSiz,sz);

    ldBuffer.append(mat);
    crBuffer.append(reSiz);
    }

    if(toggle2){
        Mat reSiz;
        sz.height = mat.rows/8;
        sz.width = mat.cols/8;
        cv::resize(mat,reSiz,sz);

        bsBuffer.append(reSiz);

    }



    frameCounter++;

    if(frameCounter % 50 == 0){

        if(toggle == true){
            emit ldcrbufferFull(true);

        }


        toggle ^= true;

    }

    if(frameCounter % 100 == 0){
        if(toggle2 == true){
            emit bsbufferFull(true);
        }

        toggle2 ^= true;
    }

    }else{

        if(toggle3){

            Mat reSiz;
            sz.height = rect.height();
            sz.width = rect.width();
            cv::resize(mat,reSiz,sz);




        ldBuffer2.append(mat);
        crBuffer2.append(reSiz);
        }

        if(toggle4){
            Mat reSiz;
            sz.height = mat.rows/8;
            sz.width = mat.cols/8;
            cv::resize(mat,reSiz,sz);

            bsBuffer2.append(reSiz);

        }



        frameCounter2++;

        if(frameCounter2 % 50 == 0){

            if(toggle3 == true){
                emit ldcrbufferFull(false);

            }


            toggle3 ^= true;

        }

        if(frameCounter2 % 100 == 0){
            if(toggle4 == true){
                emit bsbufferFull(false);
            }

            toggle4 ^= true;
        }

    }

}

void MainWindow::recHighCuts(bool rec)
{
    crPrimary = rec;

    if(primaryOnComm && !crPrimary)
        chkPrimaryForCB();

}

void MainWindow::recBS(bool rec)
{
      bsPrimary = rec;
}

void MainWindow::recNoLogo(bool rec)
{
     ldPrimary = rec;


}

void MainWindow::recHighCuts2(bool rec)
{

    crSecondary = rec;

    if(secondaryOnComm && !crSecondary)
        chkSecondaryForCB();
}

void MainWindow::recBS2(bool rec)
{
      bsSecondary = rec;
}

void MainWindow::recNoLogo2(bool rec)
{
    ldSecondary = rec;
}

void MainWindow::addFramesToThreads(bool primaryCh)
{
    if(primaryCh){
    if(!HCRateThread->isRunning()){
        HCRateThread->addFrames(crBuffer);
        if(flashBackOn)
        HCRateThread->start();
    }



    if(!LDThread->isRunning()){
      LDThread->addFrames(ldBuffer);
      if(flashBackOn)
      LDThread->start();
    }

    crBuffer.clear();
    ldBuffer.clear();

    }else{
        if(!HCRateThread2->isRunning()){
            HCRateThread2->addFrames(crBuffer2);
            if(flashBackOn)
            HCRateThread2->start();
        }



        if(!LDThread2->isRunning()){
          LDThread2->addFrames(ldBuffer2);
          if(flashBackOn)
          LDThread2->start();
        }

        crBuffer2.clear();
        ldBuffer2.clear();


    }

}

void MainWindow::addFramesToBSThread(bool primaryCh)
{
    if(primaryCh){
    if(!bsDetectThread->isRunning()){
        bsDetectThread->readInFrames(bsBuffer);
        if(flashBackOn)
        bsDetectThread->start();
    }

     bsBuffer.clear();
    }else{
        if(!bsDetectThread2->isRunning()){
            bsDetectThread2->readInFrames(bsBuffer2);
            if(flashBackOn)
            bsDetectThread2->start();
        }

         bsBuffer2.clear();
    }
}

void MainWindow::playBackFromBuf()
{
    if(!isPIP){
       if(primary){
           if(!mainBuffer.isEmpty()){

               Size reSiz;
               reSiz.height = rect.height();
               reSiz.width = rect.width();


               Mat temp = mainBuffer.at(0);



               Mat temp2;
               cv::resize(temp,temp2,reSiz);
               matViewer->showImg(temp2);

               if(primeRec && !primaryOnComm){
                   writer1 << temp2;
               }


           }
           }else{
               if(!secondaryBuffer.isEmpty()){
                   Size reSiz;
                   reSiz.height = rect.height();
                   reSiz.width = rect.width();



                   Mat temp = secondaryBuffer.at(0);
                   Mat temp2;
                   cv::resize(temp,temp2,reSiz);
                   matViewer->showImg(temp2);

                   if(secRec && !secondaryOnComm){
                       writer2 << temp2;
                   }


               }

           }
   }
       else{
           if(primary){
               if(!mainBuffer.isEmpty()){

                   Size reSiz;
                   reSiz.height = 720/8;
                   reSiz.width = 1280/8;


                   Mat temp = mainBuffer.at(0);

                   if(primeRec && !primaryOnComm){
                       writer1 << temp;
                   }


                   Mat temp2;
                   cv::resize(temp,temp2,reSiz);
                   pipViewer->showImg(temp2);


           }
           }else{
               if(!secondaryBuffer.isEmpty()){
                   Size reSiz;
                   reSiz.height = 720/8;
                   reSiz.width = 1280/8;



                   Mat temp = secondaryBuffer.at(0);

                   if(secRec && !secondaryOnComm){
                       writer2 << temp;
                   }

                   Mat temp2;
                   cv::resize(temp,temp2,reSiz);
                   pipViewer->showImg(temp2);


               }
       }
   }
}

void MainWindow::chkPrimaryForCB()
{
    if(primaryOnComm == false){

    if(ldPrimary && bsPrimary || ldPrimary && crPrimary){
        primaryOnComm = true;

        if(!secondaryOnComm){
            notes->setTopText("Switching to Secondary");
            notes->setBottomText("Primary on Commercial Break at");
            notes->updateTime();
            notes->setVisible(true);
            notesTimer->start();
            //Switching to secondary channel
        }


        }

    }else{

        if(!ldPrimary && !bsPrimary && !crPrimary){
            primaryOnComm = false;

            if(!primary){

                notes->setTopText("Switching to Primary");
                notes->setBottomText("Primary Finished Commercial Break at");
                notes->updateTime();
                notes->setVisible(true);
                notesTimer->start();
            }


        }


    }
}

void MainWindow::chkSecondaryForCB()
{
    if(secondaryOnComm == false){
     if(ldSecondary && bsSecondary || ldSecondary && crSecondary){
       secondaryOnComm = true;

       if(primary){
           notes->setTopText("Secondary Update");
           notes->setBottomText("Secondary on Commercial Break at");
           notes->updateTime();
           notes->setVisible(true);
           notesTimer->start();
       }




        }

    }else{
        if(!ldSecondary && !bsSecondary && !crSecondary){

        secondaryOnComm = false;

        if(primary){
            notes->setTopText("Secondary Update");
            notes->setBottomText("Secondary is off Commercial Break at");
            notes->updateTime();
            notes->setVisible(true);
            notesTimer->start();
        }

    }
    }

}

void MainWindow::chkPrimaryCutRate()
{

    if(!crPrimary){
        chkPrimaryForCB();
    }
}

void MainWindow::chkSecondaryCutRate()
{

   if(!crSecondary){
       chkSecondaryForCB();
   }
}

void MainWindow::toggleFlashBack(bool on)
{
    if(on){
        HCRateThread->start();
        HCRateThread2->start();

        bsDetectThread->start();
        bsDetectThread2->start();

        LDThread->start();
        LDThread2->start();


        flashBackOn = true;

        timer->start();

    }else{
        HCRateThread->quit();
        HCRateThread2->quit();

        bsDetectThread->quit();
        bsDetectThread2->quit();

        LDThread->quit();
        LDThread2->quit();

        flashBackOn = false;
        timer->stop();
    }
}

void MainWindow::changeToPIP(bool inPIP)
{
    this->isPIP = inPIP;

}

void MainWindow::recordPrimaryStream()
{
    Size sz;
    sz.height = rect.height();
    sz.width = rect.width();

    writer1.open(recordFolder.toStdString() + "/PrimaryStream.avi",CV_FOURCC('M','P','E','G'),vid1FPS,sz,true);

    if(!writer1.isOpened()){
        qDebug() << "Failed to open writer";
    }else{
        primeRec = true;
    }
}

void MainWindow::recordSecondaryStream()
{
    Size sz;
    sz.height = rect.height();
    sz.width = rect.width();

    writer2.open(recordFolder.toStdString() + "/SecondaryStream.avi",CV_FOURCC('M','P','E','G'),vid2FPS,sz,true);

    if(!writer2.isOpened()){
        qDebug() << "Failed to open writer";
    }else{

        secRec = true;
    }
}

void MainWindow::cancelAllRecording()
{
    primeRec = false;
    secRec = false;

    writer1.release();
    writer2.release();
}

void MainWindow::setUpPB(const char *playBack)
{
    HCRateThread->quit();
    HCRateThread2->quit();
    LDThread->quit();
    LDThread2->quit();
    bsDetectThread->quit();
    bsDetectThread2->quit();

    emit breakVthread(true);
    while(vThread->isRunning()){

        qDebug() << "Vthread is running";
    }

    timer->stop();
    playBackTimer->stop();
    primary = true;
    flashBackOn = false;

    if(!vThread->isRunning()){

        if(cap.isOpened()){
            capTimer->stop();
            cap.release();
        }

        cap.open(playBack);
        if(cap.isOpened()){
        capTimer->setInterval(1000/30);
        capTimer->start();
        }else{
            qDebug() << "Could not open Recording";
            emit breakVthread(false);
            vThread->start();
            playBackTimer->start();
        }
    }
}

void MainWindow::closeNote()
{
    notes->setVisible(false);
    if(primaryOnComm)
        primary = false;

    if(!primaryOnComm)
        primary = true;

}




void MainWindow::keyPressEvent(QKeyEvent *a)
{
    //Do a switch here with a char to determine which key was pressed...
    //May be a long switch statement.

    int keyPressed = a->key();

    qDebug() << keyPressed;

    switch(keyPressed){

    case 65:
        mtoggle ^= true;
        break;

    case 66:
        stoggle ^= true;
        sMenu->setVisible(stoggle);
        break;

    case 67:
        sliderToggle ^= true;
        sVis->setVisible(sliderToggle);
        break;

    case 68:
        notifs ^= true;
        notes->setVisible(notifs);
        break;

    case 69:
        primary ^=true;
        break;

    case 96:
        isPIP = false;

        recOps->setVisible(false);
        recsWin->setVisible(false);
        chanSel->setVisible(false);
        break;

    case 16777216:
        this->close();
        break;



    }
}
