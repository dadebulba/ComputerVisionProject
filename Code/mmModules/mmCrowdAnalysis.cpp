#include "../mmLib.h"
namespace mmLib {
namespace mmModules{
mmCrowdAnalysis::mmCrowdAnalysis()
{
    //TODO correct distance calculator for accWindosComparasionSize>3
    blockSize=2;
    windowSize=5;
    area_alarm=0.6;
    accWindowsComparasionSize=3;
    numLabel=(accWindowsComparasionSize*accWindowsComparasionSize)-1;
    firstFrame=true;
    buildLabelDistanceMatrix();
    buildPseudoColorMatrix();
    //gc = new GCoptimizationGridGraph(windowSize,windowSize,numLabel);
    memory=10;

}
mmCrowdAnalysis::mmCrowdAnalysis(double alarm_threshold)
{
    //TODO correct distance calculator for accWindosComparasionSize>3
    blockSize=2;
    windowSize=5;
    area_alarm=alarm_threshold;
    accWindowsComparasionSize=3;
    numLabel=(accWindowsComparasionSize*accWindowsComparasionSize)-1;
    firstFrame=true;
    buildLabelDistanceMatrix();
    buildPseudoColorMatrix();
    //gc = new GCoptimizationGridGraph(windowSize,windowSize,numLabel);
    memory=10;

}
CrowdResult mmCrowdAnalysis::processFrame(cv::Mat frame, cv::Mat bgFrame)
{

    CrowdResult myResult;
    std::vector<CrowdArea> crowArea, currentCrows;
    cv::Mat grayFrame;
    cv::Mat outColor;
    frame.copyTo(outColor);
    cv::cvtColor(frame,grayFrame,CV_RGB2GRAY);
    nowGrayFrame=grayFrame.clone();
    width=frame.size().width;
    height=frame.size().height;
    int rows=frame.rows;
    int cols= frame.cols;
    int borderSize=(accWindowsComparasionSize*blockSize)/2;
    int i,j,linearIndex,t,z,gcindex,l;
    if(firstFrame)
    {
        frameNumber=0;
        accMatrix = cv::Mat(rows,cols,CV_8UC3,cv::Scalar(0,0,0));

        gcMatrix  = cv::Mat(rows,cols,CV_8UC3,cv::Scalar(0,0,0));
        accuMulatorMatrix= std::vector< std::vector<double> >(rows*cols,std::vector<double>((accWindowsComparasionSize*accWindowsComparasionSize)-1,0.0));
        fastAccuMulatorMatrix = std::vector< std::vector<double> >(rows*cols,std::vector<double>(memory,-1.0));
        prevAccuMulatorMatrix= std::vector< std::vector<double> >(rows*cols,std::vector<double>((accWindowsComparasionSize*accWindowsComparasionSize)-1,0.0));
        firstFrame=false;
        memory_filled=false;
        now_memory=0;

    }
    fastAccMatrix = cv::Mat(rows,cols,CV_8UC3,cv::Scalar(0,0,0));
    for(i=borderSize,linearIndex=0;i<rows-borderSize;i=i+blockSize)
    {
        for(j=borderSize;j<cols-borderSize;j=j+blockSize,linearIndex++)
        {

            cv::Rect blockRoi = cv::Rect(j,i,blockSize,blockSize);
            cv::Mat bgArea = cv::Mat(bgFrame,blockRoi);
            //Skip non bg area
            if(cv::countNonZero(bgArea)>((blockSize*blockSize)/2)-1)
            {
                memory_filled=true;
                std::vector<double> correlationMatrix=computeCorrelationMatrix(i,j);
                //extract winner
                int index_winner = max_element(correlationMatrix.begin(), correlationMatrix.end()) - correlationMatrix.begin();
                //store_winner
                if(correlationMatrix[index_winner]>0)
                {
                    accuMulatorMatrix[linearIndex][index_winner]+=1;
                    fastAccuMulatorMatrix[linearIndex][now_memory]=index_winner;
                    //get accumulator winner
                    int acc_index_winner = max_element(accuMulatorMatrix[linearIndex].begin(), accuMulatorMatrix[linearIndex].end()) - accuMulatorMatrix[linearIndex].begin();
                    cv::rectangle(accMatrix,cv::Rect(j,i,blockSize,blockSize),cv::Scalar(pseudoColorMatrix[acc_index_winner][0],pseudoColorMatrix[acc_index_winner][1],pseudoColorMatrix[acc_index_winner][2]),-1);
                }

                //Memory filled?

                //GC cycle
                int index=0;
                int cols=j/blockSize;
                int row=i/blockSize;
                //std::cout << "\n Centr: "<< row << " " << cols << std::endl;
                t=row-(windowSize-1)/2;
                z=cols-(windowSize-1)/2;

            }
            else
                fastAccuMulatorMatrix[linearIndex][now_memory]=-1;
            if(memory_filled)
            {
                int max_count=-1;
                int label_count=-1;
                for(int fasti=0;fasti<numLabel;fasti++)
                {
                    int count = (int)std::count(fastAccuMulatorMatrix[linearIndex].begin(),fastAccuMulatorMatrix[linearIndex].end(),fasti);
                    if(count>max_count)
                    {
                        max_count=count;
                        label_count=fasti;
                    }
                }
                if(max_count>0)
                {
                    cv::rectangle(fastAccMatrix,cv::Rect(j,i,blockSize,blockSize),cv::Scalar(pseudoColorMatrix[label_count][0],pseudoColorMatrix[label_count][1],pseudoColorMatrix[label_count][2]),-1);
                }
            }
        }
    }

    int test[] = {4,3,6,1,7,5,2,0};
    int minSize=100;
    int fast_min_size=150;
    double areaDiffTresh = 0.10;
    cv::Mat onColor,fastOnColor;
    cv::Mat outc=cv::Mat::zeros(accMatrix.size(),CV_8UC3);
    cv::Mat fast_outc=cv::Mat::zeros(accMatrix.size(),CV_8UC3);
    cv::vector<cv::vector<cv::Point> > contours_clean, fast_contours_clean;
    std::vector<int> colors_fast, colors;
    for(int kk=0;kk<pseudoColorMatrix.size();kk++)
    {
        int cc=test[kk];
        cv::inRange(accMatrix,cv::Scalar(pseudoColorMatrix[cc][0],pseudoColorMatrix[cc][1],pseudoColorMatrix[cc][2]),cv::Scalar(pseudoColorMatrix[cc][0],pseudoColorMatrix[cc][1],pseudoColorMatrix[cc][2]),onColor);
        cv::inRange(fastAccMatrix,cv::Scalar(pseudoColorMatrix[cc][0],pseudoColorMatrix[cc][1],pseudoColorMatrix[cc][2]),cv::Scalar(pseudoColorMatrix[cc][0],pseudoColorMatrix[cc][1],pseudoColorMatrix[cc][2]),fastOnColor);
        cv::vector<cv::vector<cv::Point> > contours;
        cv::vector<cv::Vec4i> hierarchy;
        cv::findContours( onColor, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
        //Clean Contonurs
        for( int i = 0; i< contours.size(); i++ )
        {
            if(contours[i].size()>minSize)
            {
                contours_clean.push_back(contours[i]);
                colors.push_back(cc);
            }
        }
        cv::findContours( fastOnColor, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
        for( int jj = 0; jj< contours.size(); jj++ )
        {
            if(contours[jj].size()>fast_min_size)
            {
                fast_contours_clean.push_back(contours[jj]);
                colors_fast.push_back(cc);
            }
        }
    }
    double treshArea_same=0.5,treshArea_diff=0.5;
    std::vector<int> toRemove;
    for(int n=0;n<contours_clean.size();n++)
    {
        cv::vector<cv::Point> myRectPoint_n;
        cv::RotatedRect minAreaN = cv::minAreaRect(contours_clean[n]);
        cv::Mat ImgN=cv::Mat::zeros(accMatrix.size(),CV_8UC1);
        double AreaN = minAreaN.size.area();
        cv::vector<cv::vector<cv::Point> > contours_rect_n;
        //Area N
        cv::Point2f rect_points_n[4];
        minAreaN.points(rect_points_n);
        for(int p=0;p<4;p++)
            myRectPoint_n.push_back(cv::Point(rect_points_n[p].x,rect_points_n[p].y));
        contours_rect_n.push_back(myRectPoint_n);
        //cv::drawContours( outc, contours_rect_n, 0, cv::Scalar(255,255,255),1);

       // std::cout << "Contourns size: " << contours_rect_n.size() << std::cout;
        cv::drawContours( ImgN, contours_rect_n, 0, cv::Scalar(255),-1);
        contours_rect_n.empty();
        for(int m=n+1; m<contours_clean.size();m++)
        {
            cv::RotatedRect minAreaM = cv::minAreaRect(contours_clean[m]);
            double AreaM = minAreaM.size.area();
            cv::vector<cv::vector<cv::Point> > contours_rect_m;
            cv::Mat ImgM=cv::Mat::zeros(accMatrix.size(),CV_8UC1);
            cv::Mat ImgSum=cv::Mat::zeros(accMatrix.size(),CV_8UC1);
            cv::vector<cv::Point> myRectPoint_m;
            //Area M
            cv::Point2f rect_points_m[4];
            minAreaM.points(rect_points_m);
            for(int p=0;p<4;p++)
                myRectPoint_m.push_back(cv::Point(rect_points_m[p].x,rect_points_m[p].y));
            contours_rect_m.push_back(myRectPoint_m);
            //Area in comune
            //cv::drawContours( outc, contours_rect_m, 0, cv::Scalar(255,255,255),1);
            cv::drawContours( ImgM, contours_rect_m, 0, cv::Scalar(255),-1);
            contours_rect_m.empty();
            cv::bitwise_and(ImgM,ImgN,ImgSum);
            int intersectSize=cv::countNonZero(ImgSum);
//            if(intersectSize==0 && AreaM>30000)
//            {
//                cv::imwrite("ImgM.jpg",ImgM);
//                cv::imwrite("ImgN.jpg",ImgN);
//                cv::imwrite("ImgSum.jpg",ImgSum);
//            }

            //std::cout << "Intesect size " << intersectSize << " area N " << AreaN << " N " << n << " area M " << AreaM << "m "<< m << " size " << contours_clean.size() << std::endl;
            double treshArea;
            double areaDiff = abs(AreaM-AreaN);
            if(colors[n]!=colors[m])
                treshArea=treshArea_diff;
            else
                treshArea=treshArea_same;

            if(AreaM>AreaN)
            {
                double areaRatio=areaDiff/AreaM;
                //Check if AreaN intesect is > 0.5
//                if(colors[n]!=colors[m])
//                {
//                    //std::cout << "Area Ratio" <<  areaRatio << std::endl;
//                    if((double)intersectSize/(double)AreaN>treshArea && areaRatio>areaDiffTresh)
//                    {
//                        //  std::cout << " Push N " << n << std::endl;
//                        toRemove.push_back(n);
//                    }
//                }
//                else
//                {
                    if((double)intersectSize/(double)AreaN>treshArea)
                    {
                        //  std::cout << " Push N " << n << std::endl;
                        toRemove.push_back(n);
                    }
//                }
            }
            else
            {
                double areaRatio=areaDiff/AreaN;
//                if(colors[n]!=colors[m])
//                {
//                    //std::cout << "Area Ratio" <<  areaRatio << std::endl;
//                    if((double)intersectSize/(double)AreaM>treshArea && areaRatio>areaDiffTresh)
//                    {
//                        //std::cout << " Push M " << m << std::endl;
//                        toRemove.push_back(m);
//                    }
//                }
//                else{
                    if((double)intersectSize/(double)AreaM>treshArea)
                    {
                        //std::cout << " Push M " << m << std::endl;
                        toRemove.push_back(m);
                    }
//                }
            }

        }
    }

//    for(int p=0;p<toRemove.size();p++)
//    {
//        std::cout << "element: " <<  toRemove[p] << " size " << contours_clean.size() << " list size " << toRemove.size() << std::endl;
//    }
    std::sort(toRemove.begin(),toRemove.end());
    for( int i = 0; i< contours_clean.size(); i++ )
    {
        if(!(std::binary_search(toRemove.begin(),toRemove.end(),i))){
            //std::cout << "Index: " << i << std::endl;
            cv::RotatedRect minArea = cv::minAreaRect(contours_clean[i]);

            cv::vector<cv::vector<cv::Point> > contours_rect;
            cv::vector<cv::Point> myRectPoint;

            cv::Point2f rect_points[4];
            minArea.points(rect_points);

            for(int p=0;p<4;p++)
                myRectPoint.push_back(cv::Point(rect_points[p].x,rect_points[p].y));
            contours_rect.push_back(myRectPoint);

            //cv::fitLine(contours[i],line,CV_DIST_L2,0,0.01,0.01);

            //minArea::points()
            //
            //cv::line(outc,cv::Point(x1,y1),cv::Point(x2,y2),cv::Scalar(pseudoColorMatrix[cc][0],pseudoColorMatrix[cc][1],pseudoColorMatrix[cc][2]),5,CV_AA);
            //cv::ellipse(outc,minArea, cv::Scalar(pseudoColorMatrix[cc][0],pseudoColorMatrix[cc][1],pseudoColorMatrix[cc][2]),1);
            // cv::drawContours( outc, contours_rect, 0, cv::Scalar(pseudoColorMatrix[colors[i]][0],pseudoColorMatrix[colors[i]][1],pseudoColorMatrix[colors[i]][2]),-1);
            CrowdArea tempArea;
            tempArea.label=colors[i];
//            std::cout << colors[i] << std::endl;
//            std::cout << i << std::endl;
            tempArea.AreaPoint = myRectPoint;
            crowArea.push_back(tempArea);
            cv::drawContours( outColor, contours_rect, 0, cv::Scalar(pseudoColorMatrix[colors[i]][0],pseudoColorMatrix[colors[i]][1],pseudoColorMatrix[colors[i]][2]),1);
        }
    }
    double allarTresh=area_alarm;
    for( int ii = 0; ii< fast_contours_clean.size(); ii++ )
    {
        cv::drawContours( fast_outc, fast_contours_clean, ii, cv::Scalar(pseudoColorMatrix[colors_fast[ii]][0],pseudoColorMatrix[colors_fast[ii]][1],pseudoColorMatrix[colors_fast[ii]][2]),-1);
        //Morphologico
        //cv::medianBlur(fast_outc,fast_outc,7);
        //Anomaly detector
        cv::Mat ImgFast=cv::Mat::zeros(accMatrix.size(),CV_8UC1);
        cv::drawContours( ImgFast, fast_contours_clean, ii, cv::Scalar(255),-1);
        double AreaFast = cv::countNonZero(ImgFast);
        for(int kk=0; kk<contours_clean.size();kk++)
        {
            if(colors_fast[ii]!=colors[kk])
            {
                cv::Mat ImgSlow=cv::Mat::zeros(accMatrix.size(),CV_8UC1);
                cv::drawContours( ImgSlow, contours_clean, kk, cv::Scalar(255),-1);
                cv::Mat ImgSum=cv::Mat::zeros(accMatrix.size(),CV_8UC1);
                cv::bitwise_and(ImgFast,ImgSlow,ImgSum);
                int intersectSize=cv::countNonZero(ImgSum);
                if((double)intersectSize/(double)AreaFast>allarTresh){
                    CrowdArea tempAlarm;
                    tempAlarm.label=ii;
                    tempAlarm.AreaPoint = fast_contours_clean[ii];
                    currentCrows.push_back(tempAlarm);
                    cv::drawContours( outColor, fast_contours_clean, ii, cv::Scalar(pseudoColorMatrix[colors_fast[ii]][0],pseudoColorMatrix[colors_fast[ii]][1],pseudoColorMatrix[colors_fast[ii]][2]),-1);
                }

            }
        }
    }

    prevAccuMulatorMatrix = accuMulatorMatrix;
    prevGrayFrame=grayFrame.clone();
    prevBGFrame=bgFrame.clone();
    frameNumber++;
    std::vector<cv::Mat> resultVector;
    resultVector.push_back(accMatrix.clone());
    resultVector.push_back(outColor.clone());
    resultVector.push_back(fastAccMatrix.clone());
    resultVector.push_back(fast_outc);
    //resultVector.push_back(gcMatrix.clone());Ã¹
    now_memory++;
    if(now_memory==memory)
    {
        now_memory=0;
        // memory_filled=true;
    }
    myResult.CrowdMat=resultVector;
    myResult.CrowdAreas=crowArea;
    myResult.CurrentCrows= currentCrows;
    //return resultVector;
    return myResult;
}
std::vector<double> mmCrowdAnalysis::computeCorrelationMatrix(int centerI, int centerJ)
{
    std::vector<double> correlationMatrix;
    int i,j,z,t,m,n;
    int center = (accWindowsComparasionSize*accWindowsComparasionSize)/2;
    for(i=centerI-((accWindowsComparasionSize-1)*blockSize)/2;i<=centerI+((accWindowsComparasionSize-1)*blockSize)/2;i+=blockSize)
    {
        //std::cout << "i index: "<< i << " Ceneter " << centerI << std::endl;
         for(j=centerJ-((accWindowsComparasionSize-1)*blockSize)/2;j<=centerJ+((accWindowsComparasionSize-1)*blockSize)/2;j+=blockSize)
         {
             double correlationvalue=0;
             //Scan single block
             for(z=i,m=centerI;z<i+blockSize;z++,m++)
             {
                 for(t=j,n=centerJ;t<j+blockSize;t++,n++)
                 {
                     correlationvalue+=1.0 / (1.0 + abs(double(nowGrayFrame.at<uchar> (m, n) - double(prevGrayFrame.at<uchar> (z, t)))));
                 }
             }
             if(i==centerI && j==centerJ)
                 int zip=0;
                 //correlationMatrix.push_back(0);
             else
                correlationMatrix.push_back(correlationvalue);
         }
    }
    return correlationMatrix;
}
void mmCrowdAnalysis::buildLabelDistanceMatrix()
{
    int labelNumber = (accWindowsComparasionSize*accWindowsComparasionSize)-1;
    int i,j;
    labelDistanceMatrix=std::vector< std::vector<double> >(labelNumber,std::vector<double>(labelNumber,0.0));
    for(i=0;i<labelNumber;i++)
    {
        for(j=0;j<labelNumber;j++)
        {
            double distance= std::min(abs(i-j),labelNumber-abs(i-j));
            labelDistanceMatrix[i][j]=distance;
        }
    }

}
void mmCrowdAnalysis::buildPseudoColorMatrix()
{
    /*Reference
      http://bytefish.de/blog/colormaps_in_opencv
    */
    float r[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00588235294117645,0.02156862745098032,0.03725490196078418,0.05294117647058827,0.06862745098039214,0.084313725490196,0.1000000000000001,0.115686274509804,0.1313725490196078,0.1470588235294117,0.1627450980392156,0.1784313725490196,0.1941176470588235,0.2098039215686274,0.2254901960784315,0.2411764705882353,0.2568627450980392,0.2725490196078431,0.2882352941176469,0.303921568627451,0.3196078431372549,0.3352941176470587,0.3509803921568628,0.3666666666666667,0.3823529411764706,0.3980392156862744,0.4137254901960783,0.4294117647058824,0.4450980392156862,0.4607843137254901,0.4764705882352942,0.4921568627450981,0.5078431372549019,0.5235294117647058,0.5392156862745097,0.5549019607843135,0.5705882352941174,0.5862745098039217,0.6019607843137256,0.6176470588235294,0.6333333333333333,0.6490196078431372,0.664705882352941,0.6803921568627449,0.6960784313725492,0.7117647058823531,0.7274509803921569,0.7431372549019608,0.7588235294117647,0.7745098039215685,0.7901960784313724,0.8058823529411763,0.8215686274509801,0.8372549019607844,0.8529411764705883,0.8686274509803922,0.884313725490196,0.8999999999999999,0.9156862745098038,0.9313725490196076,0.947058823529412,0.9627450980392158,0.9784313725490197,0.9941176470588236,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0.9862745098039216,0.9705882352941178,0.9549019607843139,0.93921568627451,0.9235294117647062,0.9078431372549018,0.892156862745098,0.8764705882352941,0.8607843137254902,0.8450980392156864,0.8294117647058825,0.8137254901960786,0.7980392156862743,0.7823529411764705,0.7666666666666666,0.7509803921568627,0.7352941176470589,0.719607843137255,0.7039215686274511,0.6882352941176473,0.6725490196078434,0.6568627450980391,0.6411764705882352,0.6254901960784314,0.6098039215686275,0.5941176470588236,0.5784313725490198,0.5627450980392159,0.5470588235294116,0.5313725490196077,0.5156862745098039,0.5};
    float g[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.001960784313725483,0.01764705882352935,0.03333333333333333,0.0490196078431373,0.06470588235294117,0.08039215686274503,0.09607843137254901,0.111764705882353,0.1274509803921569,0.1431372549019607,0.1588235294117647,0.1745098039215687,0.1901960784313725,0.2058823529411764,0.2215686274509804,0.2372549019607844,0.2529411764705882,0.2686274509803921,0.2843137254901961,0.3,0.3156862745098039,0.3313725490196078,0.3470588235294118,0.3627450980392157,0.3784313725490196,0.3941176470588235,0.4098039215686274,0.4254901960784314,0.4411764705882353,0.4568627450980391,0.4725490196078431,0.4882352941176471,0.503921568627451,0.5196078431372548,0.5352941176470587,0.5509803921568628,0.5666666666666667,0.5823529411764705,0.5980392156862746,0.6137254901960785,0.6294117647058823,0.6450980392156862,0.6607843137254901,0.6764705882352942,0.692156862745098,0.7078431372549019,0.723529411764706,0.7392156862745098,0.7549019607843137,0.7705882352941176,0.7862745098039214,0.8019607843137255,0.8176470588235294,0.8333333333333333,0.8490196078431373,0.8647058823529412,0.8803921568627451,0.8960784313725489,0.9117647058823528,0.9274509803921569,0.9431372549019608,0.9588235294117646,0.9745098039215687,0.9901960784313726,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0.9901960784313726,0.9745098039215687,0.9588235294117649,0.943137254901961,0.9274509803921571,0.9117647058823528,0.8960784313725489,0.8803921568627451,0.8647058823529412,0.8490196078431373,0.8333333333333335,0.8176470588235296,0.8019607843137253,0.7862745098039214,0.7705882352941176,0.7549019607843137,0.7392156862745098,0.723529411764706,0.7078431372549021,0.6921568627450982,0.6764705882352944,0.6607843137254901,0.6450980392156862,0.6294117647058823,0.6137254901960785,0.5980392156862746,0.5823529411764707,0.5666666666666669,0.5509803921568626,0.5352941176470587,0.5196078431372548,0.503921568627451,0.4882352941176471,0.4725490196078432,0.4568627450980394,0.4411764705882355,0.4254901960784316,0.4098039215686273,0.3941176470588235,0.3784313725490196,0.3627450980392157,0.3470588235294119,0.331372549019608,0.3156862745098041,0.2999999999999998,0.284313725490196,0.2686274509803921,0.2529411764705882,0.2372549019607844,0.2215686274509805,0.2058823529411766,0.1901960784313728,0.1745098039215689,0.1588235294117646,0.1431372549019607,0.1274509803921569,0.111764705882353,0.09607843137254912,0.08039215686274526,0.06470588235294139,0.04901960784313708,0.03333333333333321,0.01764705882352935,0.001960784313725483,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    float b[] = {0.5,0.5156862745098039,0.5313725490196078,0.5470588235294118,0.5627450980392157,0.5784313725490196,0.5941176470588235,0.6098039215686275,0.6254901960784314,0.6411764705882352,0.6568627450980392,0.6725490196078432,0.6882352941176471,0.7039215686274509,0.7196078431372549,0.7352941176470589,0.7509803921568627,0.7666666666666666,0.7823529411764706,0.7980392156862746,0.8137254901960784,0.8294117647058823,0.8450980392156863,0.8607843137254902,0.8764705882352941,0.892156862745098,0.907843137254902,0.9235294117647059,0.9392156862745098,0.9549019607843137,0.9705882352941176,0.9862745098039216,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0.9941176470588236,0.9784313725490197,0.9627450980392158,0.9470588235294117,0.9313725490196079,0.915686274509804,0.8999999999999999,0.884313725490196,0.8686274509803922,0.8529411764705883,0.8372549019607844,0.8215686274509804,0.8058823529411765,0.7901960784313726,0.7745098039215685,0.7588235294117647,0.7431372549019608,0.7274509803921569,0.7117647058823531,0.696078431372549,0.6803921568627451,0.6647058823529413,0.6490196078431372,0.6333333333333333,0.6176470588235294,0.6019607843137256,0.5862745098039217,0.5705882352941176,0.5549019607843138,0.5392156862745099,0.5235294117647058,0.5078431372549019,0.4921568627450981,0.4764705882352942,0.4607843137254903,0.4450980392156865,0.4294117647058826,0.4137254901960783,0.3980392156862744,0.3823529411764706,0.3666666666666667,0.3509803921568628,0.335294117647059,0.3196078431372551,0.3039215686274508,0.2882352941176469,0.2725490196078431,0.2568627450980392,0.2411764705882353,0.2254901960784315,0.2098039215686276,0.1941176470588237,0.1784313725490199,0.1627450980392156,0.1470588235294117,0.1313725490196078,0.115686274509804,0.1000000000000001,0.08431372549019622,0.06862745098039236,0.05294117647058805,0.03725490196078418,0.02156862745098032,0.00588235294117645,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int labelNumber = (accWindowsComparasionSize*accWindowsComparasionSize)-1,i;

    pseudoColorMatrix = std::vector<std::vector<int> >(labelNumber, std::vector<int>(3,0));
    for(i=0;i<labelNumber;i++)
    {
        double gray=(double)(i+1)/(double)(labelNumber+1);
        pseudoColorMatrix[i][0]=(int)(255*r[int(gray*255)]);
        pseudoColorMatrix[i][1]=(int)(255*g[int(gray*255)]);
        pseudoColorMatrix[i][2]=(int)(255*b[int(gray*255)]);

    }
}
std::vector< std::vector<int> > mmCrowdAnalysis::getPseudoColorPalette()
{
    return pseudoColorMatrix;
}
cv::Mat mmCrowdAnalysis::getCrowdDirectionImage(int width_img, int height_img)
{
    cv::Mat paletteImg(height_img, width_img,CV_8UC3,cv::Scalar(0,0,0));
    int z=(accWindowsComparasionSize*accWindowsComparasionSize)-1,t,m;
    for(int j=0,m=0; j<=height_img-(height_img/accWindowsComparasionSize);j+=height_img/accWindowsComparasionSize,m++)
    {
        for(int i=0,t=0;i<=width_img-(width_img/accWindowsComparasionSize);i+=width_img/accWindowsComparasionSize,t++)
        {
            if(!(m==int(accWindowsComparasionSize/2) && t==int(accWindowsComparasionSize/2)))
            {
                z--;
                cv::Rect roi(i,j,int(width_img/accWindowsComparasionSize),int(height_img/accWindowsComparasionSize));
                cv::rectangle(paletteImg,roi,cv::Scalar(pseudoColorMatrix[z][0],pseudoColorMatrix[z][1],pseudoColorMatrix[z][2]),-1);
            }
        }

    }
    return paletteImg.clone();
}
}
}


