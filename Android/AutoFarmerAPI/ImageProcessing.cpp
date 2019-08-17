#include "ImageProcessing.hpp"
#include "AutoFarmerLogger.hpp"

ImageProcessing::ImageProcessing(QObject *parent) : QObject(parent)
{
    // Do nothing
}

QPoint ImageProcessing::findImageOnImage(const QString templatePath, const QString imagePath, const float threshold, float scale)
{
    QPoint retVal;

    cv::Mat    templateImage = cv::imread(templatePath.toUtf8().constData(),CV_LOAD_IMAGE_COLOR);
    cv::Mat   sourceImage = cv::imread(imagePath.toUtf8().constData(),CV_LOAD_IMAGE_COLOR);

    if(scale != 1){
        cv::Mat _templateImageCoppy = templateImage;
        cv::resize(_templateImageCoppy,templateImage,cv::Size(),scale,scale);
    }

    //kiểm tra kích cỡ của ảnh input & template
    if (   templateImage.rows >   sourceImage.rows ||    templateImage.cols >   sourceImage.cols)
    {
        LOG_DEBUG << "Mat template must be smaller than matInput";
        return retVal;
    }else if(   templateImage.rows <= 0 ||    templateImage.cols <= 0 ||   sourceImage.rows <= 0 ||   sourceImage.cols <= 0){
        LOG_DEBUG << "Invalid Image";
        return retVal;
    }

    cv::Mat result;
    int result_cols =   sourceImage.cols -    templateImage.cols + 1;
    int result_rows =   sourceImage.rows -    templateImage.rows + 1;
    result.create(result_rows, result_cols, CV_32FC1);

    cv::matchTemplate(  sourceImage,    templateImage, result, CV_TM_CCOEFF_NORMED);

    cv::threshold(result, result, threshold, 1., CV_THRESH_TOZERO);
    double minval, maxval;
    double bestMaxval = 0;

    while (true)
    {
        cv::Point minloc, maxloc;
        cv::minMaxLoc(result, &minval, &maxval, &minloc, &maxloc);

        if (maxval > threshold)
        {
            //vẽ hình chữ nhật lên đối tượng tìm được
            if(maxval > bestMaxval){
                bestMaxval = maxval;
                retVal = QPoint(maxloc.x +    templateImage.cols/2, maxloc.y +    templateImage.rows/2);
            }
            cv::floodFill(result, maxloc, cv::Scalar(0), 0, cv::Scalar(.1), cv::Scalar(1.));

        }
        else
            break;
    }

    return retVal;
}
