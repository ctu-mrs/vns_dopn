clear all
close all

% Change default axes fonts.
set(0,'DefaultAxesFontName', 'Times New Roman')
set(0,'DefaultAxesFontSize', 14)

% Change default text fonts.
set(0,'DefaultTextFontname', 'Times New Roman')
set(0,'DefaultTextFontSize', 14)

outputFilename = 'graphOPN.pdf';
outputFilenamepng = 'graphOPN.png';
SAVE_TO_FILE = true;


sets={{'Tsiligirides Set 3','../data/results_OPN/resultsAll.log','Set 3, T_{max}= 50','-b'},...
      {'Chao diamond-shaped Set','../data/results_OPN/resultsAll.log','Set 64, T_{max}= 45','-.b'},...
      {'Chao squared-shaped Set','../data/results_OPN/resultsAll.log','Set 66, T_{max}= 60',':b'}};

sets2={{'Tsiligirides Set 3','../data/bench.som_opn/results-opn-som-v2.log','Set 3, T_{max}= 50','-r'},...
      {'Chao diamond-shaped Set','../data/bench.som_opn/results-opn-som-v2.log','Set 64, T_{max}= 45','-.r'},...
      {'Chao squared-shaped Set','../data/bench.som_opn/results-opn-som-v2.log','Set 66, T_{max}= 60',':r'}}; 
  
  
 signs = ['+' 'o' 's' '*' 'd' 'x']  
 signid = 1; 
 
maxRewardAll = 0;
minRewardAll = 1000000000000000000;
fig = figure(1)
%widthFig = 1400
%heightFig = 500
widthFig = 800
heightFig = 400
linewidth = 1.3;
set(fig,'Position', [0 0 widthFig heightFig]);
hold on
legendsRew={};
legendsTime={};
%ylabel('Collected reward - R')
ylim([400 2100])
%xlim([0 25])
xlabel('Neighborhood radius - \delta')

ax1 = gca; % current axes
set(gca, 'Color','none')
ax1_pos = get(ax1,'Position'); % position of first axes
%set(gcf, 'Color', 'None')

%ax2 = axes('Position',get(ax1,'Position'),...
%       'XAxisLocation','bottom',...
%       'YAxisLocation','right',...
%       'Color','none',...
%       'XColor','k','YColor','k');

%ax2.XTick = [0]
%set(ax2, 'XTick', []);
%linkaxes([ax1 ax2],'x');
%ylim(ax2,[0 30000])

hold on


rewardPlots = [];
rewardPlotsSOM = [];
timePlots = [];
legendsCT = {};
legendsRew = {};
legendsRewSOM = {};
for setID=1:size(sets,2)
setID
set = sets{:,setID};
set2 = sets2{:,setID};
resultID = 1;
comparisonStruct = struct;
setName = set{1};
resultsMy = parseLog(set{2});
resultsSOM = parseLog(set2{2});
plotLegend = set{3};
plotSyle = set{4};

plotSyleSOM = set2{4};

%legends = {legends{:},plotLegend }
legendReward = strcat('VNS DOPN,',{' '},plotLegend);
legendRewardSOM = strcat('SOM OPN,',{' '},plotLegend);
legendCT = strcat('Computational Time,',{' '},plotLegend );
legendsRew = {legendsRew{:},legendReward{1}};
legendsRewSOM = {legendsRewSOM{:},legendRewardSOM{1}}
legendsCT = {legendsCT{:},legendCT{1}};
%legends = {legends{:},legendReward{1},legendCT{1}}
myResultIDs = find(ismember({resultsMy(:).NAME},{setName}));
resultsSOMIDs = find(ismember({resultsSOM(:).NAME},{setName}));
neigh_radiuses =  unique([resultsMy(myResultIDs).NEIGHBORHOOD_RADIUS])   
%resolutions= [ 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 20 24 28 32];

    for neigh_radius = neigh_radiuses
        
        sameResolutionstIDs = find(ismember({resultsMy(:).NAME},{setName}) & [resultsMy(:).NEIGHBORHOOD_RADIUS] == neigh_radius);
        sameResolutionsSOMIDs = find(ismember({resultsSOM(:).NAME},{setName}) & [resultsSOM(:).NEIGHBORHOOD_RADIUS] == neigh_radius);
        
        rew_r1_0_max = max([resultsMy(sameResolutionstIDs).REWARDS]);
        
        rew_r1_0_max_som = max([resultsSOM(sameResolutionsSOMIDs).REWARDS]);
      
        rew_r1_0_mean_ctime_last_imp = mean([resultsMy(sameResolutionstIDs).CTIME_LAST_IMPR]);
        rew_r1_0_mean_ctime = mean([resultsMy(sameResolutionstIDs).CTIME]);

        comparisonStruct(resultID).NEIGHBORHOOD_RADIUS = neigh_radius;
       
        comparisonStruct(resultID).MY_REWARD_R_1_0_MAX = rew_r1_0_max ;
        comparisonStruct(resultID).MY_REWARD_R_1_0_MAX_SOM = rew_r1_0_max_som ;
        comparisonStruct(resultID).CTIME_LAST_IMPR = rew_r1_0_mean_ctime_last_imp/1000 ;
        comparisonStruct(resultID).CTIME = rew_r1_0_mean_ctime/1000 ;

        resultID = resultID + 1;
    end
    
  % [AX,hndlRew,hndlTime] = plotyy([comparisonStruct(:).RESOLUTION],[comparisonStruct(:).MY_REWARD_R_1_0_MAX],[comparisonStruct(:).RESOLUTION],[comparisonStruct(:).CTIME]);
   hndlRew = plot([comparisonStruct(:).NEIGHBORHOOD_RADIUS],[comparisonStruct(:).MY_REWARD_R_1_0_MAX],strcat(signs(signid*2-1),plotSyle),'Parent',ax1,'LineWidth',linewidth);
   
   hndlRewSOM = plot([comparisonStruct(:).NEIGHBORHOOD_RADIUS],[comparisonStruct(:).MY_REWARD_R_1_0_MAX_SOM],strcat(signs(signid*2),plotSyleSOM),'Parent',ax1,'LineWidth',linewidth);
   
   
   %hndlTime = plot([comparisonStruct(:).NEIGHBORHOOD_RESOLUTION],[comparisonStruct(:).CTIME_LAST_IMPR],strcat(signs(signid*2),plotSyle),'Parent',ax2)
   
   %timePlots = [timePlots , hndlTime]; 
   rewardPlots = [rewardPlots , hndlRew]; 
   rewardPlotsSOM = [rewardPlotsSOM ,hndlRewSOM]
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
ylabel(ax1,'Collected reward - R')
%ylabel(ax2,'Computational time [s]')

[legend_h,object_h,plot_h,text_strings] = columnlegend(2,[rewardPlots rewardPlotsSOM],{legendsRew{:},legendsRewSOM{:}},'northoutside','boxon','interpeter','latex');
%legend_h.set('Position',[0.11 0.78 0.89 0.03])
legend_h.set('Position',[0.15 0.78 0.75 0.03])

tightfig
%legend(rewardPlots,legends,'location','northoutside','Orientation','horizontal')
if SAVE_TO_FILE
save2pdf(outputFilename,fig);
print(fig,'pokus.pdf','-dpdf','-r0')
print2im(outputFilenamepng, fig,'-alpha');
end






