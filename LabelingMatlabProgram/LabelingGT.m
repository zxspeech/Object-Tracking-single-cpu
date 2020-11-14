%% Create mV_pM.mat.
clear all;
clc;
% excelFileName = './1.csv';
inputFileName = './walkingPeople_1.mov';
txtFileName = './GT_walkingPeople.txt';

% inputFileName = convertStringsToChars(strcat('./', 'atl_041_y1_t0','.vol'));

%%
labelingPFModulue(inputFileName, txtFileName);

% sheet = char('Sheet1');
% xlRange = 'E3';
% xlswrite(excelFileName,mV,sheet,xlRange);  




