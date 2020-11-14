//
// Created by Xudong Zhang on 2019-03-13.
//

#ifndef NICKPFTRACKING_RGBMPI_MINI_MYSET_H
#define NICKPFTRACKING_RGBMPI_MINI_MYSET_H

#include <algorithm>    // std::min_element, std::max_element.
#include "particleFilter2D.h"
#include "mpi.h"

class route{
public:
    int sourceId;
    int sParticleId;
    int NumtoSend;

    int desId;
    int dParticleId;
    int NumtoRec;
public:
    route(): sourceId(0), sParticleId(0), NumtoSend(0), desId(0), dParticleId(0), NumtoRec(0){}
    route(int a, int b, int c, int d, int e, int f)
            : sourceId(a), sParticleId(b), NumtoSend(c), desId(d), dParticleId(e), NumtoRec(f){}
};


class t1Piece{
public:
    int PUid;
    int Npars;
    int ParIds;
    t1Piece(): PUid(0), Npars(0), ParIds(0){}
    t1Piece(int a, int b, int c): PUid(a), Npars(b), ParIds(c){}
};

class t2Piece{
public:
    int PUid;
    int Npars;
    std::vector<int> ParIds;
    t2Piece(): PUid(0), Npars(0){}
    t2Piece(int a, int b, std::vector<int> c): PUid(a), Npars(b), ParIds(c){}

};

void createParEntryidVec(const int s, const int N, std::vector<int>& resultVec){

//    std::vector<int> tempVec;
//    for (int t : a) {
//        tempVec.push_back(t);
//    }
//    const int s = tempVec.size();
    for (int i = 0; i < N; i++) {
        if(i<s)
            continue;
        resultVec.push_back(i);
    }


//    std::set<int> tempSet;
//    std::vector<int> tempVec(N,0);
//    const int numExistedpar = a.size();
//
//    for (int t : a) {
//            tempSet.insert(t);
//    }
////    std::cout<<"Pass 1.0"<<std::endl;
//    for (int i = 0; i < N; i++) {
//        tempVec[i] = i;
//    }
//    std::vector<int>::iterator it;
//    it=std::set_symmetric_difference (tempSet.begin(), tempSet.end(),
//            tempVec.begin(), tempVec.end(), resultVec.begin());
//
//    resultVec.resize(it-resultVec.begin());
//    std::cout<<"-------******--------"<<std::endl;
//    for (int i = 0, ter = resultVec.size(); i < ter; i++) {
//        std::cout<<resultVec[i]<<", ";
//    }
//    std::cout<<std::endl;
//    std::cout<<"--------------------"<<std::endl;
//
//
//    resultVec.erase (resultVec.begin(),resultVec.begin()+numExistedpar);
//    std::cout<<"--------------------"<<std::endl;
//    for (int i = 0, ter = resultVec.size(); i < ter; i++) {
//        std::cout<<resultVec[i]<<", ";
//    }
//    std::cout<<std::endl;
//    std::cout<<"-------******--------"<<std::endl;

}

bool t1Sorter(const t1Piece& lhs, const t1Piece& rhs) {
    if(lhs.Npars != rhs.Npars)
        return lhs.Npars > rhs.Npars;
    else
        return lhs.PUid < rhs.PUid;
}


bool t2Sorter(const t2Piece& lhs, const t2Piece& rhs) {
    if(lhs.Npars != rhs.Npars)
        return lhs.Npars > rhs.Npars;
    else
        return lhs.PUid < rhs.PUid;
}

bool puidSorter(const t1Piece& lhs, const t1Piece& rhs) {
    if(lhs.PUid != rhs.PUid)
        return lhs.PUid < rhs.PUid;
    else
        return lhs.ParIds < rhs.ParIds;
}

void showPart(const int& numProcs, const int& myRank, std::vector<std::pair<int, int> > particle, const int& index){
    for (int i = 0; i < numProcs; i++) {
        if(i == myRank) {
            std::cout << "----------- "<< index <<"-----------" << std::endl;
            for (int i = 0; i < particle.size(); i++) {
                std::cout << myRank << " -> particle: " << particle[i].first << ", "
                          << particle[i].second << std::endl;
            }
            std::cout << "----------------------" << std::endl;
        }
    }

}

void showt1Tab(std::vector<t1Piece> t1){
    std::cout << "========= checking ==== tracker.t1 =================" << std::endl;
    for (auto t : t1) {
        std::cout << "PUid: " << t.PUid << ", ParIds: "<< t.ParIds<<", Num_particles: " << t.Npars<< std::endl;
    }
}

void showt2Tab(std::vector<t2Piece> t2){
    std::cout << "========= checking ==== tracker.t2 =================" << std::endl;
    for (auto t : t2) {
        std::cout << "PUid: " << t.PUid << ", Num_particles: " << t.Npars<< ", Vec: ";
        for (auto idx : t.ParIds) {
            std::cout << idx <<", ";
        }
        std::cout << std::endl;
    }
}


void showtSTab(std::vector<std::vector<int> > s){
    std::cout << "========= checking ==== s from ==== goodParVec =================" << std::endl;
    for (int i = 0, ter = s.size(); i < ter; i++) {
        std::cout << i << " -> N of good parts : " << s[i].size() << std::endl;
        for (auto t : s[i]) {
            std::cout << "PU: " << i << ", particleId: " << t << std::endl;
        }
        std::cout << std::endl;
    }

}


void showSch(std::vector<route> Sch, const int myRank){
    std::cout<< "---------- myRank "<< myRank <<" ----------"<< std::endl;
    for(auto t: Sch)
    std::cout << "sourceId: " << t.sourceId << ", sParticleId: " << t.sParticleId
    <<", NumtoSend: " << t.NumtoSend << ", desId: " << t.desId
    <<", dParticleId: " << t.dParticleId << ", NumtoRec: " << t.NumtoRec << std::endl;
    std::cout<< "====================="<< std::endl;
}

void showGoodParVec(std::vector<std::vector<int> > goodParVec){
    for (int i = 0, ter = goodParVec.size(); i < ter; i++) {
        std::cout << i << " -> # of good parts : " << goodParVec[i].size() << std::endl;
        for (auto t : goodParVec[i]) {
            std::cout << "PU: " << i << ", particleId: " << t << std::endl;
        }
        std::cout << "-----------------------" << std::endl;
    }
}

int checkv1(std::vector<int> v1){
    int result = 0;
    for(int i=0, ter=v1.size(); i<ter; i++){
        result += v1[i];
    }
    return result;
}

int checkgoodParVec(std::vector<std::vector<int> > goodParVec){
    int result = 0;
    for(int i=0, ter=goodParVec.size(); i<ter; i++){
        result += goodParVec[i].size();
    }
    return result;
}



void Histogram(std::vector<int> oriVec, std::vector< std::pair<int,int> >& results){
//    oriVec.clear();
//    oriVec.push_back(13);
//    oriVec.push_back(3);
//    oriVec.push_back(3);
//    oriVec.push_back(6);
//    oriVec.push_back(4);
//    oriVec.push_back(4);
//    oriVec.push_back(5);
//    oriVec.push_back(8);
//    oriVec.push_back(50);

    int maxVal = *std::max_element(oriVec.begin(),oriVec.end());

    std::vector<int> temVec(maxVal+1, 0);

    for(int i=0, ter=oriVec.size(); i<ter; i++){
        temVec[oriVec[i]]++;
    }

    for(int i=0, ter=temVec.size(); i<ter; i++){
        if(0 != temVec[i]){
            results.push_back(std::make_pair(i,temVec[i]));
        }
    }
//    std::cout << "----------------------" << std::endl;
//    for(int i=0, ter=results.size(); i<ter; i++){
//        std::cout<<"Parid: "<<results[i].first<<", #: "<<results[i].second<<std::endl;
//    }
//    std::cout << "----------------------" << std::endl;
}

void updateTablesVecs(
        const int k,
        const int j,
        std::vector<t1Piece> & t1,
        std::vector<t2Piece>& t2,
        std::vector<int>& v1,
        std::vector<bool>& v2,
        const int numParTrns,
        const int N) {
    // at row k.
    t1[k].Npars -= numParTrns;
    v1[t1[k].PUid] -= numParTrns;

    // at row j.
    t2[j].Npars -= numParTrns;  // needed number of particles decreased by numParTrns.
    v1[t2[j].PUid] += numParTrns;

    for(int i=0, ter=v2.size(); i<ter; i++){
        if(N == v1[i])  // get enough particles, so skip this row in t1 or t2.
            v2[i] = false;
//        if(0 == v1[i])    // receive all the particles.
//            v2[i] = false;
    }

    sort(t1.begin(), t1.end(), t1Sorter);
    sort(t2.begin(), t2.end(), t2Sorter);

}

#endif //NICKPFTRACKING_RGBMPI_MINI_MYSET_H
