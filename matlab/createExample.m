close all
clear all

% Change default axes fonts.
set(0,'DefaultAxesFontName', 'Times New Roman')
set(0,'DefaultAxesFontSize', 14)

% Change default text fonts.
set(0,'DefaultTextFontname', 'Times New Roman')
set(0,'DefaultTextFontSize', 14)


grey = [0.4,0.4,0.4];
yellow = [0.9 0.9 0];

RESULT_PATH='../localTests/results/';

sampledPathFile = 'sampled-path.txt';
resultLogFile = 'results.log';

PATHNAME1='resultsSet66_nr0.0_r0.0_run_4/';
PATHNAME2='resultsSet66_nr0.5_r0.0_run_4/';
PATHNAME3='resultsSet66_nr0.0_r1.0_run_4/';
PATHNAME4='resultsSet66_nr0.5_r1.0_run_4/'

TARGETS_FILENAME='resultsSet66_nr0.0_r1.0_run_4/targets.txt';
SAVE=true;
output_filename = 'planDOPNExample.pdf'
output_filenamepng = 'planDOPNExample.png'

pathFileName1=strcat(RESULT_PATH,PATHNAME1,sampledPathFile)
pathFileName2=strcat(RESULT_PATH,PATHNAME2,sampledPathFile)
pathFileName3=strcat(RESULT_PATH,PATHNAME3,sampledPathFile)
pathFileName4=strcat(RESULT_PATH,PATHNAME4,sampledPathFile)
targetsFileName=strcat(RESULT_PATH,TARGETS_FILENAME)

result_path1 = parseLog(strcat(RESULT_PATH,PATHNAME1,resultLogFile));
result_path2 = parseLog(strcat(RESULT_PATH,PATHNAME2,resultLogFile));
result_path3 = parseLog(strcat(RESULT_PATH,PATHNAME3,resultLogFile));
result_path4 = parseLog(strcat(RESULT_PATH,PATHNAME4,resultLogFile));


targets=csvread(targetsFileName);


path1=csvread(pathFileName1);
path2=csvread(pathFileName2);
path3=csvread(pathFileName3);
path4=csvread(pathFileName4);

iptsetpref('ImshowBorder','tight');
fig = figure(1);
widthratio = 1.0
heightFig = 600
widthFig = heightFig*widthratio
set(fig,'Position', [0 0 widthFig heightFig]);
hold on
colormap jet
targScatter = scatter(targets(:,2),targets(:,3), [], targets(:,4), 'filled','MarkerEdgeColor',[0 0 0]);
plot(path1(:,1),path1(:,2),'k');

offsetx = 4 + max(targets(:,2)) - min(targets(:,2));
offsety = -4 - ( max(targets(:,3)) - min(targets(:,3)));


additionalwidth = 2
radius = 0.5
sca = scatter(targets(3:end,2)+ones(size(targets,1)-2,1)*offsetx,targets(3:end,3),5,'MarkerFaceColor',yellow,'MarkerEdgeColor',grey,'LineWidth',0.2)
currentunits = get(gca,'Units');
set(gca, 'Units', 'Points');
axpos = get(gca,'Position');
set(gca, 'Units', currentunits);
markerWidth = (2*radius)/diff(xlim)*axpos(3)+additionalwidth; % Calculate Marker width in points
set(sca, 'Marker', 'o')
set(sca, 'SizeData', markerWidth^2)
%circles(targets(:,2)+ones(size(targets,1),1)*offsetx,targets(:,3),ones(size(targets,1),1)*radius,'edgecolor',grey,'facecolor',yellow);

radius = 0.5
sca = scatter(targets(3:end,2)+ones(size(targets,1)-2,1)*offsetx,targets(3:end,3)+ones(size(targets,1)-2,1)*offsety,5,'MarkerFaceColor',yellow,'MarkerEdgeColor',grey,'LineWidth',0.2)
currentunits = get(gca,'Units');
set(gca, 'Units', 'Points');
axpos = get(gca,'Position');
set(gca, 'Units', currentunits);
markerWidth = (2*radius)/diff(xlim)*axpos(3)+additionalwidth; % Calculate Marker width in points
set(sca, 'Marker', 'o')
set(sca, 'SizeData', markerWidth^2)
%circles(targets(:,2)+ones(size(targets,1),1)*offsetx,targets(:,3) +ones(size(targets,1),1)*offsety,ones(size(targets,1),1)*radius,'edgecolor',grey,'facecolor',yellow);

targScatter2 = scatter(targets(:,2)+ones(size(targets,1),1)*offsetx,targets(:,3), [], targets(:,4), 'filled','MarkerEdgeColor',[0 0 0]);
plot(path2(:,1)+ones(size(path2,1),1)*offsetx,path2(:,2),'k');



targScatter3 = scatter(targets(:,2),targets(:,3)+ones(size(targets,1),1)*offsety, [], targets(:,4), 'filled','MarkerEdgeColor',[0 0 0]);
plot(path3(:,1),path3(:,2)+ones(size(path3,1),1)*offsety,'k');

targScatter4 = scatter(targets(:,2)+ones(size(targets,1),1)*offsetx,targets(:,3)+ones(size(targets,1),1)*offsety, [], targets(:,4), 'filled','MarkerEdgeColor',[0 0 0]);
plot(path4(:,1)+ones(size(path4,1),1)*offsetx,path4(:,2)+ones(size(path4,1),1)*offsety,'k');



%scatter(targets(:,2)+ones(size(targets,1),1)*offsetx,targets(:,3),150,'filled', 'MarkerFaceAlpha',3/8,'MarkerFaceColor','r')         
%for i=3:size(targets,1)
%    rectangle('Position',[targets(i,2:3)-[radius-offsetx,radius],radius*2,radius*2],'Curvature',[1 1],'EdgeColor',grey,'LineWidth',1)
%end

%radius = 0.2
%for i=3:size(targets,1)
%    r = rectangle('Position',[targets(i,2:3)-[radius-offsetx,radius-offsety],radius*2,radius*2],'Curvature',[1 1],'EdgeColor',grey,'LineWidth',1)
%end

default_text_pos(1) = min(targets(:,2))+ (max(targets(:,2)) - min(targets(:,2)))/2 - 3.0;
default_text_pos(2) = max(targets(:,3)) +1.5 ;
text(default_text_pos(1),default_text_pos(2),strcat('\fontsize{12}{0}\selectfont{EOP}\fontsize{8}{0}\selectfont{',{' R='},num2str(result_path1.REWARDS),'}'),'Interpreter','latex')


text(default_text_pos(1)+offsetx,default_text_pos(2),strcat('\fontsize{12}{0}\selectfont{EOPN}\fontsize{8}{0}\selectfont{',{' R='},num2str(result_path2.REWARDS),'}'),'Interpreter','latex')


text(default_text_pos(1),default_text_pos(2)+offsety,strcat('\fontsize{12}{0}\selectfont{DOP}\fontsize{8}{0}\selectfont{',{' R='},num2str(result_path3.REWARDS),'}'),'Interpreter','latex')


text(default_text_pos(1)+offsetx,default_text_pos(2)+offsety,strcat('\fontsize{12}{0}\selectfont{DOPN}\fontsize{8}{0}\selectfont{',{' R='},num2str(result_path4.REWARDS),'}'),'Interpreter','latex')

xlim([-8 , 28])
ylim([-28.5 , 9.5])


cb = colorbar('south')
set(cb, 'Position', [0.08 0.004 0.84 0.03])
rewardTitle = title(cb,'reward')
set(gca,'LooseInset',get(gca,'TightInset')); 
set(rewardTitle,'FontSize',12)



%axis equal
axis off
set(gca, 'visible', 'off'); 
scale = 0.2;
width = widthFig*scale
height = heightFig*scale

%tightfig
%{
offsetX = -3.4
offsetXRight = -1.7
offsetYDown = -0.6
%}
offsetX = 0
offsetXRight = 0
offsetYDown = 0

tightfig

if SAVE
save2pdf(output_filename,fig,300);
%print2im(output_filenamepng, fig,'-alpha');
end
