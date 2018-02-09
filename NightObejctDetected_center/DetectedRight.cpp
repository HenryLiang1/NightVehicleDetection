#include "DetectedRight.h"
DetectedRight::DetectedRight(string path) :DetectedPosition(path)
{
	position= Rect(0, videoSize.height / 2, videoSize.width / 2, videoSize.height / 2);
}

DetectedRight::~DetectedRight()
{
}

void DetectedRight::SetImageProcessor(ImageProcessor *imageProcessor)
{
	processor = imageProcessor;
}

Mat DetectedRight::getResult()
{
	return src;
}

#include <fstream>
void DetectedRight::run()
{
	CBrightObjectSegment brightObjectSegment(0.995);
	CBrightObjectSegment brightObjectSegment2(0.995);
	
	char filename[] = "FPS4.csv";
	fstream fp;
	fp.open(filename, ios::app);//�}���ɮ�
	if (!fp) {//�p�G�}���ɮץ��ѡAfp��0�F���\�Afp���D0
		cout << "Fail to open file: " << filename << endl;
	}


	while (true)
	{
		
		clock_t begin = clock();

		capture >> src;
		if (src.empty())
		{
			break;
		}
		//Dividing the video to left and right part and changing image from color to gray		
		src = src(position);
		cvtColor(src, Gray, CV_BGR2GRAY);

		//right part
		Rect ROI = Rect(0, Gray.rows * 28 / 100, Gray.cols * 17 / 20, Gray.rows * 5 / 12);
		rectangle(src, ROI, Scalar(0, 255, 255), 1, 8, 0); // draw ROI
		GrayRect = Gray(ROI);
		GrayRectTemp = GrayRect.clone();

		const int frontRectX = Gray.cols * 4 / 20;
		const int frontRectY = Gray.rows * 28 / 100;
		const int frontRectWidth = Gray.cols * 16 / 20;
		const int frontRectHeight = Gray.rows * 5 / 12;
		Rect frontRect = Rect(frontRectX, frontRectY, frontRectWidth, frontRectHeight);
		rectangle(src, frontRect, Scalar(0, 255, 255), 1, 8, 0); // draw Front ROI

		const int nearRectX = Gray.cols * 3 / 5;
		const int nearRectY = Gray.rows * 28 / 100;
		const int nearRectWidth = Gray.cols * 2 / 5;
		const int nearRectHeight = Gray.rows * 5 / 12;

		Rect nearRect = Rect(nearRectX, nearRectY, nearRectWidth, nearRectHeight);
		rectangle(src, nearRect, Scalar(0, 0, 255), 1, 8, 0); // draw Front ROI

		//imageProcessor.extractEfficientImage(rightGrayRectTemp);

		Rect frontROI = Rect(frontRectX, frontRectY, frontRectWidth, frontRectHeight);
		Rect nearROI = Rect(nearRectX, nearRectY, nearRectWidth, nearRectHeight);

		ROIs.push_back(frontROI); //detect and track single light 
		ROIs.push_back(nearROI);


		Mat front = GrayRectTemp(frontROI);
		Mat near = GrayRectTemp(nearROI);

		brightObjectSegment.getBinaryImage(front);
		brightObjectSegment2.getBinaryImage(near);

		processor->removeNoice(GrayRectTemp, 5, 5, 7, 7);
		processor->detectLight(src, GrayRectTemp, 0, Gray.rows * 28 / 100, ROIs);

		clock_t end = clock();
		double elapsedSecs = double(end - begin) / CLOCKS_PER_SEC;
		stringstream ss, ss1;
		ss << "FPS: " << int(1 / elapsedSecs);
		ss1 << int(1 / elapsedSecs);
		putText(src, ss.str(), Point(50, 50), 0, 1, Scalar(0, 0, 255), 3);
		fp << ss1.str() << endl;

		imshow("Right Result", src);
		imshow("Right Binary Result", front);
		imshow("Right Binary Result2", near);

		videoWriter << src;
		switch (1) {
		case 1:
			waitKey(1);
			break;
		case 0:
			int key = waitKey(-1);

			if (key == 120)
			{

				frame++;
				cout << "frame:" << frame << endl;
				continue;
			}
			else if (key == 122)
			{
				frame--;
				if (frame < 0)
				{
					frame = 0;
				}
				else
				{
					capture.set(CV_CAP_PROP_POS_FRAMES, frame);
					cout << "frame:" << frame << endl;
				}
			}
			break;
		}
	}

	fp.close(); 
}
