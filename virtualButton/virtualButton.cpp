#include "virtualButton.h"

int main()
{
	VirtualButton *vb = new VirtualButton();	
	Camera *cam = new Camera();		

	if (cam->checkError() == true)
		return 0;

	while (true)
	{
		vb->setCamFrame(cam->getFrameFromCamera());		
		vb->processOneFrame();
		waitKey(30);
	}
	return 1;
}

Camera::Camera()
{
	errorCode = initCamera();
}

Mat Camera::getFrameFromCamera()
{
	const int horizontalFlag = 1;
	Mat camMat;
	cap->read(camMat);
	flip(camMat, camMat, horizontalFlag);
	return camMat;
}

void VirtualButton::processOneFrame()
{
	imagePreprocess();
	checkButtonPressed();
	imagePostprocess();
	drawButtons();
	showResult();
}

void VirtualButton::initVariable()
{
	for(int rowIndex=0;rowIndex<maxRow;rowIndex++)
		for (int colIndex = 0; colIndex < maxCol; colIndex++)
		{
			textPosition[rowIndex][colIndex] = 50;
			buttonRect[rowIndex][colIndex] = Rect(colIndex * 100 + 100, rowIndex * 100 + 100, 50, 50);
			int R, B, G;
			R = 255;
			G = 255;
			B = 255;
			buttonColor[rowIndex][colIndex] = Scalar(B, G, R);
		}
}

VirtualButton::VirtualButton()
{
	initVariable();
	drawButtons();
}

void VirtualButton::initWindow()
{
	namedWindow("show", WINDOW_AUTOSIZE);
	moveWindow("show", windowX, windowY);
}

void VirtualButton::setCamFrame(Mat camMat)
{
	this->camMat = camMat;
}

void VirtualButton::imagePreprocess()
{
	showMat = camMat.clone();
	currMat = camMat.clone();
}

void VirtualButton::imagePostprocess()
{
	prevMat = camMat.clone();
}

void VirtualButton::checkButtonPressed()
{
	bool isPressed = false;
	if (prevMat.cols > 0 && prevMat.rows > 0)
	{
		for (int rowIndex = 0; rowIndex < maxRow; rowIndex++)
			for (int colIndex = 0; colIndex < maxCol; colIndex++)
			{
				isPressed = checkButton(buttonRect[rowIndex][colIndex], rowIndex, colIndex);
				if (isPressed == true)
					return;
			}
	}
}

void VirtualButton::showResult()
{
	initWindow();
	cvShowImage("show", new IplImage(showMat));
}

void sendKey(char input)
{
	keybd_event(input , 0, 0, 0); // Alt Press 
	keybd_event(input, 0, KEYEVENTF_KEYUP, 0); // Alt Release
}

void VirtualButton::turnLightOn(int rowIndex, int colIndex)
{
	char text[200];
	int currIndex = rowIndex * maxCol + colIndex;
	sprintf(text, "Currnet Index=%d", currIndex);
	putText(showMat, text, Point(0, textPosition[rowIndex][colIndex]), 0, 1, buttonColor[rowIndex][colIndex], 3);
	if(currIndex==3)
		sendKey(VK_LEFT);
	if (currIndex == 5)
		sendKey(VK_RIGHT);
}

bool Camera::checkError()
{
	if ((errorCode & camOpenError) > 0)
	{
		printf("camera open failed\n");
		system("pause");
		return true;
	}
	if ((errorCode & camSetParameterError) > 0)
	{
		printf("camera set parameter failed\n");
		system("pause");
		return true;
	}

	return false;
}

int Camera::initCamera()
{
	cap = new VideoCapture(0);

	if (cap->isOpened() == false)
	{
		return camOpenError;
	}

	bool frameWidthResult = cap->set(CAP_PROP_FRAME_WIDTH, 1280);
	bool frameHeightResult = cap->set(CAP_PROP_FRAME_HEIGHT, 720);
	bool FpsResult = cap->set(CAP_PROP_FPS, 60);

	if (!(frameWidthResult && frameHeightResult && FpsResult))
	{
		return camSetParameterError;
	}

	return 0;
}

void VirtualButton::drawButtons()
{
	for (int rowIndex = 0; rowIndex < maxRow; rowIndex++)
		for (int colIndex = 0; colIndex < maxCol; colIndex++)
		{
			rectangle(showMat, buttonRect[rowIndex][colIndex], buttonColor[rowIndex][colIndex], -1);
		}
}

void VirtualButton::moveWindowRight()
{
	windowX += 5;
	if (windowX > 640)
		windowX = 640;
	moveWindow("show", windowX, windowY);
}

void VirtualButton::moveWindowLeft()
{
	windowX -= 5;
	if (windowX < 0)
		windowX = 0;
	moveWindow("show", windowX, windowY);
}

void VirtualButton::moveWindowUp()
{
	windowY -= 5;
	if (windowY < 0)
		windowY = 0;
	moveWindow("show", windowX, windowY);
}

void VirtualButton::moveWindowDown()
{
	windowY += 5;
	if (windowY > 480)
		windowY = 0;
	moveWindow("show", windowX, windowY);
}

double VirtualButton::getCost(Rect buttonRect)
{
	Mat currBGR;
	Mat prevBGR;
	Mat currHSV;
	Mat prevHSV;
	Mat splitCurrHSV[3];
	Mat splitPrevHSV[3];
	Mat diffMat;
	double cost = 0;

	currMat(buttonRect).copyTo(currBGR);
	prevMat(buttonRect).copyTo(prevBGR);
	cvtColor(currBGR, currHSV, CV_BGR2HSV);
	cvtColor(prevBGR, prevHSV, CV_BGR2HSV);
	split(currHSV, splitCurrHSV);
	split(prevHSV, splitPrevHSV);

	absdiff(splitCurrHSV[0], splitPrevHSV[0], diffMat);
	cost = sum(diffMat)[0];
	return cost;
}


bool VirtualButton::checkButton(Rect buttonRect, int rowIndex, int colIndex)
{
	double buttonCost = getCost(buttonRect);	

	printf("current Cost = %f\n", buttonCost);

	if (buttonCost > costTh)
	{
		turnLightOn(rowIndex, colIndex);
		return true;
	}
	return false;
}