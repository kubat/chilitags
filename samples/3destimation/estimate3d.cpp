#include <iostream>

#include <chilitags.hpp>

#include <opencv2/core/core.hpp> // for cv::Mat
#include <opencv2/highgui/highgui.hpp> // for camera capture

using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
    cout
        << "Usage: "<< argv[0]
        << " [-c <tag configuration (YAML)>] [-i <camera calibration (YAML)>]\n";

    const char* intrinsicsFilename = 0;
    const char* configFilename = 0;

    for( int i = 1; i < argc; i++ )
    {
        if( strcmp(argv[i], "-c") == 0 )
            configFilename = argv[++i];
        else if( strcmp(argv[i], "-i") == 0 )
            intrinsicsFilename = argv[++i];
    }

    /*****************************/
    /*    Init camera capture    */
    /*****************************/
    int cameraIndex = 0;
    cv::VideoCapture capture(cameraIndex);
    if (!capture.isOpened())
    {
        cerr << "Unable to initialise video capture.\n";
        return 1;
    }

    /******************************/
    /* Setting up pose estimation */
    /******************************/
#ifdef OPENCV3
    double inputWidth  = capture.get(cv::CAP_PROP_FRAME_WIDTH);
    double inputHeight = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
#else
    double inputWidth  = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    double inputHeight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
#endif

    chilitags::Chilitags3D chilitags3D(Size(inputWidth, inputHeight));

    if (configFilename) chilitags3D.readTagConfiguration(configFilename);

    if (intrinsicsFilename) {
        Size calibratedImageSize = chilitags3D.readCalibration(intrinsicsFilename);
#ifdef OPENCV3
        capture.set(cv::CAP_PROP_FRAME_WIDTH, calibratedImageSize.width);
        capture.set(cv::CAP_PROP_FRAME_HEIGHT, calibratedImageSize.height);
#else
        capture.set(CV_CAP_PROP_FRAME_WIDTH, calibratedImageSize.width);
        capture.set(CV_CAP_PROP_FRAME_HEIGHT, calibratedImageSize.height);
#endif
    }

    /*****************************/
    /*             Go!           */
    /*****************************/
    cout << "I'm now looking for objects...\n";
    cv::Mat inputImage;

    for (; 'q' != (char) cv::waitKey(10); ) {
        capture.read(inputImage);

        for (auto& kv : chilitags3D.estimate(inputImage)) {
            cout << kv.first << " at " << Mat(kv.second) << "\n";
        }
    }

    capture.release();

    return 0;
}

