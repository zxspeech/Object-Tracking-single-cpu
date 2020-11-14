//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/imgproc.hpp>
//#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
//#include "particleFilter2D.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

void showInfo(const Mat& img, const int & counter){
    std::cout<<"-----"<< counter <<"------"<<std::endl;
    std::cout<<"1. depth: "<<img.depth()<<std::endl;   //-> 0.
    std::cout<<"2. cols: "<<img.cols<<std::endl;  //-> 1280.
    std::cout<<"3. rows: "<<img.rows<<std::endl;  //-> 720.
    std::cout<<"4. channels: "<<img.channels()<<std::endl;    //-> 3.
    std::cout<<"5. type: "<<img.type()<<std::endl;    //-> 16. CV_8UC3, //-> 0. CV_8UC1
    imshow("img", img);
    std::cout<<"-----------"<<std::endl;
    waitKey(2000);
    
}

class pix{
public:
    int r;
    int g;
    int b;
    pix():r(0), g(0), b(0) {}
    pix(int x,int y, int z) : r(x), g(y), b(z) {}
};

void readRGBtxt(std::string filename, std::vector<std::vector<pix> >& RGB){
    int rows, cols, x;
//    int counter = 0;
//    pix tempPix;


    std::cout << filename << endl;

    std::ifstream inFile;
    inFile.open(filename);
    if (!inFile) {
        std::cout << "Error: Unable to open file";
        exit(1);
    }
    inFile >> rows;
    inFile >> cols;

//    std::cout<<"rows: "<< rows <<", cols: "<< cols << std::endl;
    RGB.assign(rows, vector<pix>(cols, pix(0,0,0)));

    for(int i=0; i< rows; i++){
        for(int j=0; j< cols; j++){
            for(int k=0; k< 3; k++){   // read those 3 values.
                inFile >> x;
                if(0 == k%3){    // read 1st column.
//                    tempPix = pix(0, 0, 0);
//                    tempPix.r = x;
                    RGB[i][j].r = x;
//                    cout<<"x: "<< x <<", RGB[i][j].r : "<< RGB[i][j].r  <<endl;
                }
                else if(1 == k%3){   // read 2nd column.
//                    tempPix.g = x;
                    RGB[i][j].g = x;
                }
                else if(2 == k%3){   // read 3rd column.
//                    tempPix.b = x;
                    RGB[i][j].b = x;
                }
//                counter++;
            }
        }
    }
    inFile.close();

}

void showGT(std::vector<std::pair<int, int> >* GT){
    for(int i=0, sz= (*GT).size(); i<sz; i++)
        std::cout << (*GT)[i].first <<", " << (*GT)[i].second << std::endl;

}

void RGBToMat(const std::vector<std::vector<pix> >& RGB, Mat& frame){

    int debugging = 0;
    MatIterator_<Vec3b> color_it, color_end;
    int rows = RGB.size();
    int cols = RGB[0].size();
    int rid=0, cid=0;

    frame = Mat::zeros(rows, cols, CV_8UC3);
//    std::cout<<"rows: "<< rows <<", cols: "<< cols << std::endl;
    int counter = 0;

    for(color_it = frame.begin<Vec3b>(), color_end = frame.end<Vec3b>();
        color_it != color_end; ++color_it) {
        cid = counter%cols;
        rid = counter/cols;

//        cout<< "counter: "<< counter<<", rows: "<< rid <<", cols: "<< cid <<endl;


        (*color_it)[0] = (uchar)RGB[rid][cid].r; // blue
        (*color_it)[1] = (uchar)RGB[rid][cid].g; // green
        (*color_it)[2] = (uchar)RGB[rid][cid].b; // red
//        cout<< "b: "<< RGB[rid][cid].b<<", g: "<< RGB[rid][cid].g <<", r: "<< RGB[rid][cid].r <<endl;
//        (*color_it)[0] = 0; // blue
//        (*color_it)[1] = 255; // green
//        (*color_it)[2] = 0; // red

        counter++;

        debugging++;
//        if(2000 <= debugging)
//            break;
    }

}





void writeVideoToTXT(const Mat& frame, std::string outPutFoldername, const int index){

    std::string tempIndex = to_string(index);

    std::string fullFileName =  outPutFoldername.append(tempIndex);
    fullFileName = outPutFoldername.append(".txt");

    std::cout << fullFileName << endl;

    ofstream outFile(fullFileName);
    if (!outFile.is_open()) {
        std::cout << "Error: Unable to open file" << endl;
        exit(1);
    }

    // write dimension (rows cols) into TXT file.
    outFile << frame.rows << " " << frame.cols <<std::endl;




    int xudongCounter = 0;
    MatConstIterator_<Vec3b> color_it, color_end;
    int temp0 = 0,temp1 = 0,temp2 = 0, sqrSum=0;


    for(color_it = frame.begin<Vec3b>(), color_end = frame.end<Vec3b>();
        color_it != color_end; ++color_it) {

        temp0 = (*color_it)[0]; // blue
        temp1 = (*color_it)[1]; // green
        temp2 = (*color_it)[2]; // red

//        std::cout << "temp0 = "<< temp0 <<std::endl;
//        std::cout << "temp1 = "<< temp1 <<std::endl;
//        std::cout << "temp2 = "<< temp2 <<std::endl;
        outFile << temp0 << " " << temp1 << " " << temp2 <<std::endl;
        xudongCounter++;
    }



//    std::cout << sqrSum << std::endl;
//    std::cout << face.rows << std::endl;
//    std::cout << face.cols << std::endl;
//    std::cout << mse << std::endl;
//    std::cout << singleWeight << std::endl;
    outFile.close();

}


int main(int argc, char** argv){
    

    const string outPutFoldername = "./inputVideoOutputTxt/";
    const string filename = "./movingDucks.mov";
    VideoCapture cap(filename);
    if(!cap.isOpened()){
        std::cout << "Error opening video stream or file" << std::endl;
        return -1;
    }
    Mat frame;    // 1.


    int index = 0;
    while(true){    // 5.

        cap >> frame; // 2.
        if (frame.empty())  // 7.
            break;  // 8.
        writeVideoToTXT(frame, outPutFoldername, index);
        index++;
        if(index >= 4)
            break;
//        imshow("xudong", frame);
//        waitKey(1);
    }   // 6.






//----------- read frame from txt files --------------
//    std::vector<std::vector<pix> > RGB;
//    readRGBtxt("./inputVideoOutputTxt/0.txt", RGB);
////    for(int i=0; i< RGB.size(); i++) {
////        for (int j = 0; j < RGB[0].size(); j++) {
////            std::cout <<RGB[i][j].r << " "<< RGB[i][j].g << " "<< RGB[i][j].b << std::endl;
////        }
////    }
//
//
//
//    Mat tempFrame;
//    RGBToMat(RGB, tempFrame);
//    imshow("xudong", tempFrame);
//    waitKey(3000);
//
////    showInfo(frame, 1);
//----------------------------------------------------

     // When everything done, release the video capture object
     cap.~VideoCapture ();    // 3.
     cap.release();   // 4.

     // Closes all the frames
     destroyAllWindows();
    
    return 0;
}
