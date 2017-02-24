close all
clear all

filename = 'planMBZIRC.png'
outputImage = 'planMBZIRCwBackgroundFullRes.png'
save = true


FLYDATA = '../data/trajectories-2-robert/2016-06-29-13-49-20/flights/f-1.csv'
TARGETS = '../data/trajectories-2-robert/targets2.txt'
%PLAN_DOP = '../data/mbzirc/sampled-path.txt'
%PLAN_EOP = '../data/mbzirc/trajectories/path2/sampled-path.txt'

BACKGROUND = '../data/background.png'
background = imread(BACKGROUND);


data=csvread(FLYDATA,9,0);
planDOP=data(:,5:7);
%planEOP=csvread(PLAN_EOP,0,0);
targets=csvread(TARGETS,0,0);
targets=targets(1:(size(targets,1)-1),1:4)


start = [planDOP(1,1),planDOP(1,2),1]
stop = [planDOP(end,1),planDOP(end,2),2]
%new_odom=data(:,1:7);
%mpc_tracker_current_pose = data(:,8:10);
%mpc_tracker_current_pose_setpoint_nuc2 = data(:,11:13);
%actual_cross_pose = data(:,14:15);
%estimate_cross_pose = data(:,16:19);
%control = data(:,20:24);
%rtk_gps_position = data(:,25:28);
%precise_odom=data(:,29:35);
%activeRTKData = find( [rtk_gps_position(:,1)] ~= 10000 );
%rtk_gps_position_active = rtk_gps_position(activeRTKData,:);

%firstStart= 1431;
%firstStop=2646;
%firstPath=precise_odom(firstStart:firstStop,:);

%secondSart=3662;
%secondStop=4865;
%secondPath=precise_odom(secondSart:secondStop,:);

thirdSart=6008;
thirdStop=7209;
thirdPath=data(:,2:3);

path1PlotStyle='r--'
planPath1Style='r'
path2PlotStyle='k--'
planPath2Style='k'
path3PlotStyle='b--'
planPath3Style='b'
lw = 2;

scale = 2.4;
toX = 0;
toY = 210;

limy=[-10 , 58]

theta = pi*12/180

fig = figure('Color',[1 1 1])
widthFig = round(600*scale)
heightFig = round(400*scale)
%set(fig, 'Visible', 'off')

set(fig,'Position', [0 0 widthFig heightFig]);

%set(gcf,'renderer','OpenGL'); % Sets renderer to OpenGL
%set(gcf,'color','none');
%set(gcf,'color','none');
%set(gca,'color','none') 
set(gca, 'Color', 'none');

%img = imread(BACKGROUND);
%imshow(img);
%image([0 80], [0 50], img);
hold on
colormap jet

R = [cos(theta) -sin(theta) 0; ...
     sin(theta)  cos(theta) 0; ...
             0           0  1];


targetsPositions = [targets(:,2),targets(:,3),ones(size(targets,1),1)];
targetsPositionsT = R*targetsPositions';
targetsPositions = targetsPositionsT';
targScatter = scatter(targetsPositions(:,1),targetsPositions(:,2), [], targets(:,4));

startT =  R*start';
start = startT';

stopT = R*stop';
stop = stopT';

plot(start(1),start(2),'ob','MarkerSize',10,'MarkerFaceColor',[0 0 1],'MarkerEdgeColor',[0 0 1])
plot(stop(1),stop(2),'ob','MarkerSize',10,'MarkerFaceColor',[0 0 1],'MarkerEdgeColor',[0 0 1])


%secondPathPositions = [secondPath(:,1),secondPath(:,2),ones(size(secondPath,1),1)];
%secondPathPositionsT = R*secondPathPositions';
%secondPathPositions = secondPathPositionsT';

thirdPathPositions = [thirdPath(:,1),thirdPath(:,2),ones(size(thirdPath,1),1)];
thirdPathPositionsT = R*thirdPathPositions';
thirdPathPositions = thirdPathPositionsT';

%EOP plan
%path2Plot = plot(secondPathPositions(:,1),secondPathPositions(:,2),path2PlotStyle,'LineWidth',lw);
%DOP plan
path3Plot = plot(thirdPathPositions(:,1),thirdPathPositions(:,2),path3PlotStyle,'LineWidth',lw);

%planEOPPositions = [planEOP(:,1),planEOP(:,2),ones(size(planEOP,1),1)];
%planEOPPositionsT = R*planEOPPositions';
%planEOPPositions = planEOPPositionsT';

planDOPPositions = [planDOP(:,1),planDOP(:,2),ones(size(planDOP,1),1)];
planDOPPositionsT = R*planDOPPositions';
planDOPPositions = planDOPPositionsT';

%EOP plan
%planPath2 = plot(planEOPPositions(:,1),planEOPPositions(:,2),planPath2Style,'LineWidth',lw);
%DOP plan
planPath3 = plot(planDOPPositions(:,1),planDOPPositions(:,2),planPath3Style,'LineWidth',lw);
s = 8;
currentunits = get(gca,'Units');
set(gca, 'Units', 'Points');
axpos = get(gca,'Position');
set(gca, 'Units', currentunits);
markerWidth = s/diff(xlim)*axpos(3); % Calculate Marker width in points
%set(h, 'Marker', 'square')
set(targScatter, 'SizeData', markerWidth^2)
set(targScatter, 'LineWidth', 2.5)

xlim([0 , 80])
ylim(limy)
ylabel('y[m]')
xlabel('x[m]')

cb = colorbar('south')
set(cb, 'Position', [0.1 0.13 0.8 0.05],'Color','white')
rewardTitle = title(cb,'reward')
set(gca,'LooseInset',get(gca,'TightInset')); 
set(rewardTitle,'FontSize',16,'Color','white')

legendsHandlers=[path3Plot,planPath3]
legendsTexts = {'DOP path traveled by UAV',...
                      'DOP path plan' }
%legend(legendsHandlers,legendsTexts,'Position',[0.15,0.26,0.25,0.1],'TextColor','white','FontSize',18);
%legend('boxoff');

%axis off
axis equal

ax = gca % Get handle to current axes.
ax.XColor = 'w'; % Red
ax.YColor = 'w'; % Blue



if save
[A alpha] = print2im('', fig,'-alpha');
%[A,map,transparency] = imread(filename);
%alpha = double(transparency) / 255.0;
image  = background;
sizeBackground = size(background)
sizeA = size(A)
A = im2double(A);
image = im2double(image);
%A = double(A);
%A = imrotate(A,10);
%alpha = imrotate(alpha,10);
%figure(3)
%imshow(image)


%image(toX:end,toY:end,:)=(1.0-alpha(:,:)).*image(toX:end,toY:end,:) + alpha(:,:).*A(:,:,:);

Ar = alpha(:,:).*A(:,:,1);
Ag = alpha(:,:).*A(:,:,2);
Ab = alpha(:,:).*A(:,:,3);
Aalpha = cat(3, Ar, Ag, Ab);
imageR = (ones(size(alpha))-alpha).*image((toX+1):(toX+size(A,1)),(toY+1):(toY+size(A,2)),1);
imageG = (ones(size(alpha))-alpha).*image((toX+1):(toX+size(A,1)),(toY+1):(toY+size(A,2)),2);
imageB = (ones(size(alpha))-alpha).*image((toX+1):(toX+size(A,1)),(toY+1):(toY+size(A,2)),3);
imageAlpha = cat(3, imageR, imageG, imageB);
image((toX+1):(toX+size(A,1)),(toY+1):(toY+size(A,2)),:) = Aalpha + imageAlpha;
%{
for x=1:size(A,1)
    for y=1:size(A,2)
       % if alpha(x,y) < 1.0
           image(toX+x,toY+y,:)=(1.0-alpha(x,y))*image(toX+x,toY+y,:) + alpha(x,y)*A(x,y,:);
           %image(toX+x,toY+y,:)= A(x,y,:);
       % end
    end
end
%}

fromX=toX+1;
fromY=toY+1-60;
tillX=(toX+size(A,1))+6;
tillY=(toY+size(A,2))+58;
%subimage = image(fromX:tillX,fromY:(tillY),:);
subimage = image;
%subimage = insertText(subimage,[790 330],'A','TextColor','white','FontSize',18,'BoxOpacity',0.0);
%subimage = insertText(subimage,[505 265],'B','TextColor','white','FontSize',18,'BoxOpacity',0.0);
%subimage = insertText(subimage,[325 44],'C','TextColor','white','FontSize',18,'BoxOpacity',0.0);

%figure(2)
%imshow(subimage)
imwrite(subimage, outputImage );

%save2pdf(filename,fig);
end


