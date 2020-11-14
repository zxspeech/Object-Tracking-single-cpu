//
// Created by Nick Zhang on 2019-03-17.
//

#ifndef NICKPFTRACKING_RGBMPI_MINI_READGT_H
#define NICKPFTRACKING_RGBMPI_MINI_READGT_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>


void readGT(std::string filename, std::vector<std::pair<int, int> >& GT){
    int x;
    int cout = 0;
    std::pair<int, int> tempPair;
    std::ifstream inFile;
    inFile.open(filename);
    if (!inFile) {
        std::cout << "Error: Unable to open file";
        exit(1);
    }

    while (inFile >> x) {

        if(0 == cout%2){
            tempPair = std::make_pair(0,0);
            tempPair.first = x;
        }
        else{
            tempPair.second = x;
            GT.push_back(tempPair);
        }
        cout++;

    }

    inFile.close();

}

void showGT(std::vector<std::pair<int, int> >* GT){
    for(std::pair<int, int> t : *GT)
        std::cout << t.first <<", " << t.second << std::endl;

}


#endif //NICKPFTRACKING_RGBMPI_MINI_READGT_H
