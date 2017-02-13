clear all
close all

outputFilename = 'grapTimeIterations.pdf';
outputFilenamepng = 'grapTimeIterations.png';
SAVE_TO_FILE = true;
%{
sets={{'Tsiligirides Set 3','../data/resultsDiffResolutionsP3/results.txt','Set 3, T_{max}= 55','+-r'},...
      {'Chao diamond-shaped Set','../data/resultsDiffResolutions64/results.txt','Set 64, T_{max}= 55','o-.b'},...
      {'Chao squared-shaped Set','../data/resultsDiffResolutions66/results.txt','Set 66, T_{max}= 65','*:g'}};

sets={{'Tsiligirides Set 3','../data/resultsDiffResolutionsP3/results_r1.2.txt','Set 3, T_{max}= 55','+-r'},...
      {'Chao diamond-shaped Set','../data/resultsDiffResolutions64/results_r1.2.txt','Set 64, T_{max}= 55','o-.b'},...
      {'Chao squared-shaped Set','../data/resultsDiffResolutions66/results_r1.2.txt','Set 66, T_{max}= 65','*:g'}};
%}
%{
sets={{'Tsiligirides Set 3','../data/resultsRVNSDiffResolution/resultsDiffResolutionsP3/results.log','Set 3, T_{max}= 55','-r'},...
      {'Chao diamond-shaped Set','../data/resultsRVNSDiffResolution/resultsDiffResolutions64/results.log','Set 64, T_{max}= 55','-.b'},...
      {'Chao squared-shaped Set','../data/resultsRVNSDiffResolution/resultsDiffResolutions66/results.log','Set 66, T_{max}= 65',':g'}};
%}

  sets={{'Tsiligirides Set 1','../data/metacentrum/resultsAll.log','Set 1','-r'},...
       {'Tsiligirides Set 2','../data/metacentrum/resultsAll.log','Set 2','-.g'},...
       {'Tsiligirides Set 3','../data/metacentrum/resultsAll.log','Set 3',':b'},...
       {'Chao diamond-shaped Set','../data/metacentrum/resultsAll.log','Set 64','-k'},...
       {'Chao squared-shaped Set','../data/metacentrum/resultsAll.log','Set 66','-.m'}};

  
 signs = ['+' 'o' 's' '*' 'd' 'x' '^' 'v' 'p' 'h' '>' '<']  
 signid = 1; 
 
wantedResolution = 16;
wantedRadius = 0.7;
maxRewardAll = 0;
minRewardAll = 1000000000000000000;
fig = figure(1)
widthFig = 520
heightFig = 280
widthFig = 800
heightFig = 400
set(fig,'Position', [0 0 widthFig heightFig]);
hold on
legendsRew={};
legendsTime={};
%ylabel('Collected reward - R')
ax1 = subplot(2,2,1,'Color','none')
ylim([0 100])
xlim([0 135])
xlabel('Budget')
title('\rho=0')

%ax1 = gca; % current axes
%set(gca, 'Color','none')
%ax1_pos = get(ax1,'Position'); % position of first axes
%set(gcf, 'Color', 'None')
%{
ax2 = axes('Position',get(ax1,'Position'),...
       'XAxisLocation','bottom',...
       'YAxisLocation','right',...
       'Color','none',...
       'XColor','k','YColor','k');
%}

%linkaxes([ax1 ax2],'x');
%ylim(ax2,[0 3000])

ax2 = subplot(2,2,3,'Color','none')
ylim([0 5000])
xlim([0 135])
xlabel('Budget')

ax3 = subplot(2,2,2,'Color','none')
ylim([0 800])
xlim([0 135])
xlabel('Budget')
title('\rho=0.7')

ax4 = subplot(2,2,4,'Color','none')
ylim([0 5000])
xlim([0 135])
xlabel('Budget')

rewardPlots = [];
timePlots = [];
legendsCT = {};
legendsRew = {};
for setID=1:size(sets,2)
    setID
    set = sets{:,setID};
    resultID = 1;
    comparisonStruct = struct;
    setName = set{1};
    resultsMy = parseLog(set{2});
    plotLegend = set{3};
    plotSyle = set{4};

    %legends = {legends{:},plotLegend }
    legendReward = strcat({''},plotLegend);
    legendCT = strcat('Number of iterations,',{' '},plotLegend );
    legendsRew = {legendsRew{:},legendReward{1}};
    legendsCT = {legendsCT{:},legendCT{1}};
    %legends = {legends{:},legendReward{1},legendCT{1}}
    myResultIDs = find(ismember({resultsMy(:).NAME},{setName}));
    resultsProblem = resultsMy(myResultIDs);
    %resolutions =  unique([resultsMy(myResultIDs).RESOLUTION]);   
    budegets =  unique([resultsMy(myResultIDs).BUDGET])  ;
    radiuses = unique([resultsMy(myResultIDs).RADIUS]);

   for actualBudget = budegets
     
        sameBudgetIDs = find([resultsProblem(:).BUDGET] == actualBudget & [resultsProblem(:).RESOLUTION] == wantedResolution & [resultsProblem(:).RADIUS] == wantedRadius);    
        %size(sameBudgetIDs)
        
        rewardAll = [resultsProblem(sameBudgetIDs).REWARDS];
        maxRewardAll = max(rewardAll);
        minRewardAll = min(rewardAll);
        rew_r1_0_mean_ctime_last_imp = mean([resultsProblem(sameBudgetIDs).CTIME_LAST_IMPR]);
        rew_r1_0_mean_ctime = mean([resultsProblem(sameBudgetIDs).CTIME]);
        iteration = round(mean([resultsProblem(sameBudgetIDs).NUM_ITERS_LAST_IMPR]));
        resolution  = round(mean([resultsProblem(sameBudgetIDs).RESOLUTION]));
        %mean(A)
       
        %add budget
        comparisonStruct(resultID).BUDGET = actualBudget;
        comparisonStruct(resultID).RESOLUTION = resolution;
       
        comparisonStruct(resultID).MY_REWARD_R_1_0_MAX = maxRewardAll ;
        comparisonStruct(resultID).CTIME_LAST_IMPR = rew_r1_0_mean_ctime_last_imp/1000 ;
        comparisonStruct(resultID).CTIME = rew_r1_0_mean_ctime/1000 ;
        comparisonStruct(resultID).NUM_ITERS_LAST_IMPR = iteration ;
        %comparisonStruct(resultID).MY_REWARD_R_0_0_MEAN = rew_r0_0_mean;
        %comparisonStruct(resultID).MY_REWARD_R_0_5_MEAN = rew_r0_5_mean;
        %comparisonStruct(resultID).MY_REWARD_R_1_0_MEAN = rew_r1_0_mean;
        
        
        %comparisonStruct(resultID).CTIME_S_R_0_0 = rew_r0_0_mean_ctime;
        %comparisonStruct(resultID).CTIME_S_R_0_5 = rew_r0_5_mean_ctime;
        %comparisonStruct(resultID).CTIME_S_R_1_0 = rew_r1_0_mean_ctime;

        resultID = resultID + 1;
   end
   
    comparisonStructEop = struct;
     for actualBudget = budegets
     
        sameBudgetIDs = find([resultsProblem(:).BUDGET] == actualBudget & [resultsProblem(:).RESOLUTION] == 1 & [resultsProblem(:).RADIUS] == 0);    
        %size(sameBudgetIDs)
        
        rewardAll = [resultsProblem(sameBudgetIDs).REWARDS];
        maxRewardAll = max(rewardAll);
        minRewardAll = min(rewardAll);
        rew_r1_0_mean_ctime_last_imp = mean([resultsProblem(sameBudgetIDs).CTIME_LAST_IMPR]);
        rew_r1_0_mean_ctime = mean([resultsProblem(sameBudgetIDs).CTIME]);
        iteration = round(mean([resultsProblem(sameBudgetIDs).NUM_ITERS_LAST_IMPR]));
        resolution  = round(mean([resultsProblem(sameBudgetIDs).RESOLUTION]));
        %mean(A)
       
        %add budget
        comparisonStructEop(resultID).BUDGET = actualBudget;
        comparisonStructEop(resultID).RESOLUTION = resolution;
       
        comparisonStructEop(resultID).MY_REWARD_R_1_0_MAX = maxRewardAll ;
        comparisonStructEop(resultID).CTIME_LAST_IMPR = rew_r1_0_mean_ctime_last_imp/1000 ;
        comparisonStructEop(resultID).CTIME = rew_r1_0_mean_ctime/1000 ;
        comparisonStructEop(resultID).NUM_ITERS_LAST_IMPR = iteration ;
        %comparisonStruct(resultID).MY_REWARD_R_0_0_MEAN = rew_r0_0_mean;
        %comparisonStruct(resultID).MY_REWARD_R_0_5_MEAN = rew_r0_5_mean;
        %comparisonStruct(resultID).MY_REWARD_R_1_0_MEAN = rew_r1_0_mean;
        
        
        %comparisonStruct(resultID).CTIME_S_R_0_0 = rew_r0_0_mean_ctime;
        %comparisonStruct(resultID).CTIME_S_R_0_5 = rew_r0_5_mean_ctime;
        %comparisonStruct(resultID).CTIME_S_R_1_0 = rew_r1_0_mean_ctime;

        resultID = resultID + 1;
   end
  
   
  % [AX,hndlRew,hndlTime] = plotyy([comparisonStruct(:).RESOLUTION],[comparisonStruct(:).MY_REWARD_R_1_0_MAX],[comparisonStruct(:).RESOLUTION],[comparisonStruct(:).CTIME]);
  subplot(ax1); 
  hold on
  hndlRew = plot([comparisonStructEop(:).BUDGET],[comparisonStructEop(:).CTIME_LAST_IMPR],strcat(signs(signid*2-1),plotSyle),'Parent',ax1);
 
  subplot(ax2); 
  hold on
  hndlTime = plot([comparisonStructEop(:).BUDGET],[comparisonStructEop(:).NUM_ITERS_LAST_IMPR],strcat(signs(signid*2-1),plotSyle),'Parent',ax2);
  
  subplot(ax3); 
  hold on
  hndlRew = plot([comparisonStruct(:).BUDGET],[comparisonStruct(:).CTIME_LAST_IMPR],strcat(signs(signid*2-1),plotSyle),'Parent',ax3);
 
  subplot(ax4); 
  hold on
  hndlTime = plot([comparisonStruct(:).BUDGET],[comparisonStruct(:).NUM_ITERS_LAST_IMPR],strcat(signs(signid*2-1),plotSyle),'Parent',ax4);
   
  timePlots = [timePlots , hndlTime]; 
  %rewardPlots = [rewardPlots , hndlRew]; 
  signid= signid +1;
end

%axes(ax1);


%columnlegend(6,legends,'NorthWest');
%gridLegend(rewardPlots,3,legends)
%hdl,nCols,gKey
%columnlegend(3,legends,'NorthWest')
%legend(ax1,legends,'Location','southeast');
%legend(ax2,legends,'Location','southeast');
%legend('boxoff');
ylabel(ax1,'Computational time [s]')
ylabel(ax2,'Iterations')
ylabel(ax3,'Computational time [s]')
ylabel(ax4,'Iterations')

tightfig
%offsetLegend = [0.022 , -0.04 , 0 , 0]
offsetLegendRight = [0.01 , -0.068 , 0 , 0]
offsetLegendLeft = [0.026 , -0.068 , 0 , 0]

subplot(ax1); 
hold on
hLegend1 = legend([rewardPlots],{legendsRew{:}},'location','northwest');
hLegend1.Color = 'none'
hLegend1.Box = 'Off'
hLegend1.Position = hLegend1.Position - offsetLegendLeft

subplot(ax2); 
hold on
hLegend2 = legend([rewardPlots],{legendsRew{:}},'location','northwest');
hLegend2.Color = 'none'
hLegend2.Box = 'Off'
hLegend2.Position = hLegend2.Position - offsetLegendLeft

subplot(ax3); 
hold on
hLegend3 = legend([rewardPlots],{legendsRew{:}},'location','northeast');
hLegend3.Color = 'none'
hLegend3.Box = 'Off'
hLegend3.Position = hLegend3.Position - offsetLegendRight

subplot(ax4); 
hold on
hLegend4 = legend([rewardPlots],{legendsRew{:}},'location','northeast');
hLegend4.Color = 'none'
hLegend4.Box = 'Off'
hLegend4.Position = hLegend4.Position - offsetLegendRight

%[legend_h,object_h,plot_h,text_strings] = columnlegend(2,[rewardPlots],{legendsRew{:}},'northoutside','boxon');
%legend_h.set('Position',[0.15 0.88 0.8 0.03])
%legend(rewardPlots,legends,'location','northoutside','Orientation','horizontal')
if SAVE_TO_FILE
save2pdf(outputFilename,fig);
print2im(outputFilenamepng, fig,'-alpha');
end






