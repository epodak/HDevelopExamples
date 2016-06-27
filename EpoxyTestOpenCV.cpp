// EpoxyTestOpenCV.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>

// structs ////////////////////////////////////////////////////////////////////////////////////////
struct PointPair {
    cv::Point2f innerPoint;
    cv::Point2f outerPoint;
    float distanceBetween;
};

// global constants ///////////////////////////////////////////////////////////////////////////////
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);

// function prototypes ////////////////////////////////////////////////////////////////////////////
std::vector<std::vector<cv::Point> > removeSmallContours(std::vector<std::vector<cv::Point> > contours);
std::vector<std::vector<cv::Point> > removeContoursThatAreNotCircles(std::vector<std::vector<cv::Point> > contours);
cv::Point2f findCenterOfCircles(cv::Mat imgContours);
std::vector<PointPair> findVectorOfPointPairs(std::vector<cv::Point> outerContour, std::vector<cv::Point> innerContour, cv::Point2f centerOfCircles);
cv::Point2f step(cv::Point2f currentPoint, double currentDegree);
float distanceBetweenPoints(cv::Point2f firstPoint, cv::Point2f secondPoint);

///////////////////////////////////////////////////////////////////////////////////////////////////
int main() {

    cv::Mat imgOriginal;        // declare various images
    cv::Mat imgGrayscale;
    cv::Mat imgThresh;
    cv::Mat imgThreshCopy;

    imgOriginal = cv::imread("pass1.png");              // open image

    if (imgOriginal.empty()) {                                  // if unable to open image
        std::cout << "error: image not read from file\n\n";     // show error message on command line
        return(0);                                              // and exit program
    }

    cv::resize(imgOriginal, imgOriginal, cv::Size(), 0.45, 0.45);       // resize image so it fits on the screen (for convenience)

    cv::imshow("imgOriginal", imgOriginal);                             // show original image for reference

    cv::cvtColor(imgOriginal, imgGrayscale, CV_BGR2GRAY);               // convert to grayscale (threshold function does not take color images)

    cv::imshow("imgGrayscale", imgGrayscale);                           // show grayscale image for reference

    cv::threshold(imgGrayscale, imgThresh, 0.0, 255.0, CV_THRESH_BINARY | CV_THRESH_OTSU);          // Otsu threshold

    cv::imshow("imgThresh", imgThresh);                 // show thresh image for reference

    imgThreshCopy = imgThresh.clone();                  // clone thresh image b/c findContours() function modifies any image passed in

    std::vector<std::vector<cv::Point> > contours;              // declare our vector of contours

    cv::findContours(imgThreshCopy, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);         // find contours

    cv::Mat imgContours(imgOriginal.size(), CV_8UC3, SCALAR_BLACK);                 // declare an image to draw contours on for reference
    cv::drawContours(imgContours, contours, -1, cv::Scalar(255.0, 255.0, 255.0));       // draw contours so we can show for reference
    cv::imshow("imgContours", imgContours);                                         // show contours image for reference

    contours = removeSmallContours(contours);                                       // remove small contours
    
    imgContours = cv::Mat(imgOriginal.size(), CV_8UC3, SCALAR_BLACK);
    cv::drawContours(imgContours, contours, -1, cv::Scalar(255.0, 255.0, 255.0));       // draw contours so we can show for reference
    cv::imshow("imgContours with small contours removed", imgContours);             // show contours image for reference

    contours = removeContoursThatAreNotCircles(contours);                           // remove contours that are not circles

    imgContours = cv::Mat(imgOriginal.size(), CV_8UC3, SCALAR_BLACK);
    cv::drawContours(imgContours, contours, -1, cv::Scalar(255.0, 255.0, 255.0));       // draw contours so we can show for reference
    cv::imshow("imgContours with contours that are not circles removed", imgContours);      // show contours image for reference

    cv::Point2f centerOfCircles = findCenterOfCircles(imgContours);                 // find the center point of the circles

    std::vector<cv::Point> outerContour;                                // declare an outer contour
    std::vector<cv::Point> innerContour;                                // and an inner contour

    if (contours.size() != 2) {                                         // check to make sure we have two contours
        std::cout << "error, number of contours != 2";
        return(0);
    } else if (cv::contourArea(contours[0]) > cv::contourArea(contours[1])) {           // if contour 0 is bigger . . .
        outerContour = contours[0];                                                     // assign contour 0 to bigger
        innerContour = contours[1];                                                     // and contour 1 to smaller
    } else {                                                                            // else
        innerContour = contours[0];                                                     // the other way around
        outerContour = contours[1];
    }

    std::vector<PointPair> pointPairs = findVectorOfPointPairs(outerContour, innerContour, centerOfCircles);    // find our vector of point pairs around the circles

    cv::Mat imgPointPairs(imgOriginal.size(), CV_8UC3, SCALAR_BLACK);           // declare an image to draw the point pairs on

    for (auto &pointPair : pointPairs) {                                            // for each point pair
        cv::circle(imgPointPairs, pointPair.innerPoint, 1, SCALAR_GREEN, -1);       // draw a small green dot at the inner point
        cv::circle(imgPointPairs, pointPair.outerPoint, 1, SCALAR_GREEN, -1);       // and at the outer point
    }

    cv::imshow("imgPointPairs", imgPointPairs);                 // show the point pairs image for reference

    float minDistanceBetween = 99999999.9;              // declare variables for min, max, and average distance between point pairs
    float maxDistanceBetween = 0.0;
    double averageDistanceBetween = 0.0;

    for (auto &pointPair : pointPairs) {                                                                            // for each point pair
        if (pointPair.distanceBetween < minDistanceBetween) minDistanceBetween = pointPair.distanceBetween;         // update if we have a new min
        if (pointPair.distanceBetween > maxDistanceBetween) maxDistanceBetween = pointPair.distanceBetween; // update if we have a new max

        averageDistanceBetween = averageDistanceBetween + pointPair.distanceBetween;                // add distance between to total
    }

    averageDistanceBetween = averageDistanceBetween / pointPairs.size();                        // compute average distance between point pairs

    std::cout << "\n\n";
    std::cout << "minDistanceBetween = " << minDistanceBetween << " \n\n";                      // show the data we just computed
    std::cout << "maxDistanceBetween = " << maxDistanceBetween << " \n\n";
    std::cout << "difference between min and max distance between = " << maxDistanceBetween - minDistanceBetween << " \n\n";
    std::cout << "averageDistanceBetween = " << averageDistanceBetween << " \n\n";

    cv::waitKey(0);                 // hold windows open until user presses a key

    return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::vector<cv::Point> > removeSmallContours(std::vector<std::vector<cv::Point> > contours) {

    std::vector<std::vector<cv::Point> > largeEnoughContours;           // this will be the return value

    for (auto &contour : contours) {
        if (cv::contourArea(contour) > 100) {
            largeEnoughContours.push_back(contour);
        }
    }

    return(largeEnoughContours);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::vector<cv::Point> > removeContoursThatAreNotCircles(std::vector<std::vector<cv::Point> > contours) {

    std::vector<std::vector<cv::Point> > circleShapedContours;          // this will be the return value

    cv::Mat imgCircle(cv::Size(500, 500), CV_8UC3, SCALAR_BLACK);           // declare an image to draw a perfect circle on

    cv::circle(imgCircle, cv::Point(200, 200), 100, cv::Scalar(255.0, 255.0, 255.0), -1);       // draw a perfect circle

    //cv::imshow("imgCircle", imgCircle);                                 // show the perfect circle for reference

    cv::cvtColor(imgCircle, imgCircle, CV_BGR2GRAY);                    // convert image to grayscale, this is necessary for call to findContours()

    std::vector<std::vector<cv::Point> > perfectCircleContours;         // declare a vector of contours, this will only hold a single contour, the perfect circle

    cv::Mat imgCircleCopy = imgCircle.clone();                          // clone the perfect circle image before passing into findContours() since findContours() will change it

    cv::findContours(imgCircleCopy, perfectCircleContours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);       // find the contour for the perfectcircle

    cv::Mat imgCircleContour(imgCircle.size(), CV_8UC3, SCALAR_BLACK);      

    cv::drawContours(imgCircleContour, perfectCircleContours, -1, cv::Scalar(255.0, 255.0, 255.0), 1);

    //cv::imshow("imgCircleContour", imgCircleContour);                   // show the perfect circle contour for reference

    std::vector<cv::Point> perfectCircleContour = perfectCircleContours[0];     // assign the perfect circle contour to a single contour

    for (auto &contour : contours) {                // for each contour

        if (cv::matchShapes(contour, perfectCircleContour, CV_CONTOURS_MATCH_I1, 0.0) < 0.01) {     // if it matches the perfect circle in shape . . .
            circleShapedContours.push_back(contour);                                                // add to our vector of circle shaped contours
        }

    }

    return(circleShapedContours);                   // return only the circle shaped contours
}

///////////////////////////////////////////////////////////////////////////////////////////////////
cv::Point2f findCenterOfCircles(cv::Mat imgContours) {
	
    cv::Point2f centerOfCircles;                // this will be the return value

    std::vector<cv::Vec3f> circles;

    cv::cvtColor(imgContours, imgContours, CV_BGR2GRAY);

    cv::HoughCircles(imgContours, circles, CV_HOUGH_GRADIENT, 2.0, imgContours.cols / 4);

    cv::Mat imgCircles(imgContours.size(), CV_8UC3, SCALAR_BLACK);
	
    for (auto &circle : circles) {
        cv::circle(imgCircles, cv::Point((int)circle[0], (int)circle[1]), (int)circle[2], cv::Scalar(SCALAR_GREEN), 2);
    }

    double averageX = 0.0;
    double averageY = 0.0;

    for (auto &circle : circles) {
        averageX = averageX + circle[0];
        averageY = averageY + circle[1];
    }

    averageX = averageX / (double)circles.size();
    averageY = averageY / (double)circles.size();

    centerOfCircles.x = (float)averageX;
    centerOfCircles.y = (float)averageY;

    return(centerOfCircles);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<PointPair> findVectorOfPointPairs(std::vector<cv::Point> outerContour, std::vector<cv::Point> innerContour, cv::Point2f centerOfCircles) {
	
    std::vector<PointPair> pointPairs;          // this will be the return value

    cv::Point2f currentPoint;

    for (double currentDegree = 0.0; currentDegree < 360.0; currentDegree++) {          // for each degree all the way around the circle . . .

        currentPoint.x = centerOfCircles.x;                 // start by re-assigning the current point to the center of the circles
        currentPoint.y = centerOfCircles.y;
                                                                                // first we step outward until we reach the edge of the inner circle
        while (cv::pointPolygonTest(innerContour, currentPoint, false) > 0) {       // while we are still inside the inner circle
            currentPoint = step(currentPoint, currentDegree);                       // step out one
        }
                                    // when we get here we have reached the outside of the inner circle
        PointPair pointPair;                        // declare a point pair

        pointPair.innerPoint.x = currentPoint.x;        // update the inner circle x and y points
        pointPair.innerPoint.y = currentPoint.y;
                                                                                // next we step outward until we reach the edge of the outer circle
        while (cv::pointPolygonTest(outerContour, currentPoint, false) > 0) {       // while we are still inside the outer circle
            currentPoint = step(currentPoint, currentDegree);                       // step out one
        }
                                    // when we get here we have reached the outside of the outer circle
        pointPair.outerPoint.x = currentPoint.x;        // update the outer circle x and y points
        pointPair.outerPoint.y = currentPoint.y;
                                                    // now we have both the inner circle point and the outer circle point
        pointPairs.push_back(pointPair);            // add the current point pair to our vector of point pairs
    }

    for (auto &pointPair : pointPairs) {                                                                    // for each point pair
        pointPair.distanceBetween = distanceBetweenPoints(pointPair.innerPoint, pointPair.outerPoint);      // get the distance between points
    }

    return(pointPairs);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// starting from currentPoint, step a size of one pixel in the direction of currentDegree
cv::Point2f step(cv::Point2f currentPoint, double currentDegree) {

    float deltaX = std::sin(currentDegree * (3.14159 / 180.0));
    float deltaY = std::cos(currentDegree * (3.14159 / 180.0));

    currentPoint.x = currentPoint.x + deltaX;
    currentPoint.y = currentPoint.y + deltaY;

    return(currentPoint);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// compute the distance between two floating point points using the pythagorean theorem
float distanceBetweenPoints(cv::Point2f firstPoint, cv::Point2f secondPoint) {
    float distanceBetween = 0.0;

    float deltaX = std::abs(firstPoint.x - secondPoint.x);
    float deltaY = std::abs(firstPoint.y - secondPoint.y);

    distanceBetween = sqrt(pow(deltaX, 2) + pow(deltaY, 2));

    return(distanceBetween);
}



