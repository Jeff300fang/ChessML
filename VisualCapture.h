#pragma once
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include "VisualPiece.h"
#include "Imports.h"

class VisualCapture
{
public:
	VisualCapture();
	std::vector<VisualPiece> getPieces();
	bool computerPlayingWhite();
	std::string playerMadeMove(bool computerWhite);
	void moveOnScreen(std::string move);
	void adjust();
private:
	HWND hwnd;
	std::map<int, char> pieceMap;
	cv::Ptr<cv::ml::KNearest> knn;
	cv::Mat hwnd2mat();
	cv::Mat getScreen();
	std::vector<std::pair<cv::Mat, cv::Rect>> getPieceImages();
	cv::Mat getBoard();
	VisualPiece predictPiece(cv::Mat image);
	void trainModel();
	void gatherTrainingData();
	cv::Rect getBoardCoordinates();
};

