clear all
close all

% Change default axes fonts.
set(0,'DefaultAxesFontName', 'Times New Roman')
set(0,'DefaultAxesFontSize', 14)

% Change default text fonts.
set(0,'DefaultTextFontname', 'Times New Roman')
set(0,'DefaultTextFontSize', 14)

outputFilename = 'graphDiffNeighResolution.pdf';
outputFilenamepng = 'graphDiffNeighResolution.png';
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

  sets={{'Tsiligirides Set 3','../data/resultsDiffNeighSampling/resultsAll.log','Set 3, T_{max}= 55','-r'},...
       {'Chao diamond-shaped Set','../data/resultsDiffNeighSampling/resultsAll.log','Set 64, T_{max}= 55','-.b'},...
       {'Chao squared-shaped Set','../data/resultsDiffNeighSampling/resultsAll.log','Set 66, T_{max}= 65',':g'}};

  
 signs = ['+' 'o' 's' '*' 'd' 'x']  
 signid = 1; 
 
maxRewardAll = 0;
minRewardAll = 1000000000000000000;
fig = figure(1)
widthFig = 520
heightFig = 280
widthFig = 800
%heightFig = 400
heightFig = 300
set(fig,'Position', [0 0 widthFig heightFig]);
hold on
legendsRew={};
legendsTime={};
%ylabel('Collected reward - R')
ylim([0 1600])
xlim([0 25])
xlabel('Neighborhood samples - o')

ax1 = gca; % current axes
set(gca, 'Color','none')
ax1_pos = get(ax1,'Position'); % position of first axes
%set(gcf, 'Color', 'None')

ax2 = axes('Position',get(ax1,'Position'),...
       'XAxisLocation','bottom',...
       'YAxisLocation','right',...
       'Color','none',...
       'XColor','k','YColor','k');

%ax2.XTick = [0]
set(ax2, 'XTick', []);
linkaxes([ax1 ax2],'x');
ylim(ax2,[0 30000])

hold on


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
legendReward = strcat('Achieved reward,',{' '},plotLegend);
legendCT = strcat('Computational Time,',{' '},plotLegend );
legendsRew = {legendsRew{:},legendReward{1}};
legendsCT = {legendsCT{:},legendCT{1}};
%legends = {legends{:},legendReward{1},legendCT{1}}
myResultIDs = find(ismember({resultsMy(:).NAME},{setName}));
neigh_resolutions =  unique([resultsMy(myResultIDs).NEIGHBORHOOD_RESOLUTION]);   
%resolutions= [ 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 20 24 28 32];

    for neigh_resolution = neigh_resolutions
        
        sameResolutionstIDs = find(ismember({resultsMy(:).NAME},{setName}) & [resultsMy(:).NEIGHBORHOOD_RESOLUTION] == neigh_resolution);
        
        rew_r1_0_max = max([resultsMy(sameResolutionstIDs).REWARDS]);
        %maxRewardAll = max(maxRewardAll,rew_r1_0_max);
        %minRewardAll = min(minRewardAll,rew_r1_0_max);
        rew_r1_0_mean_ctime_last_imp = mean([resultsMy(sameResolutionstIDs).CTIME_LAST_IMPR]);
        rew_r1_0_mean_ctime = mean([resultsMy(sameResolutionstIDs).CTIME]);
        %mean(A)
       
         %add budget
        %comparisonStruct(resultID).BUDGET = actualBudget;
        comparisonStruct(resultID).NEIGHBORHOOD_RESOLUTION = neigh_resolution;
       
        comparisonStruct(resultID).MY_REWARD_R_1_0_MAX = rew_r1_0_max ;
        comparisonStruct(resultID).CTIME_LAST_IMPR = rew_r1_0_mean_ctime_last_imp/1000 ;
        comparisonStruct(resultID).CTIME = rew_r1_0_mean_ctime/1000 ;
        %comparisonStruct(resultID).MY_REWARD_R_0_0_MEAN = rew_r0_0_mean;
        %comparisonStruct(resultID).MY_REWARD_R_0_5_MEAN = rew_r0_5_mean;
        %comparisonStruct(resultID).MY_REWARD_R_1_0_MEAN = rew_r1_0_mean;
        
        
        %comparisonStruct(resultID).CTIME_S_R_0_0 = rew_r0_0_mean_ctime;
        %comparisonStruct(resultID).CTIME_S_R_0_5 = rew_r0_5_mean_ctime;
        %comparisonStruct(resultID).CTIME_S_R_1_0 = rew_r1_0_mean_ctime;

        resultID = resultID + 1;
    end
    
  % [AX,hndlRew,hndlTime] = plotyy([comparisonStruct(:).RESOLUTION],[comparisonStruct(:).MY_REWARD_R_1_0_MAX],[comparisonStruct(:).RESOLUTION],[comparisonStruct(:).CTIME]);
   hndlRew = plot([comparisonStruct(:).NEIGHBORHOOD_RESOLUTION],[comparisonStruct(:).MY_REWARD_R_1_0_MAX],strcat(signs(signid*2-1),plotSyle),'Parent',ax1)
   
    
   hndlTime = plot([comparisonStruct(:).NEIGHBORHOOD_RESOLUTION],[comparisonStruct(:).CTIME_LAST_IMPR],strcat(signs(signid*2),plotSyle),'Parent',ax2)
   
   timePlots = [timePlots , hndlTime]; 
   rewardPlots = [rewardPlots , hndlRew]; 
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
ylabel(ax1,'Maximal collected reward - R_{max}')
ylabel(ax2,'Computational time [s]')

[legend_h,object_h,plot_h,text_strings] = columnlegend(2,[rewardPlots,timePlots],{legendsRew{:},legendsCT{:}},'northoutside','boxon');
legend_h.set('Position',[0.1 0.75 0.8 0.03])

tightfig
%legend(rewardPlots,legends,'location','northoutside','Orientation','horizontal')
if SAVE_TO_FILE
save2pdf(outputFilename,fig);
print2im(outputFilenamepng, fig,'-alpha');
end






