#include "myvideocapture.h"
#include <QDebug>

MyVideoCapture::MyVideoCapture(QObject *parent)
    : QThread { parent }
    , mVideoCap { ID_CAMERA }
{
}

void MyVideoCapture::run()
{
    if (mVideoCap.isOpened())
    {
        while (true)
        {
            mVideoCap >> mFrame;
            if (!mFrame.empty())
            {
                cv::rectangle(mFrame, cv::Point(10, 10), cv::Point(401, 401), cv::Scalar(0, 0, 255), 1);

                mPixmap = cvMatToQPixmap(mFrame);
                emit newPixmapCaptured();
            }
        }
    }
}

QImage MyVideoCapture::cvMatToQImage(const cv::Mat &inMat)
{
    switch (inMat.type())
    {
        // 8-bit, 4 channel
        case CV_8UC4:
        {
            QImage image(inMat.data, inMat.cols, inMat.rows, static_cast<int>(inMat.step), QImage::Format_ARGB32);
            return image;
        }
        // 8-bit, 3 channel
        case CV_8UC3:
        {
            QImage image(inMat.data, inMat.cols, inMat.rows, static_cast<int>(inMat.step), QImage::Format_RGB888);
            return image.rgbSwapped();
        }
        // 8-bit, 1 channel
        case CV_8UC1:
        {
    #if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
            QImage image(inMat.data, inMat.cols, inMat.rows, static_cast<int>(inMat.step), QImage::Format_Grayscale8);
    #else
            static QVector<QRgb>  sColorTable;

            // only create our color table the first time
            if (sColorTable.isEmpty())
            {
                sColorTable.resize(256 );

                for (int i = 0; i < 256; ++i )
                {
                    sColorTable[i] = qRgb(i, i, i );
                }
            }

            QImage image(inMat.data,
                         inMat.cols, inMat.rows,
                         static_cast<int>(inMat.step),
                         QImage::Format_Indexed8 );

            image.setColorTable(sColorTable );
    #endif
            return image;
        }
        default:
        {
            qWarning()<< "ASM::cvMatToQImage()- cv::Mat image type not handled in switch:" << inMat.type();
            break;
        }
    }
    return QImage();
}

QPixmap MyVideoCapture::cvMatToQPixmap(const cv::Mat &inMat)
{
    return QPixmap::fromImage(cvMatToQImage(inMat));
}
