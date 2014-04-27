#include "videothread.h"

VideoThread::VideoThread(AVFormatContext *formCtx, AVFormatContext *formCtx2, QObject *parent) :
    QThread(parent)
{
    this->formCtx = formCtx;
    this->formatCtx2 = formCtx2;

    fOpened = true;

    //this->mutex = mutex;

    aFile_name1 = "C:/Users/Rene/Desktop/audioA.mp2";
    vFile_name1 = "C:/Users/Rene/Desktop/videoA.mpg";

    aFile1 = fopen(aFile_name1.toStdString().c_str(),"wb");
    vFile1 = fopen(vFile_name1.toStdString().c_str(),"wb");

    if(!aFile1){
        fOpened = false;
    }



    vCodecCtx1 = NULL;
    vCodecCtx2 = NULL;
    vCodec = NULL;
    vCodec2 = NULL;
    cDict = NULL;

    aCodecCtx1 = NULL;
    aCodec = NULL;
    aDict = NULL;
    audio = false;

    av_init_packet(&packet);


    findVideoStream();

    if(findAudioStream()){
        initAudioFrame();
        getBufferSize();
        fillAudioFrame();
      //  setAudioFormat();
       // setAudioDeviceInfo();
        audio = true;
    }

    breaker = false;
    AToggle = true;
    recPlayBack = false;

    if(checkCodec() == true){
    setDstInts();
    initVideoFrame();
    initConverter();
    allocRGBPic();
    }else{
        qDebug() << "Failed to open codec";
    }




}

void VideoThread::run(){

    qDebug() << "Video Thread Running";
    bool toggle = true;


    for(;;){

        if(breaker){
            break;
        }


        if(recPlayBack){
            qDebug() << "Record PlayBack Thread Running";

                    while(av_read_frame(recFormCtx,&packet) == 0){
                        if(packet.stream_index == recVidStream->index){

                            while(packet.size > 0){

                            int len = 0,framefinished = 0;

                             len = avcodec_decode_video2(rCodecCtx, rFrame, &framefinished, &packet);

                            if(framefinished){

                            sws_scale(rImgCovtCtx,rFrame->data,rFrame->linesize,
                                          0,rCodecCtx->height,rBGRFrame->data,rBGRFrame->linesize);


                            //emit sendData1(rBGRFrame->data[0]);



                           // msleep(5);
                            }
                            packet.size -= len;
                            packet.data += len;
                            }
                        }
                        av_free_packet(&packet);
                }

        }else{

        if(toggle){
            if(av_read_frame(formCtx,&packet) == 0){
                if(packet.stream_index == videostream1->index){

                    while(packet.size > 0){

                    int len = 0,framefinished = 0;

                     len = avcodec_decode_video2(vCodecCtx1, vFrame, &framefinished, &packet);

                    if(framefinished){

                    sws_scale(imgConvertCtx,vFrame->data,vFrame->linesize,
                                  0,vCodecCtx1->height,bgrFrame->data,bgrFrame->linesize);


                   emit sendData1(bgrFrame->data[0]);


                    //vArry1.append((const char*) bgrFrame->data[0]);
                   // qDebug() << (const char*)bgrFrame->data[0];


                    }
                    packet.size -= len;
                    packet.data += len;
                    }
                }else if(packet.stream_index == audioStream1->index && audio){
                   // qint64 written = 0;

                    //qDebug() << "Decoding Audio";

                    while(packet.size > 0){
                        int len = 0, framefinished = 0, data_size;

                        len = avcodec_decode_audio4(aCodecCtx1,aFrame,&framefinished,&packet);

                        if(framefinished){
                            data_size = av_samples_get_buffer_size(NULL,aCodecCtx1->channels,
                                                                   aFrame->nb_samples,aCodecCtx1->sample_fmt,1);

                            if(data_size > 0){

                               //written = aBuf.write((const char*)aFrame->data[0],data_size);

                                arry.append((const char*)aFrame->data[0]);
                               // AVSampleFormat sfmt = aCodecCtx1->sample_fmt;
                              //  int chans = aCodecCtx1->channels;

                               // qDebug() << output->state();
                               if(arry.size() >= data_size){
                                   emit sendBuffA(arry);
                                   arry.clear();
                               }
//                                }

                               // qDebug() <<(const char*) aBuf.data();

//                                if(av_sample_fmt_is_planar(sfmt)){
//                                  //  const char* packed;
//                                    sfmt = av_get_packed_sample_fmt(sfmt);
//                                    chans = 1;
//                                }

//                                getFormatFromSample(&fmt,sfmt);

//                                QString cmd = "";

//                                cmd.append("ffplay -f ");
//                                cmd.append(fmt);
//                                cmd.append(" -ac ");
//                                cmd.append(QString::number(chans,10));
//                                cmd.append(" -ar ");
//                                cmd.append(QString::number(aCodecCtx1->sample_rate,10));
//                                cmd.append(" ");
//                                cmd.append(aFile_name1);

//                                if(fOpened){
//                               // system(cmd.toStdString().c_str());
//                                }
                                //system("ffplay -f %s -ac %d -ar %d %s\n",fmt,chans,aCodecCtx1->sample_rate);
                            }
                        }
                        packet.size -= len;
                        packet.data += len;
                    }
                }
                av_free_packet(&packet);
        }


    }else{
            if(av_read_frame(formatCtx2,&packet) == 0){
                if(packet.stream_index == videostream2->index){

                    while(packet.size > 0){

                    int len = 0,framefinished = 0;

                     len = avcodec_decode_video2(vCodecCtx2, vFrame2, &framefinished, &packet);

                    if(framefinished){


                    sws_scale(imgConvertCtx2,vFrame2->data,vFrame2->linesize,
                                  0,vCodecCtx2->height,bgrFrame2->data,bgrFrame2->linesize);

                   // vArry2.append((const char*) bgrFrame2->data[0]);


                    emit sendData2(bgrFrame2->data[0]);


                    }
                    packet.size -= len;
                    packet.data += len;
                    }
                }
                av_free_packet(&packet);

        }

    }
         toggle^= true;

  }
 }




}

VideoThread::~VideoThread()
{
    av_free_packet(&packet);

    fclose(aFile1);

    avformat_free_context(formCtx);
    avformat_free_context(formatCtx2);

    avcodec_free_frame(&vFrame);
    avcodec_free_frame(&vFrame2);
    avcodec_free_frame(&bgrFrame);
    avcodec_free_frame(&bgrFrame2);

    av_free(&videostream1);
    av_free(&videostream2);
    av_free(&imgConvertCtx);
    av_free(&imgConvertCtx2);
}

void VideoThread::initVideoFrame(){
    vFrame = avcodec_alloc_frame();
    vFrame->nb_samples = vCodecCtx1->frame_size;
    vFrame->format = vCodecCtx1->sample_fmt;
    vFrame->channel_layout = vCodecCtx1->channel_layout;

    bgrFrame = avcodec_alloc_frame();
    bgrFrame->nb_samples = vCodecCtx1->frame_size;
    bgrFrame->format = AV_PIX_FMT_BGR24;
    bgrFrame->channel_layout = vCodecCtx1->channel_layout;

    bgrFrame2 = avcodec_alloc_frame();
    bgrFrame2->nb_samples = vCodecCtx2->channel_layout;
    bgrFrame2->format = AV_PIX_FMT_BGR24;
    bgrFrame2->channel_layout = vCodecCtx2->channel_layout;

    vFrame2 = avcodec_alloc_frame();
    vFrame2->nb_samples = vCodecCtx2->frame_size;
    vFrame2->format = vCodecCtx2->sample_fmt;
    vFrame2->channel_layout = vCodecCtx2->channel_layout;


}

void VideoThread::findVideoStream(){

    vidStream = av_find_best_stream(formCtx,AVMEDIA_TYPE_VIDEO,-1,-1,&vCodec,0);
    vidStream2 = av_find_best_stream(formatCtx2,AVMEDIA_TYPE_VIDEO,-1,-1,&vCodec2,0);

    if(vidStream < 0){
        qDebug() << "Could Not find Video Stream in Vid Thread";
    }else{
        videostream1 = formCtx->streams[vidStream];
        vCodecCtx1 = videostream1->codec;
        vCodecCtx1->codec = vCodec;

    }

    if(vidStream2 < 0){
        qDebug() << "Could not find video stream 2";
    }else{
        videostream2 = formatCtx2->streams[vidStream2];
        vCodecCtx2 = videostream2->codec;
        vCodecCtx2->codec = vCodec2;
    }

}

bool VideoThread::checkCodec()
{
    int checks = avcodec_open2(vCodecCtx1,vCodec,&cDict);
    int checks2 = avcodec_open2(vCodecCtx2,vCodec2,&cDict);

    if(checks < 0 && checks2 < 0)
        return false;
    else
        return true;
}

void VideoThread::initConverter()
{//sws_getContext(destWidth,destHeight,vCodecCtx->pix_fmt,destWidth,destHeight,AV_PIX_FMT_RGB24,SWS_BILINEAR,NULL,NULL,NULL);

    imgConvertCtx = sws_getContext(destWidth,destHeight,
                                   vCodecCtx1->pix_fmt,destWidth,destHeight,AV_PIX_FMT_BGR24,SWS_BICUBIC,NULL,NULL,NULL);
    imgConvertCtx2 = sws_getContext(destWidth2,destHeight2,vCodecCtx2->pix_fmt,
                                    destWidth,destHeight,AV_PIX_FMT_BGR24,SWS_BICUBIC,NULL,NULL,NULL);

    if(imgConvertCtx < 0)
        qDebug() << "Could not obtain sws convert context: VideoThread";

    if(imgConvertCtx2 < 0)
        qDebug() << "Could not obtain sws convert context2: Video Thread";
}

void VideoThread::setDstInts()
{
    destFmt = PIX_FMT_RGB;
    destHeight = vCodecCtx1->height;
    destWidth = vCodecCtx1->width;

    destHeight2 = vCodecCtx2->height;
    destWidth2 = vCodecCtx2->width;

    nSz.height = destHeight/16;
    nSz.width = destWidth/16;
}

void VideoThread::allocRGBPic()
{

    int numBgrBytes = avpicture_get_size(AV_PIX_FMT_BGR24,vCodecCtx1->width, vCodecCtx1->height);
    byte *bufBGR = (byte*)av_malloc(numBgrBytes*sizeof(byte));
    int chek2 = avpicture_fill((AVPicture*)bgrFrame,bufBGR,AV_PIX_FMT_BGR24,vCodecCtx1->width,vCodecCtx1->height);

    int numBgrBytes2 = avpicture_get_size(AV_PIX_FMT_BGR24,vCodecCtx1->width, vCodecCtx1->height);
    byte *bufBGR2 = (byte*)av_malloc(numBgrBytes2*sizeof(byte));
    int chek3 = avpicture_fill((AVPicture*)bgrFrame2,bufBGR2,AV_PIX_FMT_BGR24,vCodecCtx1->width,vCodecCtx1->height);


    if(chek2 < 0){
        qDebug()<< "Could not fill bgrFrame picture";
    }

    if(chek3 < 0){
        qDebug() << "Could not fill bgrFrame2 picture";
    }


    //myFrame.create(destHeight,destWidth,CV_8UC3);

}

void VideoThread::sendPixVec()
{
    // emit sendPix(pVec);
}

void VideoThread::sendPixHS(QPixmap pix)
{
    emit sendPix(pix);
}

void VideoThread::sendMatsPB(Mat mat)
{
    emit sendMat(mat);
}

void VideoThread::vThreadBreak(bool breaker)
{
    this->breaker = breaker;
}

bool VideoThread::findAudioStream()
{
    aStream1 = av_find_best_stream(formCtx,AVMEDIA_TYPE_AUDIO,-1,-1,&aCodec,0);

    if(aStream1 < 0){
        qDebug() << "Error finding audio stream";
        return false;
    }else{
        audioStream1 = formCtx->streams[aStream1];
        aCodecCtx1 = audioStream1->codec;
        aCodecCtx1->codec = aCodec;

       int checks = avcodec_open2(aCodecCtx1,aCodec,&aDict);

       if(checks < 0)
           return false;

       return true;
    }
}

void VideoThread::fillAudioFrame()
{
    int check = avcodec_fill_audio_frame(aFrame,aCodecCtx1->channels,aCodecCtx1->sample_fmt,(const UINT8*)sampleSize,bufferSize,0);
    if(check < 0){
        qDebug() << "Could not fill audio frame";
    }
}

void VideoThread::initAudioFrame()
{
    aFrame = avcodec_alloc_frame();
    aFrame->nb_samples = 10000;
    aFrame->format = aCodecCtx1->sample_fmt;
    aFrame->channel_layout = aCodecCtx1->channel_layout;
}

void VideoThread::getBufferSize()
{
    bufferSize = av_samples_get_buffer_size(NULL,aCodecCtx1->channels,10000,aCodecCtx1->sample_fmt,0);

//    qDebug() << "Buffer Size " << bufferSize << " " << aCodecCtx1->channels << " "
//             << 1264 << " " << aCodecCtx1->sample_fmt;

    sampleSize = (byte*) av_malloc(bufferSize);

    if(!sampleSize)
        qDebug() << "Sample Size Error";
}

void VideoThread::setAudioFormat()
{
        aFormat.setSampleRate(8000);
        aFormat.setCodec("audio/pcm");
        aFormat.setChannelCount(1);
        aFormat.setSampleSize(8);
        aFormat.setByteOrder(QAudioFormat::LittleEndian);
        aFormat.setSampleType(QAudioFormat::UnSignedInt);


}

void VideoThread::setAudioDeviceInfo()
{
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());

        if(info.isFormatSupported(aFormat)){

        //output = new QAudioOutput(aFormat);
       // connect(output,SIGNAL(stateChanged(QAudio::State)),this,SLOT(finishedPlaying(QAudio::State)));
       // output->setVolume(100);

            emit sendAO(aFormat);

        //return true;
        qDebug() << "Sent A output";
        //semit sendAO(output);

        }
         qDebug() << "format unsupported";

}

void VideoThread::getFormatFromSample(const char **fmt, AVSampleFormat sfmt)
{

    int i;
    struct sample_fmt_entry {
            enum AVSampleFormat sample_fmt; const char *fmt_be, *fmt_le;
        } sample_fmt_entries[] = {
            { AV_SAMPLE_FMT_U8,  "u8",    "u8"    },
            { AV_SAMPLE_FMT_S16, "s16be", "s16le" },
            { AV_SAMPLE_FMT_S32, "s32be", "s32le" },
            { AV_SAMPLE_FMT_FLT, "f32be", "f32le" },
            { AV_SAMPLE_FMT_DBL, "f64be", "f64le" },
        };
    fmt = NULL;

    for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++) {
            struct sample_fmt_entry *entry = &sample_fmt_entries[i];
            if (sfmt == entry->sample_fmt) {
                *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
            }
        }

    qDebug() <<
                "sample format is not supported as output format " <<
                av_get_sample_fmt_name(sfmt);
}

void VideoThread::finishedPlaying(QAudio::State state)
{
    if (state == QAudio::IdleState) {
           output->stop();
           //delete output;
    }
}

void VideoThread::receiveRequestforFrames(bool primary)
{
    if(primary){
        emit sendVBuff1(vArry1,(int)vCodecCtx1->frame_size);
    }else{
        emit sendVBuff2(vArry2,vCodecCtx2->frame_size);
    }
}

bool VideoThread::recordContext(AVFormatContext *recFrmCtx)
{
    this->recFormCtx = recFrmCtx;

    if(checkRecCodec() == true){
    setRecDstInts();
    initRecordFrame();
    initRecConverter();
    allocBGRPic();
    }else{
        qDebug() << "Failed to open codec";
        return false;
    }
    return true;

}

void VideoThread::initRecordFrame()
{
    rFrame = avcodec_alloc_frame();
    rFrame->nb_samples = rCodecCtx->frame_size;
    rFrame->format = rCodecCtx->sample_fmt;
    rFrame->channel_layout = rCodecCtx->channel_layout;

    rBGRFrame = avcodec_alloc_frame();
    rBGRFrame->nb_samples = rCodecCtx->frame_size;
    rBGRFrame->format = AV_PIX_FMT_BGR24;
    rBGRFrame->channel_layout = rCodecCtx->channel_layout;
}

void VideoThread::findRecordStream()
{
   rStream = av_find_best_stream(recFormCtx,AVMEDIA_TYPE_VIDEO,-1,-1,&rCodec,0);


    if(rStream < 0){
        qDebug() << "Could Not find Video Stream in Vid Thread";
    }else{
        recVidStream = formCtx->streams[rStream];
        rCodecCtx = recVidStream->codec;
        rCodecCtx->codec = rCodec;
    }
}

bool VideoThread::checkRecCodec()
{
    int checks = avcodec_open2(rCodecCtx,rCodec,&cDict);

    if(checks < 0)
        return false;
    else
        return true;
}

void VideoThread::initRecConverter()
{
    rImgCovtCtx = sws_getContext(rDestWidth,rDestHeight,
                                   rCodecCtx->pix_fmt,1280,720,AV_PIX_FMT_BGR24,SWS_BICUBIC,NULL,NULL,NULL);

    if(rImgCovtCtx < 0)
        qDebug() << "Could not obtain sws convert context: Record PlayBack Thread";

}

void VideoThread::setRecDstInts()
{
    rDestHeight = rCodecCtx->height;
    rDestWidth = rCodecCtx->width;
}

void VideoThread::allocBGRPic()
{
    int numBgrBytes = avpicture_get_size(AV_PIX_FMT_BGR24,rCodecCtx->width, rCodecCtx->height);
    byte *bufBGR = (byte*)av_malloc(numBgrBytes*sizeof(byte));
    int chek = avpicture_fill((AVPicture*)rBGRFrame,bufBGR,AV_PIX_FMT_BGR24,rCodecCtx->width,rCodecCtx->height);

    if(chek < 0){
        qDebug() << "Could not fill rFrameBGR picture";
    }

}






