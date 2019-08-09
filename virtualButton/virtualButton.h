#define _CRT_SECURE_NO_WARNINGS

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <Windows.h>

using namespace cv;

#define maxRow 1
#define maxCol 6

class Camera
{
private:
	VideoCapture *cap = NULL;
	int errorCode;
	int initCamera();		
	const unsigned int camOpenError = 1;
	const unsigned int camSetParameterError = 2;

public:
	Camera();
	Mat getFrameFromCamera();	
	bool checkError();	
};

class VirtualButton
{
private:	
	Mat camMat, currMat, prevMat, showMat;
	bool checkButton(Rect myRect, int rowIndex, int colIndex);	
	void turnLightOn(int rowIndex, int colIndex);
	void checkButtonPressed();
	void imagePostprocess();
	void moveWindowRight();
	void moveWindowLeft();
	void moveWindowDown();	
	void moveWindowUp();
	void imagePreprocess();
	void drawButtons();
	void showResult();	
	void initWindow();
	void initVariable();
	double getCost(Rect buttonRect);

	int costTh = 52000;
	char showTime[200];	

	int currRowIndex = -1;
	int currColIndex = -1;

	int windowX = 0;
	int windowY = 0;
	int textPosition[maxRow][maxCol];
	Rect buttonRect[maxRow][maxCol];
	Scalar buttonColor[maxRow][maxCol];

public:
	VirtualButton();
	void setCamFrame(Mat camFrame);
	void processOneFrame();	
};