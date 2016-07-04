#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <unistd.h>

#include "../include/tiobj.hpp"
#include "../include/tiobj-cv.hpp"

using namespace std;
using namespace  cv;




int main(int argc, char** argv){
	VideoCapture capture;
	capture.open( 0 );

	//TiObj obj;
	Mat img, img2;
	while(1){
		TiObj obj;
		capture >> img;
		obj << img;
		obj->save("image", false);

		TiObj iii(false, "image");
		img2 << iii;

		imshow("sss",img2);
		waitKey(20);

		usleep(100000);
	}

	return 0;
}
