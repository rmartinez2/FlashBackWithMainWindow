#include "cutratedetectionthread.h"

//Store stdDev2 than get the stdDev2 of th next 5 frames, compare to current stdDev and StdDev2 that's stored
//decide if on commercial break from that realization

//Avoid counting cuts in which the stdDev of past frames and current frames aren't too far of a difference
//1) Support vector model may be a solution to creating a threshold for the each new data point in the model.
//2) Percentage diff from current stdDev

//To Effectively time the cuts per minute, implement a timer that times out every 60 seconds than restarts at 0



CutRateDetectionThread::CutRateDetectionThread(QVector<Mat> myMats, QSize frameWH, QObject *parent) :
    QThread(parent)
{
    this->myMats = myMats;
    originWindowSz = frameWH;
    cutsPerSec = 0;
    isMats = true;
    currentDev = 0;
}

CutRateDetectionThread::CutRateDetectionThread(QObject *parent):
    QThread(parent)
{
    currentDev = 0;
    cutsPerSec = 0;
}

void CutRateDetectionThread::run()
{



   if(isMats){

      // qDebug() << myMats.size();
       long long mean = 0;


       if(seconds == 15){
           seconds = 0;

           if(cutsPerSec > 5){
               qDebug() << "High Cut Rate";
            emit highCuts(true);
           }else{
               qDebug() << "Regular Cut Rate";
               emit highCuts(false);
           }

          // qDebug() << cutsPerSec;

           cutsPerSec = 0;

       }

       seconds++;

       for(int i = 0; i < myMats.size(); i++){

          int temp = getPixelData(myMats.at(i),i);

          currentFrameSums.append(temp);


          mean = mean + temp;


       mean = mean/myMats.size();


       long long hold = 0;
       for(int i = 0; i < currentFrameSums.size(); i++){

           long long temp1 = 0;
           long long temp2 = 0;
           temp1 += (currentFrameSums.at(i) - mean);
           temp2 = temp1*temp1;
           hold += temp2;

       }

       long long stdDev = (long long) sqrt(hold/(myMats.size() - 1));

       if(currentDev == 0){
           currentDev = stdDev;

       }else{

           int diff = abs(currentDev - stdDev);

           int dig1 = numbofDigits(currentDev);
           int dig2 = numbofDigits(stdDev);

           int noCut;

           if(dig1 < dig2)
               noCut = dig1;
           else
               noCut = dig2;



           if(numbofDigits(diff) > noCut){
               cutsPerSec++;

           }


           currentDev = stdDev;




       }



       currentFrameSums.clear();




   }

   this->quit();




}

}








int CutRateDetectionThread::getPixelData(Mat mat, int frame)
{

  if(mat.channels() == 3){

   long long sum = 0;

   for(int i = 0; i < mat.rows; i++){
        for(int j = 0; j < mat.cols; j++){

            cv::Vec3b bgrPixel = mat.at<cv::Vec3b>(i,j);
            sum = sum + bgrPixel[0]
                      + bgrPixel[1]
                      + bgrPixel[2];
        }
    }

   //qDebug() << "CutRate Sum " << sum << " cast to int " << (int) sum;
   return (int) sum;

    }
    else{
        qDebug() << "One Channel Mat";
    return -1;
  }




}



int CutRateDetectionThread::plusOrMinus(long long stdDevNew, long long stdDevCur, long long diff)
{

    int DN = (int) stdDevNew;
    int DC = (int) stdDevCur;
    int D = (int) diff;

    double per1 = ((double)D/(double)DN);
    double per2 = ((double)D/(double)DC);

    qDebug() << "Percent Diff/DevNew " << per1 << " Per2 " << per2;

    if(per1 < 0.5 && per2 < 0.5){
        if(per1 < per2){
            return PER2;
       } else
            return PER1;

    }else if(per1 > 0.5 && per2 < 0.5){
        return PER2;

    }else if(per1 < 0.5 && per2 > 0.5){
        return PER1;

    }else{
        return NOCH;
    }


   // return true;


}


void CutRateDetectionThread::addFrames(QVector<Mat> frames)
{
   myMats = frames;
   this->isMats = true;

}

int CutRateDetectionThread::numbofDigits(int numb)
{
    if(numb >= 10){
        if(numb >= 100){
            if(numb >= 1000){
                 if(numb >= 10000){
                     if(numb >= 100000){
                        if(numb >= 1000000){
                             if(numb >= 10000000){
                                 if(numb >= 100000000){
                                    if(numb >= 1000000000){
                                         return 10;
                                    }
                                    return 9;
                                }
                            return 8;
                             }
                        return 7;
                        }
                    return 6;
                    }
                return 5;
                }
              return 4;
             }
            return 3;
        }

        return 2;
    }else{
        return 1;
    }
}

