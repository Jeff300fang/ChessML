#include "VisualCapture.h"

VisualCapture::VisualCapture() {
    hwnd = GetDesktopWindow();
    pieceMap[0] = 'P'; //white pawn
    pieceMap[1] = 'B'; //white bishop
    pieceMap[2] = 'N'; //white knight
    pieceMap[3] = 'R'; //white rook
    pieceMap[4] = 'Q'; //white queen
    pieceMap[5] = 'K'; //white king
    pieceMap[6] = 'p'; //black pawn
    pieceMap[7] = 'b'; //black bishop
    pieceMap[8] = 'n'; //black knight
    pieceMap[9] = 'r'; //black rook
    pieceMap[10] = 'q'; //black queen
    pieceMap[11] = 'k'; //black king
    knn = cv::ml::KNearest::create();
    knn = cv::Algorithm::load<cv::ml::KNearest>("trained_knn_model.yml");
    std::cout << "Visual Capture Done" << std::endl;
}
cv::Mat VisualCapture::hwnd2mat() {
    HDC hwindowDC, hwindowCompatibleDC;

    int height, width, srcheight, srcwidth;
    HBITMAP hbwindow;
    cv::Mat src;
    BITMAPINFOHEADER  bi;

    hwindowDC = GetDC(hwnd);
    hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

    RECT windowsize;    // get the height and width of the screen
    GetClientRect(hwnd, &windowsize);

    srcheight = windowsize.bottom;
    srcwidth = windowsize.right;
    height = windowsize.bottom / 1;  //change this to whatever size you want to resize to
    width = windowsize.right / 1;

    src.create(height, width, CV_8UC4);

    // create a bitmap
    hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height;  //this is the line that makes it draw upside down or not
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    // use the previously created device context with the bitmap
    SelectObject(hwindowCompatibleDC, hbwindow);
    // copy from the window device context to the bitmap device context
    StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
    GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

    // avoid memory leak
    DeleteObject(hbwindow);
    DeleteDC(hwindowCompatibleDC);
    ReleaseDC(hwnd, hwindowDC);

    return src;
}
cv::Mat VisualCapture::getScreen () {
    //from: https://stackoverflow.com/questions/14148758/how-to-capture-the-desktop-in-opencv-ie-turn-a-bitmap-into-a-mat
    return(hwnd2mat());
}
cv::Mat VisualCapture::getBoard() {
    //get current screen
    cv::Mat currentScreen;
    currentScreen = getScreen();
    
    //mask the board
    cv::Mat bgr, masked, hsv;
    cv::cvtColor(currentScreen, bgr, cv::COLOR_BGRA2BGR);
    cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);
    //cv::Scalar lowerBound(0, 0, 0);
    //cv::Scalar upperBound(30, 180, 180);

    //mask for board color
    cv::Scalar lowerBound(40, 100, 0);
    cv::Scalar upperBound(50, 120, 180);
    cv::inRange(hsv, lowerBound, upperBound, masked);
    
    //find contours where color matches
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(masked, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<std::vector<cv::Point>> filteredContours;
    for (auto contour : contours) {
        if (cv::contourArea(contour) > 100) {
            filteredContours.push_back(contour);
        }
    }

    //cv::drawContours(masked, filteredContours, -1, cv::Scalar(255, 255, 255), -1);
    //find the biggest bounding rect --> has the board
    cv::Rect maxBoundingRect = cv::boundingRect(contours[0]);
    for (auto contour : filteredContours) {
        cv::Rect boundingRect = cv::boundingRect(contour);
        if (boundingRect.area() > maxBoundingRect.area()) {
            maxBoundingRect = boundingRect;
        }
    }

    //return cropped board
    //cv::rectangle(currentScreen, maxBoundingRect, cv::Scalar(0, 0, 255), 2);
    cv::Mat croppedBoard = currentScreen(maxBoundingRect);
    return(croppedBoard);
}
cv::Rect VisualCapture::getBoardCoordinates() {
    //get current screen
    cv::Mat currentScreen;
    currentScreen = getScreen();

    //mask the board
    cv::Mat bgr, masked, hsv;
    cv::cvtColor(currentScreen, bgr, cv::COLOR_BGRA2BGR);
    cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);
    //cv::Scalar lowerBound(0, 0, 0);
    //cv::Scalar upperBound(30, 180, 180);

    //mask for board color
    cv::Scalar lowerBound(40, 100, 0);
    cv::Scalar upperBound(50, 120, 180);
    cv::inRange(hsv, lowerBound, upperBound, masked);

    //find contours where color matches
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(masked, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<std::vector<cv::Point>> filteredContours;
    for (auto contour : contours) {
        if (cv::contourArea(contour) > 100) {
            filteredContours.push_back(contour);
        }
    }

    //cv::drawContours(masked, filteredContours, -1, cv::Scalar(255, 255, 255), -1);
    //find the biggest bounding rect --> has the board
    cv::Rect maxBoundingRect = cv::boundingRect(contours[0]);
    for (auto contour : filteredContours) {
        cv::Rect boundingRect = cv::boundingRect(contour);
        if (boundingRect.area() > maxBoundingRect.area()) {
            maxBoundingRect = boundingRect;
        }
    }
    return(maxBoundingRect);
}
std::vector<std::pair<cv::Mat, cv::Rect>> VisualCapture::getPieceImages() {
    cv::Mat board = getBoard();
    //mask the board
    cv::Mat bgr, masked, hsv;
    cv::cvtColor(board, bgr, cv::COLOR_BGRA2BGR);
    cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);

    //mask for pieces
    cv::Scalar lowerBound(0, 0, 0);
    cv::Scalar upperBound(30, 180, 180);
    cv::inRange(hsv, lowerBound, upperBound, masked);

    //find pieces using contours
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    //cv::dilate(masked, masked, kernel); //CHANGED HERE
    cv::findContours(masked, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    //cv::drawContours(masked, contours, -1, cv::Scalar(255, 255, 255), 1);
    //cv::imshow("resize", masked);
    //while (cv::waitKey(1) != 'q');

    //isolate pieces
    std::vector<std::pair<cv::Mat, cv::Rect>> imagePieces;
    for (auto contour : contours) {
        cv::Rect boundingRect = cv::boundingRect(contour);
        double boundingRectArea = boundingRect.area();
        cv::Size boardSize = board.size();
        double pixelSize = boardSize.area();
        double pieceRatio = boundingRectArea / pixelSize;
        //if very little, not a piece
        if (pieceRatio < 0.0001) {
            continue;
        }

        //get slice of image
        cv::Mat croppedPiece = masked(boundingRect);
        cv::Mat croppedColor = board(boundingRect);

        //find contours to slice out piece of original image that contains piece to preserve the color
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(croppedPiece, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        cv::Mat canvas = cv::Mat::zeros(croppedPiece.size(), CV_8UC4);
        for (auto contour : contours) {
            cv::drawContours(canvas, std::vector<std::vector<cv::Point>>{contour}, -1,cv::Scalar(255, 255, 255), cv::FILLED);
        }
        cv::Mat colorPiece;
        cv::bitwise_and(canvas, croppedColor, colorPiece);

        //cv::imshow("masked", colorPiece);
        //while (cv::waitKey(1) != 'q');

        //scale image
        cv::Mat resizedImage;
        cv::Size originalSize = colorPiece.size();
        int originalHeight = originalSize.height;
        int originalWidth = originalSize.width;
        double aspectRatio = (double)originalWidth / originalHeight;
        double newWidth = 100 * aspectRatio;
        cv::resize(colorPiece, resizedImage, cv::Size((int)newWidth, 100));


        //pad image to become uniform
        int desiredDim = 100;
        cv::Mat paddedImage(desiredDim, desiredDim, resizedImage.type(), cv::Scalar(0, 0, 0));

        //find location where to put image into padded image
        int xCenter = (paddedImage.cols - resizedImage.cols) / 2;
        int yCenter = (paddedImage.rows - resizedImage.rows) / 2;
        //place image
        cv::Rect roiRect(xCenter, yCenter, resizedImage.cols, resizedImage.rows);
        resizedImage.copyTo(paddedImage(roiRect));
        std::pair<cv::Mat, cv::Rect> arrayPair;
        arrayPair.first = paddedImage;
        arrayPair.second = boundingRect;
        imagePieces.push_back(arrayPair);
        
        cv::rectangle(board, boundingRect, cv::Scalar(0, 0, 255), 2);
    }
    //cv::imshow("board", board);
    //while (cv::waitKey(1) != 'L');
    return(imagePieces);
}
std::vector<VisualPiece> VisualCapture::getPieces() {
    std::vector<VisualPiece> pieces;
    std::vector<std::pair<cv::Mat, cv::Rect>> imagePieces = getPieceImages();
    cv::Mat board = getBoard();
    //get peice name
    //std::cout << board.size().height << " " << board.size().width << std::endl;
    for (auto piecePair : imagePieces) {
        VisualPiece thePiece = predictPiece(piecePair.first);
        //get piece coordinates based on rectangle coordinates
        thePiece.colCoord = piecePair.second.x * 8 / board.size().height;
        thePiece.rowCoord = piecePair.second.y * 8 / board.size().width;
        char currentPieceType = thePiece.pieceType;
        //see what color the piece is
        if (currentPieceType > 'Z') { //black piece
            thePiece.isWhite = false;
            thePiece.pieceType = thePiece.pieceType - 'a' + 'A';
        }
        else {
            thePiece.isWhite = true;
        }
        pieces.push_back(thePiece);
        //std::cout << thePiece.rowCoord << " " << thePiece.colCoord << " " << thePiece.pieceType << std::endl;
    }
    return(pieces);
}
VisualPiece VisualCapture::predictPiece(cv::Mat image) {
    cv::Mat featureVector = image.reshape(1, 1);
    cv::Mat response;
    int k = 5;
    //convert image to feature vector
    featureVector.convertTo(featureVector, CV_32F);
    //input into knn model
    knn->findNearest(featureVector, k, response);
    //get prediction
    int predictedLabel = static_cast<int>(response.at<float>(0, 0));
    VisualPiece temp;
    temp.pieceType = pieceMap[predictedLabel];
    return(temp);
}
void VisualCapture::gatherTrainingData() {
    //open file to 
    cv::FileStorage fs("training_data.yml", cv::FileStorage::WRITE);
    fs << "TrainingData" << "[";
    for (int piece = 0; piece < 12; piece++) {
        //get image pieces
        std::vector<std::pair<cv::Mat, cv::Rect>> pieces = getPieceImages();
        bool first = true;
        int counter = 0;
        for (auto pairs : pieces) {
            cv::Mat featureVector = pairs.first.reshape(1, 1);
            if (first) {
                cv::imshow("asdas", pairs.first);
                while (cv::waitKey(1) != 'q');
                first = false;
            }
            cv::FileNode newNode;
            featureVector.convertTo(featureVector, CV_32F);
            fs << "{" << "FeatureVector" << featureVector << "Label" << piece << "}";
            counter++;
        }
        std::cout << counter << std::endl;
        while (cv::waitKey(1) != 'k');
    }
    fs << "]";
    fs.release();
}
void VisualCapture::trainModel() {
    //create model
    cv::Ptr<cv::ml::KNearest> knn = cv::ml::KNearest::create();
    cv::Mat samples;
    //gather data
    cv::FileNode trainingDataNode;
    cv::FileStorage fs("training_data.yml", cv::FileStorage::READ);
    trainingDataNode = fs["TrainingData"];

    cv::Mat trainingData;
    std::vector<int> labels;

    //get all data and combine
    for (const auto& sampleNode : trainingDataNode) {
        cv::FileNode featureNode = sampleNode["FeatureVector"];
        cv::FileNode labelNode = sampleNode["Label"];
        cv::Mat featureVector;
        featureNode >> featureVector; // Read the feature vector from the file
        int label = static_cast<int>(labelNode);

        trainingData.push_back(featureVector); // Add the feature vector to trainingData
        labels.push_back(label);
        std::cout << label << " ";
    }
    trainingData.convertTo(trainingData, CV_32F);
    //train model
    knn->train(trainingData, cv::ml::ROW_SAMPLE, labels);
    knn->save("trained_knn_model.yml");
    fs.release();
    return;
}
bool VisualCapture::computerPlayingWhite() {
    std::vector<VisualPiece> visualPieces = getPieces();
    //see if the bottom left piece is white or black, indicating the color the computer is playing
    for (VisualPiece visualPiece : visualPieces) {
        if (visualPiece.rowCoord == 7 && visualPiece.colCoord == 0) {
            return(visualPiece.isWhite);
        }
    }
    return(false);
}
std::string VisualCapture::playerMadeMove(bool computerColor) {
    //find indicators for piece movement
    cv::Mat theBoard = getBoard();
    cv::Mat bgr;
    cv::cvtColor(theBoard, bgr, cv::COLOR_BGRA2BGR);
    cv::Mat hsv;
    cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);
    cv::Mat mask;

    bool opposingColor = false;
    bool currentIsWhite = false;
    std::string starting = "";
    std::string ending = "";
    cv::Size boardSize = theBoard.size();

    //light highlighted
    //cv::inRange(hsv, cv::Scalar(30, 100, 240), cv::Scalar(40, 110, 250), mask);
    //cv::inRange(hsv, cv::Scalar(30, 150, 200), cv::Scalar(40, 160, 210), mask);

    std::vector < std::pair < cv::Scalar, cv::Scalar>> colorBounds;
    std::pair < cv::Scalar, cv::Scalar> firstPair, secondPair;
    firstPair.first = cv::Scalar(20,120, 240);
    firstPair.second = cv::Scalar(30,130, 250);
    secondPair.first = cv::Scalar(30, 160, 200);
    secondPair.second = cv::Scalar(40, 170, 210);
    colorBounds.push_back(firstPair);
    colorBounds.push_back(secondPair);

    for (const std::pair<cv::Scalar, cv::Scalar> &colorBound : colorBounds) {
        cv::inRange(hsv, colorBound.first, colorBound.second, mask);
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        cv::erode(mask, mask, kernel, cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, cv::morphologyDefaultBorderValue());
        //cv::imshow("mask", mask);
        //while (cv::waitKey(1) != 'q');
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(mask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        std::string temporaryString;
        for (auto contour : contours) {
            cv::Rect boundingRect = cv::boundingRect(contour);
            //IDEA || (float)boundingRect.height/boardSize.height * 8 < 0.8
            if (boundingRect.area() < 1000 ) {
                continue;
            }
            double percentFull, total;
            cv::Mat slice;
            slice = mask(boundingRect);
            cv::Scalar pixelSum = cv::sum(slice);
            total = pixelSum[0];
            percentFull = total / boundingRect.area();
            //if the square is blank, indicate starting location
            temporaryString = std::to_string(boundingRect.y * 8 / boardSize.width) + std::to_string(boundingRect.x * 8 / boardSize.height);
            if (percentFull > 230) {
                starting = temporaryString;
            }
            else {
                //ending location
                ending = temporaryString;
                //find what color the highlighted square is
                cv::Mat maskSlice;
                cv::Mat boardSlice;
                maskSlice = mask(boundingRect);
                boardSlice = theBoard(boundingRect);

                cv::Mat notSlice;
                cv::bitwise_not(maskSlice, notSlice);

                cv::Mat andSlice;
                cv::Mat grayBoard;
                cv::cvtColor(boardSlice, grayBoard, cv::COLOR_BGRA2GRAY);
                //std::cout << notSlice.channels() << boardSlice.channels() << std::endl;
                cv::bitwise_and(notSlice, grayBoard, andSlice);

                //cv::Scalar totalPixels = cv::sum(notSlice);
                int totalPixels = cv::countNonZero(notSlice);
                cv::Scalar totalColor = cv::sum(andSlice);
                //cv::imshow("and", andSlice);
                //cv::imshow("not", notSlice);
                //while (cv::waitKey(1) != 'q');

                double colorRatio;
                //std::cout << totalPixels[0] << " " << totalColor[0] << std::endl;
                colorRatio = totalColor[0] / (double)totalPixels;
                //std::cout << colorRatio << std::endl;
                if (colorRatio > 185) {
                    currentIsWhite = true;
                }
            }
            //cv::imshow("mask",mask);
            //while (cv::waitKey(1) != ' q');   
        }
    }

    //std::cout << starting << " " << ending << std::endl;
    if (starting == "" || ending == "") {
        return(" ");
    }
    opposingColor = (currentIsWhite == computerColor) ? false : true;
    if (opposingColor) {
        return(starting + ending);
    }
    return(" ");
} //test
void VisualCapture::adjust() {
    cv::Mat theBoard = getBoard();
    cv::Mat bgr;
    cv::cvtColor(theBoard, bgr, cv::COLOR_BGRA2BGR);
    cv::Mat hsv;
    cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);
    cv::Mat mask;

    bool opposingColor = false;
    bool currentIsWhite = false;
    std::string starting = "";
    std::string ending = "";
    cv::Size boardSize = theBoard.size();

    //light highlighted
    //cv::inRange(hsv, cv::Scalar(30, 100, 240), cv::Scalar(40, 110, 250), mask);
    //cv::inRange(hsv, cv::Scalar(30, 150, 200), cv::Scalar(40, 160, 210), mask);

    std::vector < std::pair < cv::Scalar, cv::Scalar>> colorBounds;
    std::pair < cv::Scalar, cv::Scalar> firstPair, secondPair;
    for (int i = 0; i < 25; i++) {
        firstPair.first = cv::Scalar(30,160,200);
        firstPair.second = cv::Scalar(40,170,210);
        std::cout << i * 10 << std::endl;
        cv::inRange(hsv, firstPair.first, firstPair.second, mask);
        cv::imshow("asdas", mask);
        while (cv::waitKey(1) != 'q');
    }

}
void VisualCapture::moveOnScreen(std::string move){
    cv::Rect boardQualities = getBoardCoordinates();
    int fromRow, fromCol, toRow, toCol;
    fromRow = move[0] - '0';
    fromCol = move[1] - '0';
    toRow = move[2] - '0';
    toCol = move[3] - '0';
    int targetX, targetY;
    targetX = fromCol * boardQualities.width / 8 + boardQualities.width / 16 + boardQualities.x;
    targetY = fromRow * boardQualities.height / 8 + boardQualities.height / 16 + boardQualities.y;
    SetCursorPos(targetX, targetY);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    targetX = toCol * boardQualities.width / 8 + boardQualities.width / 16 + boardQualities.x;;
    targetY = toRow * boardQualities.height / 8 + boardQualities.height / 16 + boardQualities.y;;
    SetCursorPos(targetX, targetY);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    targetX = boardQualities.x - 50;
    targetY = boardQualities.y - 50;
    SetCursorPos(targetX, targetY);

    return;
}