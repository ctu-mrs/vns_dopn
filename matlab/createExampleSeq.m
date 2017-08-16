close all
clear all

RESULT_PATH='../paths/*.my';
PATH_FILENAME1='result_r_0.0_p_64_b_050_run_2-sampled-path.txt';
PATH_FILENAME2='result_r_1.3_p_64_b_050_run_5-sampled-path.txt';
TARGETS_FILENAME='targets_p_64.txt';
SAVE=true;
output_filename = 'planDOPExample.pdf'
output_filenamepng = 'planDOPExample.png'

pathFileName1=strcat(RESULT_PATH,PATH_FILENAME1);

targetsFileName=strcat('./',TARGETS_FILENAME)



targets=csvread(targetsFileName);
fig = figure(1);
files = dir(RESULT_PATH);


%iptsetpref('ImshowBorder','tight');

widthFig = 500
heightFig = 500
set(fig,'Position', [0 0 widthFig heightFig]);
hold on
colormap jet
targScatter = scatter(targets(:,2),targets(:,3), [], targets(:,4), 'filled','MarkerEdgeColor',[0 0 0]);


offset =2;
targScatter2 = scatter(targets(:,2),targets(:,3), [], targets(:,4), 'filled','MarkerEdgeColor',[0 0 0]);
xlim([-7.5 , 7.5])
ylim([-9 , 9])

%xlabel(cb, 'reward')
%plot(targets(:,2),targets(:,3),'o');

%scatter(pathX,pathY, size, concentration);
%set(gca,'CLim',[0 1]);
cb = colorbar('south')
set(cb, 'Position', [0.18 0.004 0.64 0.04]);
rewardTitle = title(cb,'reward');
set(gca,'LooseInset',get(gca,'TightInset'));
set(rewardTitle,'FontSize',12);
%axis equal
axis off
scale = 0.2;
width = widthFig*scale;
height = heightFig*scale;

%{
    offsetX = -3.4
    offsetXRight = -1.7
    offsetYDown = -0.6
%}
offsetX = 0
offsetXRight = 0
offsetYDown = 0

maxreward= -10;
maxpath = [];
default_text_pos = [-7.5 9]
initialization = 50;
max = 30055;
num_images = 1000 - initialization;
step = round(max/num_images)
for filenum = [1:initialization , (initialization+1):step:max]
    fprintf('%d/%d\n',filenum,max);
    hold on
    pathFileName1 = strcat('../paths/pathRobot',num2str(filenum),'.my');
    path1=csvread(pathFileName1);
    rewardhFileName1 = strcat('../paths/pathRobot',num2str(filenum),'.reward');
    rewlength=csvread(rewardhFileName1);
    reward = rewlength(1);
    if(reward > maxreward)
        maxreward=reward;
        maxpath = path1;
    end
    length = rewlength(2);
    pathplot = plot(path1(:,1),path1(:,2),'k','LineWidth',2);
    pathplotbest = plot(maxpath(:,1),maxpath(:,2),'g','LineWidth',2);
    reward_text=text(default_text_pos(1),default_text_pos(2),strcat('\fontsize{16}{0}\selectfont{actual reward = ',num2str(reward),'}'),'Interpreter','latex');
    reward_text_max=text(default_text_pos(1)+7,default_text_pos(2),strcat('\fontsize{16}{0}\selectfont{maximal reward = ',num2str(maxreward),'}'),'Interpreter','latex','Color','g');
    xlim([-7.5 , 7.5])
    ylim([-9 , 9])

    if SAVE
    fig.PaperPositionMode = 'auto';
    
    %print(fig,'MySavedPlot','-dpng')
    print(fig,strcat('seq/im',sprintf('%05d',filenum),'.jpg'), '-djpeg','-r0');
    %print2im(strcat('seq/im',sprintf('%05d',filenum),'.jpg'));
    end
delete(reward_text);
delete(reward_text_max);
delete(pathplot);
delete(pathplotbest);
drawnow;
    
end




%set(fig, 'PaperPosition', [0 0 width height]); %Position plot at left hand corner with width 5 and height 5.
%set(fig, 'PaperSize', [width height]); %Set the paper to have width 5 and height 5.
%print(,'-dpdf',filename);
%print(fig,'-djpeg', strcat(filename,'.jpeg'), '-r600');
%set(gcf, 'color', 'none');
%export_fig(strcat(filename,'.png'), '-r600')
