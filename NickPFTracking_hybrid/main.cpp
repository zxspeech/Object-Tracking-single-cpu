// Copyright by Xudong Zhang, City University of New York.

#include "mpi.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <random>
#include <string>
#include "particleFilter2D.h"
#include "mySet.h"
#include "readgt.h"

using namespace cv;


MPI_Datatype createWeightType(){
    MPI_Datatype new_type;
    int count = 3;
    int blocklens[] = { 1,1,1 };
    MPI_Aint indices[3];
    indices[0] = (MPI_Aint)offsetof(class detailWeights, nodeId);
    indices[1] = (MPI_Aint)offsetof(class detailWeights, particleId);
    indices[2] = (MPI_Aint)offsetof(class detailWeights, weight);
    MPI_Datatype old_types[] = {MPI_INT, MPI_INT, MPI_DOUBLE};
    MPI_Type_create_struct(count,blocklens,indices,old_types,&new_type);
    MPI_Type_commit(&new_type);
    return new_type;
}

MPI_Datatype createRouteType(){
    MPI_Datatype new_type;
    int count = 6;
    int blocklens[] = { 1,1,1,1,1,1 };
    MPI_Aint indices[3];
    indices[0] = (MPI_Aint)offsetof(class route, sourceId);
    indices[1] = (MPI_Aint)offsetof(class route, sParticleId);
    indices[2] = (MPI_Aint)offsetof(class route, NumtoSend);
    indices[3] = (MPI_Aint)offsetof(class route, desId);
    indices[4] = (MPI_Aint)offsetof(class route, dParticleId);
    indices[5] = (MPI_Aint)offsetof(class route, NumtoRec);
    MPI_Datatype old_types[] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Type_create_struct(count,blocklens,indices,old_types,&new_type);
    MPI_Type_commit(&new_type);
    return new_type;
}

const int NumofTransparticles = 3;
const int timeInterval = 1;
//const int NumofparticlesPerCore = 20;

int main(int argc, char** argv) {

    MPI_Status stat;
    int myRank, numProcs, ier;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
//    MPI_Datatype MPI_pair = createRecType();
//    int procSize = numberOfParticles/numProcs;


    MPI_Datatype MPI_pair;
    MPI_Type_contiguous(2, MPI_INT, &MPI_pair);
    MPI_Type_commit(&MPI_pair);

    MPI_Datatype MPI_Route;
    MPI_Type_contiguous(6, MPI_INT, &MPI_Route);
    MPI_Type_commit(&MPI_Route);
//    MPI_Datatype MPI_Route = createRouteType();

    MPI_Datatype MPI_WEI = createWeightType();
    int NumofparticlesPerCore = std::stoi(argv[1]);

    double startTimer, endTimer, totalTime;

//    if(0 == myRank){
////        std::cout<< argc <<std::endl;
//int jk = std::stoi(argv[1]);
//            std::cout<<jk<<", "<<std::endl;
//    }

//
//    std::cout << "Hello" << std::endl;



    //    VideoCapture cap(0);  // capture the camera.

//    const String filename = "./debate.mov";
//    const String filename = "./pres_debate.avi";
    const String filename = "./movingDucks.mov";

    VideoCapture cap(filename);
    if (!cap.isOpened()) {
        std::cout << "Error opening video stream or file" << std::endl;
        return -1;
    }


    Mat frame, colorFace;
    cap >> frame;
//    showInfo(&frame,3);
//    cvtColor(frame, fraGray1, COLOR_BGR2GRAY);

    //    Mat img = imread("img1.png, IMREAD_COLOR);
//    if (fraGray1.empty())
//        throw std::runtime_error("unable to open the image");

    //    Mat search_space(frame.rows, frame.cols, fraGray1.type()); // create search_space according to frame.
    Mat search_spaceColor(frame.rows, frame.cols, frame.type());
    //    showInfo(search_space,7);
    //    showInfo(search_spaceColor,8);



    //    showInfo(fraGray1,1);
//        int x=350, y=160, width=130, heigth=155; // for debate accurately.
    int x = 320, y = 183, width = 190, heigth = 70; // for movingDucks.mov.



//    Mat dst_roi = fraGray1(cv::Rect(x, y, width, heigth));
//    face = dst_roi.clone(); // this is also good -> dst_roi.copyTo(face);

    Mat dst_roiColor = frame(cv::Rect(x, y, width, heigth));
    colorFace = dst_roiColor.clone(); // this is also good -> dst_roi.copyTo(face);

    //    showInfo(dst_roi,2);
    //    showInfo(frame,3);
    //    showInfo(search_space,4);
    //    showInfo(face,5);
//        showInfo(colorFace,6);

    //    std::cout<<"colorFace: "<<std::endl;


//    particleFilter2DColor tracker = particleFilter2DColor(colorFace, x,y, frame,
//                search_spaceColor, NumofparticlesPerCore, 2, 10, 50); //for debate.


    particleFilter2DColor tracker = particleFilter2DColor(colorFace, x, y, frame,
            search_spaceColor, NumofparticlesPerCore, numProcs, 2, 20, 22); //for ducklings.

    if(0 == myRank){
        readGT("GT.txt", tracker.GT);
//        showGT(&tracker.GT);
    }



//     // for debugging:
   int xudongcounter = 0;

    startTimer = MPI_Wtime();
while(true) {

    tracker.update(&frame);


    if(0 == tracker.timeStep % timeInterval){
//    if(true){
//        if(0 == myRank){
//            std::cout<<"Enter Global "<< std::endl;
//        }

     //--------------------------------- after sampling ------------------------------------------
     // xudong: all processors need to wait for resampling here !!!

 //    MPI_Barrier(MPI_COMM_WORLD);    // all processors are waiting here.
 //
 //

     // resampling as followings:
     // 1. obtain the deWei.
     for (int i = 0, ter = tracker.num_particles; i < ter; i++) {
         // 1. deWei needs "myRank", so needs to do here, not inside class.
         tracker.deWei[i] = detailWeights(myRank, i, tracker.oriWeis[i]);
     }


     // 2. assign the space for totalWeight on myRank=0.
     if (0 == myRank) {
         tracker.totalWeights.assign(tracker.num_particles * numProcs, detailWeights());
 //        &(tracker.totalWeights) = malloc(sizeof(detailWeights) * tracker.num_particles * numProcs);
     }


 //    MPI_Barrier(MPI_COMM_WORLD);    // all processors are waiting here.
 //    for(int i = 0, ter=tracker.num_particles; i<ter; i++){
 //        std::cout<<myRank<<" -> nodeId: "<<tracker.deWei[i].nodeId
 //        <<", particleId: "<<tracker.deWei[i].particleId
 //        <<", weight: "<<tracker.deWei[i].weight<<std::endl;
 //    }

     // 3. copy the deWei on all the PU to CU (myRank=0)
     MPI_Barrier(MPI_COMM_WORLD);    // all processors are waiting here.
     MPI_Gather(&(tracker.deWei.front()), tracker.num_particles, MPI_WEI,
                &(tracker.totalWeights.front()), tracker.num_particles, MPI_WEI, 0, MPI_COMM_WORLD);

 //    MPI_Barrier(MPI_COMM_WORLD);    // all processors are waiting here.
 //
 //    if(0 == myRank) {
 //        std::cout << "========== checking ======= totalWeights =================" << std::endl;
 //        for (int i = 0, ter = tracker.totalWeights.size(); i < ter; i++) {
 //            std::cout << myRank << " -> nodeId: " << tracker.totalWeights[i].nodeId
 //                      << ", particleId: " << tracker.totalWeights[i].particleId
 //                      << ", weight: " << tracker.totalWeights[i].weight << std::endl;
 //        }
 //    }

     // 4. normalizing the totalWeights on CU (myRank=0).
     if (0 == myRank) {
         weiNormalizor(tracker.totalWeights);
     }

 //    if(0 == myRank) {
 //        std::cout << "========== checking ======= Normalized totalWeights =================" << std::endl;
 //        for (int i = 0, ter = tracker.totalWeights.size(); i < ter; i++) {
 //            std::cout << myRank << " -> nodeId: " << tracker.totalWeights[i].nodeId
 //                      << ", particleId: " << tracker.totalWeights[i].particleId
 //                      << ", weight: " << tracker.totalWeights[i].weight << std::endl;
 //        }
 //    }



     // 5. global resampling on CU (myRank=0) and calculate the 2d vector -> goodParVec.
     if (0 == myRank) {
 //        std::vector<std::multiset<int> > goodParVec(numProcs, std::multiset<int>());
         tracker.goodParVec.assign(numProcs, std::vector<int>());
         selectGoodPars(tracker.totalWeights, numProcs, tracker.goodParVec);

         //update v1 and v2:
         tracker.v1.assign(numProcs, 0);
         tracker.v2.assign(numProcs, false);

         for (int i = 0, ter = tracker.goodParVec.size(); i < ter; i++) {
             tracker.v1[i] = tracker.goodParVec[i].size();
             if (tracker.v1[i] != NumofparticlesPerCore) {
                 tracker.v2[i] = true;   // find the PU, which need send and receive pars.
             }
         }


 //        std::cout << "1. === checking === goodParVec =====" << std::endl;
 //        showGoodParVec(tracker.goodParVec);
 //        std::cout << "1. ============= Finish ============" << std::endl;
         if(checkv1(tracker.v1) != numProcs * NumofparticlesPerCore){
             std::cout << "Error here, total # of particle = "<< checkgoodParVec(tracker.goodParVec)<<std::endl;
             break;
         }

 //        for (int i = 0, ter = tracker.goodParVec.size(); i < ter; i++) {
 //            std::cout << i << " -> # of good parts : " << tracker.goodParVec[i].size() << std::endl;
 //            for (auto t : tracker.goodParVec[i]) {
 //                std::cout << "PU: " << i << ", particleId: " << t << std::endl;
 //            }
 //            std::cout << std::endl;
 //        }
     }

     //6. sending the size of good particles for those poor PUs, for self-copying.
     if (0 == myRank) {
 //        std::cout << "2. ============== sending... =============" << std::endl;
         for (int i = 0, ter = (int) tracker.goodParVec.size(); i < ter; i++) {
 //            int Nofpars = (int) tracker.goodParVec[i].size();
 //            std::cout << "myRank: " << myRank << ", sending size (integer): " << tracker.v1[i] << std::endl;
             MPI_Send(&(tracker.v1[i]), 1, MPI_INT, i, i, MPI_COMM_WORLD);   // sending selfCopyVecLength from PU0.
         }
 //        std::cout << "2. ============= Finish ============" << std::endl;

     }

     //7. Receiving the size of good particles and ini selfCopyVec on poor PUs.
     MPI_Barrier(MPI_COMM_WORLD);
     for (int i = 0; i < numProcs; i++) {
         if (i == myRank) {
             MPI_Recv(&tracker.selfCopyVecLength, 1, MPI_INT, 0, i, MPI_COMM_WORLD,
                      &stat);   //receive selfCopyVecLength size.
             tracker.selfCopyVec.assign(tracker.selfCopyVecLength, -1);
 //            std::cout << " 3. myRank: " << myRank << " creates selfCopyVec with size: " << tracker.selfCopyVec.size()
 //                      << std::endl;


         }
     }


     // 8. sending the index of good particles from PU0 to those poor PUs for self-copying.
     if (0 == myRank) {
 //        std::cout << "3. ----------- Sending the index vector of goodParVec  to poor PUs ----------" << std::endl;
         for (int i = 0, ter = (int) tracker.goodParVec.size(); i < ter; i++) {
             int Nofpars = (int) tracker.goodParVec[i].size();
             if (Nofpars < NumofparticlesPerCore && Nofpars > 0) { // then i is the index for the poor PUs.
                 // send the 2.[1 3] to PU2 from PU0, 1,3 are the indices of good particle on PU2.
 //                std::cout << " 4. sending to: " << i << ", with tag: " << i * numProcs << std::endl;
                 MPI_Send(&tracker.goodParVec[i][0], (int) tracker.goodParVec[i].size(), MPI_INT, i, i * numProcs,
                          MPI_COMM_WORLD);
             }
         }
 //        std::cout << "3. ----------- Finish Sending goodParVec ----------" << std::endl;

     }


 //    if(0 == myRank){
 //        MPI_Barrier(MPI_COMM_WORLD);
 //        std::cout << "----------- Receiving goodParVec ----------" << std::endl;
 //    }

     MPI_Barrier(MPI_COMM_WORLD);
     // 9. Receiving the index of good particles for those poor PUs, for self-copying.
     for (int i = 0; i < numProcs; i++) {
         if (i == myRank) {
             if (tracker.selfCopyVecLength < NumofparticlesPerCore && tracker.selfCopyVecLength > 0) {
 //                std::cout << " 5. myRank: "<< myRank << " is receiving from PU0. with tag: " << myRank * numProcs << std::endl;
                 MPI_Recv(&tracker.selfCopyVec[0], tracker.selfCopyVecLength, MPI_INT, 0, i * numProcs, MPI_COMM_WORLD,
                          &stat);
             }
         }
     }

 //    for (int i = 0, ter = (int) tracker.selfCopyVec.size(); i < ter; i++) {
 //        if(tracker.selfCopyVec[i] < 0) // do not need to do self-copy, it's either good PU or poor PU with 0 good particle.
 //            break;
 //        std::cout << " 6. myRank: " << myRank << " need to copy it's part with index on it from beginning ->  " << tracker.selfCopyVec[i] << std::endl;
 //    }

        tracker.parTemp.assign(tracker.particle.begin(), tracker.particle.end());
     // 10. On poor PUs, rearrange particle according to selfCopyVec.
 //    showPart(numProcs, myRank, tracker.particle, 1);
     if (tracker.selfCopyVecLength < NumofparticlesPerCore && tracker.selfCopyVecLength > 0) {


         for (int i = 0, ter = (int) tracker.selfCopyVec.size(); i < ter; i++) {
             tracker.particle[i].first = tracker.parTemp[tracker.selfCopyVec[i]].first;
             tracker.particle[i].second = tracker.parTemp[tracker.selfCopyVec[i]].second;
         }


     }
 //    MPI_Barrier(MPI_COMM_WORLD);
 //    showPart(numProcs, myRank, tracker.particle, 2);






     // 11. Clear and  Create t1 and t2 according to goodParVec.
     // s1 and s2 (useless) the vector containing the PUs with surplus and shortage particles.
     if (0 == myRank) {

         tracker.t1.clear();
         tracker.t2.clear();


         for (int i = 0, ter = (int) tracker.goodParVec.size(); i < ter; i++) {
             int Nofpars = (int) tracker.goodParVec[i].size();

             if (Nofpars == NumofparticlesPerCore) {
                 // no particle need to be exchanged.
             } else if (Nofpars > NumofparticlesPerCore) {
                 // has surplus particles.
 //                tracker.s1.push_back(tracker.goodParVec[i]);
 //                for (int j = 0, ter = tracker.goodParVec[i].size(); j < ter; j++) {
 //                    tracker.s1[i][j] = tracker.goodParVec[i][j];
 //                }
                 std::vector<std::pair<int, int> > results;
                 Histogram(tracker.goodParVec[i], results);

                 int PUid = i;
                 for (int i = 0, ter = results.size(); i < ter; i++) {
                     int Npars = results[i].second;
                     int ParId = results[i].first;
                     tracker.t1.push_back(t1Piece(PUid, Npars, ParId));
                 }
             } else {
                 // has shortage of particles.
 //                tracker.s2.push_back(tracker.goodParVec[i]);
 //                for (int j = 0, ter = tracker.goodParVec[i].size(); j < ter; j++) {
 //                    tracker.s2[i][j] = tracker.goodParVec[i][j];
 //                }

                 int PUid = i;
                 std::vector<int> parEntryidVec;
 //                std::cout<<"Pass 1"<<std::endl;
                 createParEntryidVec((int) tracker.goodParVec[i].size(), NumofparticlesPerCore,
                                     parEntryidVec); //obtain the pari entries vector.
 //                std::cout<<"Pass 2"<<std::endl;
                 int NumofneededPar = NumofparticlesPerCore - Nofpars;   // must be positive.
                 tracker.t2.push_back(t2Piece(PUid, NumofneededPar, parEntryidVec));


 //                typedef std::vector<std::pair<PUid, std::pair<Npars, ParIds> > > t1Map;
 //                typedef std::vector<std::pair<PUid, std::pair<Npars, std::vector<ParIds> > > > t2Map;



 //                for (int i = 0, ter = parEntryidVec.size(); i < ter; i++) {
 //                    std::cout<<parEntryidVec[i]<<", ";
 //                }
 //                std::cout<<std::endl;
             }
         }

 //        showt1Tab(tracker.t1);
 //        showt2Tab(tracker.t2);
         sort(tracker.t1.begin(), tracker.t1.end(), t1Sorter);
         sort(tracker.t2.begin(), tracker.t2.end(), t2Sorter);

 //        showt1Tab(tracker.t1);
 //        showt2Tab(tracker.t2);
 //        std::cout << "------------------ Finish step 11. on PU0 -----------------" << std::endl;
 //        show1DVectorContents(tracker.v1);
 //        show1DVectorContents(tracker.v2);

 //        showtSTab(tracker.s1);
 //        showtSTab(tracker.s2);

     }


     // 12. Clear TransSchedule and Create routing schedules.
     if (0 == myRank) {
         tracker.TransSchedule.clear();

         int debugCounter = 0;

         while (true) {
             debugCounter++;

             int numParTrns = 0, t1Num, t2Num, surNum;
             int k = 0;  // send the kth row of t1.
             int j = 0;    // receive at jth row of t2.

             // obtain numParTrns from t1.
             for (int ter = tracker.t1.size(); k < ter; k++) {
                 if (tracker.v2[tracker.t1[k].PUid] == true) {
                     t1Num = tracker.t1[k].Npars;  // =12.
                     break;
                 }
             }

             // compare numParTrns with surplus number of parts.
             surNum = tracker.v1[tracker.t1[k].PUid] - NumofparticlesPerCore;    //=5. must > 0
             numParTrns = t1Num < surNum ? t1Num : surNum;

             // obtain numParTrns from t1.
             for (int ter = tracker.t2.size(); j < ter; j++) {
                 if (tracker.v2[tracker.t2[j].PUid] == true) {
                     t2Num = tracker.t2[j].Npars;  // =5.
                     break;
                 }
             }

             numParTrns = numParTrns < t2Num ? numParTrns : t2Num; //=5.




 //            std::cout << "------------------ 2 -----------------" << std::endl;
 //            std::cout << "t1Num: " << t1Num << std::endl;
 //            std::cout << "surNum: " << surNum << std::endl;
 //            std::cout << "t2Num: " << t2Num << std::endl;
 //            std::cout << "numParTrns: " << numParTrns << std::endl;
 //            std::cout << "-> k: " << k << ", j: " << j << std::endl;
 //            showt1Tab(tracker.t1);
 //            showt2Tab(tracker.t2);
 //            std::cout << "v1: size():" << tracker.v1.size() << std::endl;
 //            show1DVectorContents(tracker.v1);
 //            std::cout << "v2: size():" << tracker.v2.size() << std::endl;
 //            show1DVectorContents(tracker.v2);


             // add route into TransSchedule:
             for (int i = 0; i < numParTrns; i++) {
 //                std::cout << "t1-> PUid: " << tracker.t1[k].PUid << ", ParIds: " << tracker.t1[k].ParIds <<
 //                          ", t2-> PUid: " << tracker.t2[j].PUid << ", ParIds: " << tracker.t2[j].ParIds.back()
 //                          << " (";
 //                for (auto t: tracker.t2[j].ParIds)
 //                    std::cout << t << ", ";
 //                std::cout << " )";

                 tracker.TransSchedule.push_back(route(tracker.t1[k].PUid, tracker.t1[k].ParIds, 1,
                                                       tracker.t2[j].PUid, tracker.t2[j].ParIds.back(), 1));

                 if ( ! tracker.t2[j].ParIds.empty() ){
                     tracker.t2[j].ParIds.pop_back();
                 }
                 else{
                     std::cout << " Vector, named ParIds, is Empty, Error here." << std::endl;
 //                    goto label;
                 }

 //                if(! tracker.t2[j].ParIds.empty() ){
 //                    std::cout << " -> after pop_back the last entry: (";
 //                    for (auto t: tracker.t2[j].ParIds)
 //                        std::cout << t << ", ";
 //                    std::cout << " )";
 //                }


 //                std::cout << std::endl;
             }


             updateTablesVecs(k, j, tracker.t1, tracker.t2, tracker.v1, tracker.v2, numParTrns, NumofparticlesPerCore);

 //            showt1Tab(tracker.t1);
 //            showt2Tab(tracker.t2);
 //            std::cout << "v1: " << std::endl;
 //            show1DVectorContents(tracker.v1);
 //            std::cout << "v2: " << std::endl;
 //            show1DVectorContents(tracker.v2);

             int trueNumber = 0;
             for (int i = 0, ter = tracker.v2.size(); i < ter; i++) {
                 if (true == tracker.v2[i])
                     trueNumber++;
             }
             if (0 == trueNumber)
                 break;


 //            if (3 == debugCounter)
 //                break;

         }
 //        label:
 //        std::cout << "------------------- Finish step 12. on PU0 ----------------" << std::endl;
         tracker.selfCopyVec.clear();
         tracker.selfCopyVec.assign(numProcs*NumofparticlesPerCore,-1);
         for (int i = 0, ter=tracker.t1.size(); i < ter; i++) {
             int id = tracker.t1[i].PUid * NumofparticlesPerCore + tracker.t1[i].ParIds;
             tracker.selfCopyVec[id] = tracker.t1[i].Npars;
         }
     }

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Scatter(&tracker.selfCopyVec[0], NumofparticlesPerCore, MPI_INT, &tracker.GoodParselfCopyVec[0],
                    NumofparticlesPerCore, MPI_INT, 0, MPI_COMM_WORLD);

     // 13. PU0 Broadcasts the TransSchedule vector to the rest of PUs.
     MPI_Barrier(MPI_COMM_WORLD);
     if (0 == myRank) {
         int Num = tracker.TransSchedule.size();
         tracker.SchSize = Num;
         tracker.NofparTrans += Num;
         tracker.cumNparTrans.push_back(Num);
 //        std::cout << "------------------- Finish step 13. on PU0 ----------------" << std::endl;
     }
     //14. send the TransSchedule size.
     MPI_Bcast(&tracker.SchSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
     if (0 != myRank) {
         tracker.TransSchedule.assign(tracker.SchSize, route());
 //        std::cout << "------------------- Finish step 14. on PU0 ----------------" << std::endl;
     }


     MPI_Barrier(MPI_COMM_WORLD);
     // send the TransSchedule.
     MPI_Bcast(&tracker.TransSchedule[0], tracker.TransSchedule.size(), MPI_Route, 0, MPI_COMM_WORLD);
 //    MPI_Bcast(&tracker.TransSchedule[0], 6, MPI_INT, 0, MPI_COMM_WORLD);
 //    MPI_Barrier(MPI_COMM_WORLD);
 //    for (int i = 0; i < numProcs; i++) {
 //        if(i == myRank){
 ////            std::cout<<"tracker.SchSize: "<< tracker.SchSize <<std::endl;
 //            std::cout<<myRank<<" -> tracker.TransSchedule.size(): "<< tracker.TransSchedule.size() <<std::endl;
 ////            showSch(tracker.TransSchedule, myRank);
 //        }
 //    }

 //    if(3 == myRank){
 //        showSch(tracker.TransSchedule, myRank);
 //    }


     // 15. Sending and Receiving particles according to the TransSchedule.
        MPI_Barrier(MPI_COMM_WORLD);

        for (int i = 0, ter = tracker.TransSchedule.size(); i < ter; i++) {
            int s = tracker.TransSchedule[i].sourceId;
            int sPid = tracker.TransSchedule[i].sParticleId;
            int d = tracker.TransSchedule[i].desId;
            int dPid = tracker.TransSchedule[i].dParticleId;

            if (myRank == s) {
                MPI_Send(&tracker.particle[sPid], 1, MPI_pair, d, i, MPI_COMM_WORLD);
            }

            else if (myRank == d) {
                MPI_Recv(&tracker.particle[dPid], 1, MPI_pair, s, i, MPI_COMM_WORLD, &stat);
            }

        }

        for (int i = 0, ter = tracker.GoodParselfCopyVec.size(); i < ter; i++) {
            int j = 0;
            int parSize = tracker.GoodParselfCopyVec[i];    //=4.
            if( parSize > 0 ){  //i=3.
                for (; j < NumofparticlesPerCore; j++) {
                    tracker.particle[j].first = tracker.parTemp[i].first;
                    tracker.particle[j].second = tracker.parTemp[i].second;
                }
            }
        }
     MPI_Barrier(MPI_COMM_WORLD);
//     showPairVectorContents(tracker.particle);



 //    showPairVectorContents(tracker.particle);

    }
    else{   // locally.
        MPI_Barrier(MPI_COMM_WORLD);
//        if(0 == myRank){
//            std::cout<<"Enter Local "<< std::endl;
//        }
        tracker.resampling();   // locally.


        // sending...
//        if(0 == myRank){
//            std::cout<<"Sending... "<< std::endl;
//        }



        for (int i = 0; i < NumofTransparticles; i++) {
            int index1 = randIndex(tracker.num_particles);
//            std::cout<<"index1: "<< index1 <<std::endl;
            int index2 = randIndex(tracker.num_particles);
//            std::cout<<"index2: "<< index2 <<std::endl;
            for (int j = 0; j < numProcs; j++) {
                int tag = (i+1)*(j+1)*NumofTransparticles*numProcs;
                if (myRank == j) {
//                    std::cout<<"1. myRank: "<<myRank<<" send to destination: "
//                    <<(j + 1) % numProcs<<" -> index: "<< index1 <<", tag: "<<tag<<std::endl;

                    MPI_Send(&(tracker.particle[index1]),1, MPI_pair, (j + 1) % numProcs, tag, MPI_COMM_WORLD);

//                    std::cout<<"particle: "<<tracker.particle[index1].first<<", "<<tracker.particle[index1].second<< std::endl;
                }
                else if (myRank == ((j + 1) % numProcs)) {
//                    std::cout<<"2. myRank: "<<myRank<<" receive from source: "
//                    <<j<<" -> receive index: "<< index2 <<", tag: "<<tag<< std::endl;

                    MPI_Recv(&(tracker.particle[index2]),1, MPI_pair, j, tag, MPI_COMM_WORLD, &stat);

//                    std::cout<<"particle: "<<tracker.particle[index2].first<<", "<<tracker.particle[index2].second<< std::endl;
                }

            }
            MPI_Barrier(MPI_COMM_WORLD);
        }

    }

//    if(0 == myRank){
//        std::cout<<"End of either global or local "<< std::endl;
//    }

    tracker.estimation();
    MPI_Barrier(MPI_COMM_WORLD);    // all processors are waiting here for following reducing.

    // gathering all the estimation and show estimation on myRank0:
    MPI_Reduce(&tracker.prediction.first, &tracker.prediction_MPI_sum.first, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&tracker.prediction.second, &tracker.prediction_MPI_sum.second, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    tracker.timeStep++; // No matter global or local, timeStep = timeStep+1 in every time step.

    if (0 == myRank) {
//        std::cout << "------------------- showing image... ----------------" << std::endl;
        tracker.showPrediction(frame, timeInterval);
        imshow("xudong", frame);
    }
    waitKey(1);

    cap >> frame;
    if (frame.empty())
        break;

//    if(0 == myRank){
//        std::cout<<"tracker.timeStep on PU0: "<< tracker.timeStep <<std::endl;
//    }
    xudongcounter++;
//    if(xudongcounter == 1)
//        break;

}

// -----------------------------------------------------------------------------------------

    endTimer = MPI_Wtime();
    totalTime = endTimer - startTimer;
    MPI_Reduce(&totalTime, &tracker.timeComsumption, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if(0 == myRank){
        calRMSE(tracker.RMSE,tracker.GT,tracker.predVec);
        std::cout<<"NofparTrans: "<< tracker.NofparTrans << std::endl;
        std::cout<<"timeComsumption (Second): "<< tracker.timeComsumption << std::endl;
        std::cout<<"Number of Time Steps: "<< tracker.timeStep << std::endl;
        std::cout<<"Number of PU: "<< tracker.num_Procs << std::endl;
        std::cout<<"Number of Particles: "<< tracker.num_particles * tracker.num_Procs << std::endl;
        std::cout<<"RMSE: "<< tracker.RMSE << std::endl;
    }


     // When everything done, release the video capture object
     cap.~VideoCapture ();
     cap.release();
     destroyAllWindows();   // Closes all the frames

//    std::cout<<"Pass the end"<<std::endl;
    MPI_Type_free(&MPI_pair);
    MPI_Type_free(&MPI_WEI);
    MPI_Finalize();


    return 0;
}
