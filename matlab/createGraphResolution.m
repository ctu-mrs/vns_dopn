clear all
close all

% Change default axes fonts.
set(0,'DefaultAxesFontName', 'Times New Roman')
set(0,'DefaultAxesFontSize', 14)

% Change default text fonts.
set(0,'DefaultTextFontname', 'Times New Roman')
set(0,'DefaultTextFontSize', 14)


outputFilename = 'grapDiffResolution.pdf';
outputFilenamepng = 'grapDiffResolution.png';
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

sets={{'Chao squared-shaped Set','../data/bench.som_opn/results-combined.log','Set 66, T_{max}= 65',':g'}};
  
signs = {'ro-' , 'g+-' , 's' , '*' , 'd' , 'x'}  
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
%ylim([0 1300])
%xlim([0 32])
xlabel('Neighborhood radius - m')
ylabel('reward')

%ax1 = gca; % current axes
set(gca, 'Color','none')
%ax1_pos = get(ax1,'Position'); % position of first axes
%set(gcf, 'Color', 'None')

%ax2 = axes('Position',get(ax1,'Position'),...
%       'XAxisLocation','bottom',...
%       'YAxisLocation','right',...
%       'Color','none',...
%       'XColor','k','YColor','k');

%ax2.XTick = [0]
%linkaxes([ax1 ax2],'x');
%ylim(ax2,[0 1600])

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

%legendsCT = {legendsCT{:},legendCT{1}};
%legends = {legends{:},legendReward{1},legendCT{1}}
myResultIDs = find(ismember({resultsMy(:).NAME},{setName}));
neighRadiuses =  unique([resultsMy(myResultIDs).NEIGHBORHOOD_RADIUS])  
methos = unique({resultsMy(myResultIDs).METHOD}); 
%resolutions= [ 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 20 24 28 32];
  for method = methos
    comparisonStruct = struct;
    legendReward = strcat('Achieved reward,',{' '},plotLegend,{' method '},method);
        %legendCT = strcat('Computational Time,',{' '},plotLegend );
legendsRew = {legendsRew{:},legendReward{1}};
    for neighRadius = neighRadiuses
        sameResolutionstIDs = find(ismember({resultsMy(:).NAME},{setName}) & strcmp({resultsMy(:).METHOD},method) & [resultsMy(:).NEIGHBORHOOD_RADIUS] == neighRadius);
        rew_r1_0_max = max([resultsMy(sameResolutionstIDs).REWARDS]);
        maxRewardAll = max(maxRewardAll,rew_r1_0_max);
        minRewardAll = min(minRewardAll,rew_r1_0_max);
        %rew_r1_0_mean_ctime_last_imp = mean([resultsMy(sameResolutionstIDs).CTIME_LAST_IMPR]);
        rew_r1_0_mean_ctime = mean([resultsMy(sameResolutionstIDs).CTIME]);
        %mean(A)
       
         %add budget
        %comparisonStruct(resultID).BUDGET = actualBudget;
        comparisonStruct(resultID).NEIGH_RADIUS = neighRadius;
       
        comparisonStruct(resultID).MY_REWARD_R_1_0_MAX = rew_r1_0_max ;
        %comparisonStruct(resultID).CTIME_LAST_IMPR = rew_r1_0_mean_ctime_last_imp/1000 ;
        comparisonStruct(resultID).CTIME = rew_r1_0_mean_ctime/1000 ;

        resultID = resultID + 1;
    end
   size(comparisonStruct)
   signs(signid)
  % [AX,hndlRew,hndlTime] = plotyy([comparisonStruct(:).RESOLUTION],[comparisonStruct(:).MY_REWARD_R_1_0_MAX],[comparisonStruct(:).RESOLUTION],[comparisonStruct(:).CTIME]);
   hndlRew = plot([comparisonStruct(:).NEIGH_RADIUS],[comparisonStruct(:).MY_REWARD_R_1_0_MAX],signs{signid});
   
   %timePlots = [timePlots , hndlTime]; 
   rewardPlots = [rewardPlots , hndlRew]; 
   signid= signid +1;
  end
end

%axes(ax1);

%ylabel(ax1,'Collected reward - R')
%ylabel(ax2,'Computational time [s]')

[legend_h,object_h,plot_h,text_strings] = columnlegend(1,[rewardPlots],{legendsRew{:}},'northwest','boxon');
legend_h.set('Position',[0.07 0.88 0.8 0.03])

tightfig;
%legend(rewardPlots,legends,'location','northoutside','Orientation','horizontal')
if SAVE_TO_FILE
save2pdf(outputFilename,fig);
print2im(outputFilenamepng, fig,'-alpha');
end






