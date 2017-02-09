close all
clear all

% Change default axes fonts.
set(0,'DefaultAxesFontName', 'Times New Roman')
set(0,'DefaultAxesFontSize', 14)

% Change default text fonts.
set(0,'DefaultTextFontname', 'Times New Roman')
set(0,'DefaultTextFontSize', 14)

RESULT_PATH='../localTests/results/';
PATH_FILENAME1='resultsSet66_nr0.0_r1.0_run_4/sampled-path.txt';
PATH_FILENAME2='resultsSet66_nr0.0_r1.0_run_4/sampled-path.txt';
TARGETS_FILENAME='resultsSet66_nr0.0_r1.0_run_4/targets.txt';
SAVE=true;
output_filename = 'planDOPExample.pdf'
output_filenamepng = 'planDOPExample.png'

pathFileName1=strcat(RESULT_PATH,PATH_FILENAME1)
pathFileName2=strcat(RESULT_PATH,PATH_FILENAME2)
targetsFileName=strcat(RESULT_PATH,TARGETS_FILENAME)

targets=csvread(targetsFileName);


path1=csvread(pathFileName1);
path2=csvread(pathFileName2);

iptsetpref('ImshowBorder','tight');
fig = figure(1);
widthFig = 600
heightFig = 320
set(fig,'Position', [0 0 widthFig heightFig]);
hold on
colormap jet
targScatter = scatter(targets(:,2),targets(:,3), [], targets(:,4), 'filled','MarkerEdgeColor',[0 0 0]);
plot(path1(:,1),path1(:,2),'k');

offset =2 + max(targets(:,2)) - min(targets(:,2));
targScatter2 = scatter(targets(:,2)+ones(size(targets,1),1)*offset,targets(:,3), [], targets(:,4), 'filled','MarkerEdgeColor',[0 0 0]);
plot(path2(:,1)+ones(size(path2,1),1)*offset,path2(:,2),'k');
xlim([-7 , 23.5])
ylim([-8 , 7.5])

%xlabel(cb, 'reward')
%plot(targets(:,2),targets(:,3),'o');

%scatter(pathX,pathY, size, concentration);
%set(gca,'CLim',[0 1]);
cb = colorbar('south')
set(cb, 'Position', [0.10 0.004 0.84 0.07])
rewardTitle = title(cb,'reward')
set(gca,'LooseInset',get(gca,'TightInset')); 
set(rewardTitle,'FontSize',12)
%axis equal
axis off
scale = 0.2;
width = widthFig*scale
height = heightFig*scale

%{
offsetX = -3.4
offsetXRight = -1.7
offsetYDown = -0.6
%}
offsetX = 0
offsetXRight = 0
offsetYDown = 0
if SAVE
save2pdf(output_filename,fig);
print2im(output_filenamepng, fig,'-alpha');
end
%set(fig, 'PaperPosition', [0 0 width height]); %Position plot at left hand corner with width 5 and height 5.
%set(fig, 'PaperSize', [width height]); %Set the paper to have width 5 and height 5.
%print(,'-dpdf',filename);
%print(fig,'-djpeg', strcat(filename,'.jpeg'), '-r600');
%set(gcf, 'color', 'none');
%export_fig(strcat(filename,'.png'), '-r600')
