// read_circular_test_pattern.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>
#include<fstream>

// structs ////////////////////////////////////////////////////////////////////////////////////////
struct Line {
    cv::Point point1;
    cv::Point point2;
};

// global constants ///////////////////////////////////////////////////////////////////////////////
const int NUM_CIRCLES = 50;

const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);
const cv::Scalar SCALAR_BLUE = cv::Scalar(255.0, 0.0, 0.0);

// function prototypes ////////////////////////////////////////////////////////////////////////////
cv::Mat cropWhiteSpace(cv::Mat imgOriginal);
cv::Point findCenterPoint(cv::Mat imgOriginal);
std::vector<Line> findLines(cv::Mat imgOriginal);
cv::Point findCenterPointGivenLines(std::vector<Line> lines, cv::Mat imgOriginal);
cv::Point findIntersection(Line line1, Line line2);
int findNumOfLinePairs(cv::Mat imgOriginal, std::vector<int> circlePixValues, float radius);
float calculateStdDev(std::vector<int> circlePixValues);

///////////////////////////////////////////////////////////////////////////////////////////////////
int main() {

    cv::Mat imgOriginal;

    imgOriginal = cv::imread("circular_test_pattern_image.png", CV_LOAD_IMAGE_GRAYSCALE);       // read in image as grayscale

    if (imgOriginal.empty()) {                                  // if unable to open image
        std::cout << "error: image not read from file\n\n";     // show error message on command line
        return(0);                                              // and exit program
    }

    imgOriginal = cropWhiteSpace(imgOriginal);

    cv::imshow("imgOriginal", imgOriginal);             // show original image for reference
    cv::imwrite("imgOriginal.png", imgOriginal);

    cv::Point centerPoint = findCenterPoint(imgOriginal);           // get the center point of the test pattern

    cv::Mat imgCircle(imgOriginal.size(), CV_8UC3, SCALAR_BLACK);           // declare an image to draw a perfect circle on

    std::vector<int> radii;
    int circleIncrementDistance = centerPoint.y / NUM_CIRCLES;
    int currentRadius = 0;

                                            // find the number of line pairs
    int radius1 = centerPoint.y / 2;
    cv::circle(imgCircle, centerPoint, radius1, SCALAR_WHITE, 1);         // draw a perfect circle
    cv::cvtColor(imgCircle, imgCircle, CV_BGR2GRAY);                    // convert image to grayscale, this is necessary for call to findContours()
    std::vector<std::vector<cv::Point> > perfectCircleContours1;        // declare a vector of contours, this will only hold a single contour, the perfect circle
    cv::Mat imgCircleCopy1 = imgCircle.clone();                          // clone the perfect circle image before passing into findContours() since findContours() will change it
    cv::findContours(imgCircleCopy1, perfectCircleContours1, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);       // find the contour for the perfectcircle
    std::vector<cv::Point> perfectCircleContour1 = perfectCircleContours1[0];     // assign the perfect circle contour to a single contour
    std::vector<int> circlePixValues1;                       // declare a vector to hold the pixel values going around the circle
    for (int i = 0; i < perfectCircleContour1.size() - 1; i++) {                 // for each point in the perfect circle contour
        cv::Point point(perfectCircleContour1[i].x, perfectCircleContour1[i].y);      // get the coordinates into point format
        circlePixValues1.push_back(imgOriginal.at<uchar>(point));                               // add grayscale value at that point to our vector
    }
    int numOfLinePairs = findNumOfLinePairs(imgOriginal, circlePixValues1, radius1);

    for (int i = 0; i < NUM_CIRCLES; i++) {
        currentRadius = currentRadius + circleIncrementDistance;
        radii.push_back(currentRadius);
    }

    std::reverse(radii.begin(), radii.end());           // change the radii to largest to smallest order, more intuitive to start with the outer circle

    std::vector<float> linePairDistancesInPixels;
    std::vector<float> standardDeviations;

    for (auto &radius : radii) {

        imgCircle = cv::Mat(imgOriginal.size(), CV_8UC3, SCALAR_BLACK);

        cv::circle(imgCircle, centerPoint, radius, SCALAR_WHITE, 1);         // draw a perfect circle
        
        //cv::imshow("imgCircle", imgCircle);                                 // show the perfect circle for reference
        
        cv::cvtColor(imgCircle, imgCircle, CV_BGR2GRAY);                    // convert image to grayscale, this is necessary for call to findContours()
        
        std::vector<std::vector<cv::Point> > perfectCircleContours;         // declare a vector of contours, this will only hold a single contour, the perfect circle
        
        cv::Mat imgCircleCopy = imgCircle.clone();                          // clone the perfect circle image before passing into findContours() since findContours() will change it
        
        cv::findContours(imgCircleCopy, perfectCircleContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);       // find the contour for the perfectcircle
        
        cv::Mat imgCircleContour(imgCircle.size(), CV_8UC3, SCALAR_BLACK);
        
        cv::drawContours(imgCircleContour, perfectCircleContours, -1, cv::Scalar(255.0, 255.0, 255.0), 1);
        
        //cv::imshow("imgCircleContour", imgCircleContour);                   // show the perfect circle contour for reference
        
        std::vector<cv::Point> perfectCircleContour = perfectCircleContours[0];     // assign the perfect circle contour to a single contour

        std::vector<int> circlePixValues;                       // declare a vector to hold the pixel values going around the circle

        for (int i = 0; i < perfectCircleContour.size() - 1; i++) {                 // for each point in the perfect circle contour

            cv::Point point(perfectCircleContour[i].x, perfectCircleContour[i].y);      // get the coordinates into point format

            int grayValue;                                                          // declare a variable to store the gray value

            grayValue = imgOriginal.at<uchar>(point);                           // get the grayscale value at that point

            circlePixValues.push_back(grayValue);                               // add grayscale value at that point to our vector
        }

        float circumference = 2.0 * CV_PI * (float)radius;

        float linePairDistInPix = circumference / (float)numOfLinePairs;

        linePairDistancesInPixels.push_back(linePairDistInPix);

        float stdDev = calculateStdDev(circlePixValues);

        standardDeviations.push_back(stdDev);
    }

    std::cout << "numOfLinePairs = " << numOfLinePairs << "\n\n";

    int circleNumber = radii.size();

    std::cout << "circle number   radius in pix   LP dist in pix   1 / LP dist in pix   std dev\n";
    std::cout << "-------------   -------------   --------------   ------------------   -------\n";

    for (int i = 0; i < radii.size(); i++) {
        std::cout << "     " << circleNumber << "           " << radii[i] << "             " << linePairDistancesInPixels[i] << "           " << 1 / linePairDistancesInPixels[i] << "    " << standardDeviations[i] << "\n";
        circleNumber--;
    }

    std::ofstream outputFile;

    outputFile.open("data.txt");

    outputFile << "circle number,radius in pix,LP dist in pix,1 / LP dist in pix,std dev\n";

    for (int i = 0; i < radii.size(); i++) {
        outputFile << circleNumber << "," << radii[i] << "," << linePairDistancesInPixels[i] << "," << 1 / linePairDistancesInPixels[i] << "," << standardDeviations[i] << "\n";
    }

    outputFile.close();

    std::cout << "\n\n";

    cv::waitKey(0);                 // hold windows open until user presses a key

    return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
cv::Mat cropWhiteSpace(cv::Mat imgOriginal) {

    cv::Mat imgROI;
    cv::Mat imgCropped;
    cv::Mat imgThresh;

    cv::threshold(imgOriginal, imgThresh, 128.0, 255.0, CV_THRESH_BINARY_INV);

    cv::Mat structuringElement5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));

    cv::erode(imgThresh, imgThresh, structuringElement5x5);
    cv::dilate(imgThresh, imgThresh, structuringElement5x5);

    cv::threshold(imgThresh, imgThresh, 128.0, 255.0, CV_THRESH_BINARY);

    std::vector<std::vector<cv::Point> > contours;

    cv::findContours(imgThresh.clone(), contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

    int minX = 9999999;
    int maxX = 0;
    int minY = 9999999;
    int maxY = 0;

    for (auto &contour : contours) {
        cv::Rect rect = cv::boundingRect(contour);
        if (rect.x < minX) minX = rect.x;
        if ((rect.x + rect.width) > maxX) maxX = rect.x + rect.width;
        if (rect.y < minY) minY = rect.y;
        if ((rect.y + rect.height) > maxY) maxY = rect.y + rect.height;
    }

    cv::Rect croppingRect(minX, minY, maxX - minX, maxY - minY);

    imgROI = imgOriginal(croppingRect);

    imgROI.copyTo(imgCropped);

    return(imgCropped);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
cv::Point findCenterPoint(cv::Mat imgOriginal) {

    std::vector<Line> lines = findLines(imgOriginal);

    cv::Point centerPoint = findCenterPointGivenLines(lines, imgOriginal);

    return(centerPoint);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<Line> findLines(cv::Mat imgOriginal) {

    std::vector<Line> lines;

    std::vector<cv::Vec4i> v4iLines;

    cv::Mat imgThresh;

    cv::threshold(imgOriginal, imgThresh, 128.0, 255.0, CV_THRESH_BINARY);

    cv::imshow("imgThresh", imgThresh);

    cv::HoughLinesP(imgThresh, v4iLines, 0.5, CV_PI / 180.0, 128, imgOriginal.rows / 2);

    for (int i = 0; i < v4iLines.size(); i++) {
        Line myLine;

        myLine.point1.x = v4iLines[i][0];
        myLine.point1.y = v4iLines[i][1];

        myLine.point2.x = v4iLines[i][2];
        myLine.point2.y = v4iLines[i][3];

        lines.push_back(myLine);
    }

    cv::Mat imgLines(imgOriginal.size(), CV_8UC3, SCALAR_BLACK);           // declare an image to draw lines on

    for (auto &line : lines) {
        cv::line(imgLines, line.point1, line.point2, SCALAR_BLUE, 2);
    }

    cv::imshow("imgLines", imgLines);

    return(lines);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
cv::Point findCenterPointGivenLines(std::vector<Line> lines, cv::Mat imgOriginal) {
    cv::Point centerPoint;

    if (lines.size() < 2) {
        std::cout << "error, findCenterPointGivenLines called without at least 2 lines";
        return centerPoint;
    }

    int linesSizeEven;

    if (lines.size() % 2 == 0) {
        linesSizeEven = (int)lines.size();
    } else {
        linesSizeEven = (int)lines.size() - 1;
    }

    std::vector<cv::Point> intersections;

    for (int i = 0; i < linesSizeEven - 1; i = i + 2) {

        cv::Point intersectionPoint = findIntersection(lines[i], lines[i + 1]);

        if (intersectionPoint.x >= 0 && intersectionPoint.x < imgOriginal.cols &&
            intersectionPoint.y >= 0 && intersectionPoint.y < imgOriginal.rows ) {
            intersections.push_back(intersectionPoint);
        }
    }

    long xTotal = 0;
    long yTotal = 0;

    for (auto &intersection : intersections) {
        xTotal = xTotal + intersection.x;
        yTotal = yTotal + intersection.y;
    }

    centerPoint.x = xTotal / (int)intersections.size();
    centerPoint.y = yTotal / (int)intersections.size();

    return(centerPoint);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
cv::Point findIntersection(Line line1, Line line2) {
    cv::Point2f fltIntersection;
    cv::Point intIntersection;

    float s1_x, s1_y, s2_x, s2_y;

    s1_x = line1.point2.x - line1.point1.x;
    s1_y = line1.point2.y - line1.point1.y;
    s2_x = line2.point2.x - line2.point1.x;
    s2_y = line2.point2.y - line2.point1.y;

    float s, t;
    s = (-s1_y * (line1.point1.x - line2.point1.x) + s1_x * (line1.point1.y - line2.point1.y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = (s2_x * (line1.point1.y - line2.point1.y) - s2_y * (line1.point1.x - line2.point1.x)) / (-s2_x * s1_y + s1_x * s2_y);

    fltIntersection.x = line1.point1.x + (t * s1_x);
    fltIntersection.y = line1.point1.y + (t * s1_y);

    intIntersection.x = std::round(fltIntersection.x);
    intIntersection.y = std::round(fltIntersection.y);

    return(intIntersection);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int findNumOfLinePairs(cv::Mat imgOriginal, std::vector<int> circlePixValues, float radius) {

    int numberOfLinePairs = 0;
    int numOfTransitions = 0;

    const int MIN_CONSEC_TO_ASSURE_CHANGE = 3;

    bool previouslyOnWhite;
    bool previouslyOnBlack;
    bool waitingForWhite = false;
    bool waitingForBlack = false;

    if (circlePixValues[circlePixValues.size() - 1] > 128) {    // if last value in contour is white
        previouslyOnWhite = true;                   // then set previously on white
        previouslyOnBlack = false;
    } else {                                                    // else if the last value in the contour is black
        previouslyOnBlack = true;                   // then set previously on black
        previouslyOnWhite = false;
    }

    int consecutiveWhitePix = 0;
    int consecutiveBlackPix = 0;

    for (auto &circlePixValue : circlePixValues) {
        if (waitingForWhite && circlePixValue <= 128) {             // if we're waiting for white but still on black
            continue;
        } else if (waitingForBlack && circlePixValue > 128) {       // if we're waiting for black but still on white
            continue;
        } else if (previouslyOnWhite && circlePixValue > 128) {         // if we were previously on white and we're still on white
            consecutiveWhitePix++;
            if (consecutiveWhitePix >= MIN_CONSEC_TO_ASSURE_CHANGE) {
                numOfTransitions++;
                waitingForBlack = true;
                waitingForWhite = false;
            }
        } else if (previouslyOnBlack && circlePixValue <= 128) {        // if we were previously on black and we're still on black
            consecutiveBlackPix++;
            if (consecutiveBlackPix >= MIN_CONSEC_TO_ASSURE_CHANGE) {
                numOfTransitions++;
                waitingForWhite = true;
                waitingForBlack = false;
            }
        } else if (previouslyOnWhite && circlePixValue <= 128) {        // if we were previously on white and now we're on black
            previouslyOnBlack = true;
            previouslyOnWhite = false;
            consecutiveBlackPix = 1;
            consecutiveWhitePix = 0;
            waitingForWhite = false;
            waitingForBlack = false;
        } else if (previouslyOnBlack && circlePixValue > 128) {        // if we were previously on black and now we're on white
            previouslyOnWhite = true;
            previouslyOnBlack = false;
            consecutiveWhitePix = 1;
            consecutiveBlackPix = 0;
            waitingForWhite = false;
            waitingForBlack = false;
        } else {
            // should never get here
        }
    }

    if (numOfTransitions % 2) {
        numOfTransitions++;
    }

    numberOfLinePairs = numOfTransitions / 2;

    return(numberOfLinePairs);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
float calculateStdDev(std::vector<int> circlePixValues) {

    double sum_mean = 0.0;
    float mean = 0.0;
    double sum_std_dev = 0.0;
    float std_dev = 0.0;

    for (auto &circlePixValue : circlePixValues) {
        sum_mean = sum_mean + circlePixValue;
    }

    mean = (float)(sum_mean / circlePixValues.size());

    for (auto &circlePixValue : circlePixValues) {
        sum_std_dev = sum_std_dev + ((abs(circlePixValue - mean)) * (abs(circlePixValue - mean)));
    }

    std_dev = (float)sqrt(sum_std_dev / circlePixValues.size());

    return(std_dev);
}




