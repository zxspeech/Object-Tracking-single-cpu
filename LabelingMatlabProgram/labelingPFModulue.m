%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Copyright by Xudong Zhang, City University of New York.
% Input: 
%   volName: file name of volume image.
%   startSlice: Starting index of 2d img. 
%   interval:
%   endSlice: 
%   
% Output: mV is array of 3d coordinates.
%         coords_list is cell with 3d coordinates.
%   The following code is to label the mitral valve.
%   1. Run the code to load the *.vol 3d image.
%   2. Select the starting point by mouse.
%   3. press "command" key on Mac, then the coordinates
%      marked by cursor will be stroed in array named mV.
%   4. The use arraw keys to move the cursor to the positon,
%       you want to record.
%   5. Repeat stpe 3 and 4, until you label all the points
%      in the 2d image.
%   6. Press "esc" key on keyboard to process the next 2d img.
%   7. The program will terminate after you label the last 2d img.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


function labelingPFModulue(inputFileName, txtFileName)
% inputFileName = './movingDucks.mov';
% txtFileName = './GT.txt';

close all;

fid=fopen(txtFileName,'w');
v = VideoReader(inputFileName);
ter = round(v.FrameRate*v.Duration);
Nof2dImg = 1;
% NofDataPoint = 1;
% for k = startSlice:interval:endSlice
% for k = 1:1:2
while hasFrame(v)
fig = figure;
set(gcf,'PaperPositionMode','auto')
set(fig, 'Position', [100 100 250 250]);

vidFrame = readFrame(v);
grayFrame = rgb2gray(vidFrame);
size(grayFrame);
imshow(grayFrame);


% Img = permute(vol(:,:,k), [2,1,3]);    %swap the X and Y.
imshow(grayFrame);



datacursormode on;
waitforbuttonpress; %waiting to select the first starting point.
% for i = 1:1
%               shg
	dcm_obj = datacursormode(fig);    %% xudong change here!!!
% 	set(dcm_obj,'DisplayStyle','datatip','SnapToDataVertex','off','Enable','on')
    set(dcm_obj,'DisplayStyle','window','SnapToDataVertex','off','Enable','on')
%     if i == 1
%         a = getCursorInfo(dcm_obj);a.Position  % Debugging here !
%     end
    
%     while true
% %         disp("----- whiting the key --------");
%         w = waitforbuttonpress;
% %         disp("----- get the key --------");
%         Key=get(gcf,'CurrentKey');
% %         disp("the key presses is: ");
% %         num2str(Key)
%         if strcmp(num2str(Key),'0')==1 || strcmp(num2str(Key),'escape')==1
% %             disp("key q was pressed!");
%             break
%         end  
%     end

    
    temp = getCursorInfo(dcm_obj);
% 	coord{i,Nof2dImg} = [temp.Position,k];
%     coord_temp{i} = temp.Position
%     a = temp.Position
    b = [temp.Position]
%     coord_temp{i} = b;
    
%     fprintf(fid, [cell2mat(coord_temp{i})  '\n']);
    fprintf(fid, '%d %d\n', b);
%     disp("==================================");
	Key=get(gcf,'CurrentKey');
%       %using strcmp for string comparison if comparison is true = 1
%       if strcmp(num2str(Key),'uparrow')==1
%       %If up arrow is pressed thrust = 1
	if strcmp(num2str(Key),'escape')==1
        break
    end 
    close all;
end
% coords{Nof2dImg} = b;
% Nof2dImg = Nof2dImg+1;
% % coord_temp = [];    % empty the coord_temp.
% close all;
% end
% disp("Finish !!!");
% close all;


%%convert 2d cell to 1d cell.
% [~,NumOfStack] = size(coords);
% pos = 1;
% for j = 1:NumOfStack
%     [~,NofSlice] = size(coords{1,j});
%     for i = 1:NofSlice
%         coords{1,j}{1,i}
%         pos
%         coords_list{pos} = coords{1,j}{1,i};
%         
%         pos = pos+1;
%     end
% end


%%
% [~,NumOfDataPoints] = size(coords);
% mV = zeros(NumOfDataPoints,2);
% for j = 1:NumOfDataPoints
%     mV(j,:) = coords{1,j}{1,1};
% end
fclose(fid)
% size(mV)



