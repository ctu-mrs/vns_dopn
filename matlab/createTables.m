clear all
close all

SAVE=true;
OUTPUT_FOLDER = './';
%sets={'Tsiligirides Set 1','Tsiligirides Set 2','Tsiligirides Set 3','Chao diamond-shaped Set','Chao squared-shaped Set'};
%sets={'Chao diamond-shaped Set'};
sets={{'Tsiligirides Set 3',strcat(OUTPUT_FOLDER, 'resultsSet3DOPN.tex'),'../data/resultsP36466/resultsAll.log','Results comparison for Set 3','table:set3'},...
      {'Chao diamond-shaped Set',strcat(OUTPUT_FOLDER, 'resultsSet64DOPN.tex'),'../data/resultsP36466/resultsAll.log','Results comparison for Set 64','table:set64'},...
      {'Chao squared-shaped Set',strcat(OUTPUT_FOLDER, 'resultsSet66DOPN.tex'),'../data/resultsP36466/resultsAll.log','Results comparison for Set 66','table:set66'}}


%ORIGINAL_CHAO_LOG='../etc/previous_results/results-chao.log'
%ORIGINAL_4PHASE_LOG='../etc/previous_results/results-4phase.log'
%ORIGINAL_SCHILDE='../etc/previous_results/results-schilde.log'
%ORIGINAL_VNS='../etc/previous_results/results-vns.log'


%resultsOriginalChao = parseLog(ORIGINAL_CHAO_LOG);
%resultsOriginal4Phase = parseLog(ORIGINAL_4PHASE_LOG);
%resultsOriginalShilde = parseLog(ORIGINAL_SCHILDE);
%resultsOriginalVNS = parseLog(ORIGINAL_VNS);

for setID=1:size(sets,2)
clear input
set = sets{:,setID};
resultID = 1;
comparisonStruct = struct;
setName = set{1};
resultTableFile = set{2};
resultsMy = parseLog(set{3})
caption = set{4}
label = set{5};
myResultIDs = find(ismember({resultsMy(:).NAME},{setName}));
budegets =  unique([resultsMy(myResultIDs).BUDGET]);    
radiuses = sort(unique([resultsMy(myResultIDs).RADIUS]))
neigh_radiuses = sort(unique([resultsMy(myResultIDs).NEIGHBORHOOD_RADIUS]))
%radiuses = [ 0 , 0.5 , 0.9 , 1.3]

hasChao = false;
has4Phase = false;
hasVNS = false;

    for actualBudget = budegets
     
        sameBudgetIDs = find(ismember({resultsMy(:).NAME},{setName}) & [resultsMy(:).BUDGET] == actualBudget);
   
        %mean(A)
       
         %add budget
        comparisonStruct(resultID).BUDGET = actualBudget;
        
        %{
        originalRIDBudgetChao = find(ismember({resultsOriginalChao(:).NAME},{setName})& [resultsOriginalChao(:).BUDGET]==actualBudget);
        if(size(originalRIDBudgetChao,2)>0)
            hasChao = true;
            resultOriginalChao = resultsOriginalChao(originalRIDBudgetChao);
            comparisonStruct(resultID).ORIGINAL_REWARD_CHAO = resultOriginalChao.REWARDS;
        end
        
        originalRIDBudget4Phase = find(ismember({resultsOriginal4Phase(:).NAME},{setName})& [resultsOriginal4Phase(:).BUDGET]==actualBudget);
        if(size(originalRIDBudget4Phase,2)>0)
            has4Phase = true;
            resultOriginal4Phase = resultsOriginal4Phase(originalRIDBudget4Phase);
            comparisonStruct(resultID).ORIGINAL_REWARD_4PHASE = resultOriginal4Phase.REWARDS;
        end
        
        originalRIDBudgetVNS = find(ismember({resultsOriginalVNS(:).NAME},{setName})& [resultsOriginalVNS(:).BUDGET]==actualBudget);
        if(size(originalRIDBudgetVNS,2)>0)
            hasVNS = true;
            resultOriginalVNS = resultsOriginalVNS(originalRIDBudgetVNS);
            comparisonStruct(resultID).ORIGINAL_REWARD_VNS = resultOriginalVNS.REWARDS;
        end
        %}
        
       
        for neigh_radius = neigh_radiuses
            for radius = radiuses
            
                sameBudgetRadiusIDs = find(ismember({resultsMy(:).NAME},{setName}) & [resultsMy(:).BUDGET] ==actualBudget & [resultsMy(:).NEIGHBORHOOD_RADIUS] == neigh_radius & [resultsMy(:).RADIUS] == radius);

                rew_max = max([resultsMy(sameBudgetRadiusIDs).REWARDS]);

                rew_mean = mean([resultsMy(sameBudgetRadiusIDs).REWARDS]);

                rew_std = std([resultsMy(sameBudgetRadiusIDs).REWARDS]);

                rew_mean_ctime_last_impr = round(mean([resultsMy(sameBudgetRadiusIDs).CTIME_LAST_IMPR]));

                rew_mean_ctime_end = mean([resultsMy(sameBudgetRadiusIDs).CTIME]);

                if(rew_mean_ctime_last_impr<100)
                    if rew_mean_ctime_last_impr<1
                        rew_mean_ctime_last_impr = 1;
                    end

                    rew_mean_ctime_str = sprintf('%dms',rew_mean_ctime_last_impr);
                elseif(rew_mean_ctime_last_impr<1000*100)

                    rew_mean_ctime_last_impr=round(rew_mean_ctime_last_impr/100)*100;
                    rew_mean_ctime_str = sprintf('%.1fs',rew_mean_ctime_last_impr/1000);       
                else
                    rew_mean_ctime_last_impr=round(rew_mean_ctime_last_impr/6000)*6000;
                    rew_mean_ctime_str = sprintf('%.1fm',rew_mean_ctime_last_impr/60000);           
                end


                column_name = strcat('MY_REWARD_R_',num2str(round(radius*10)),'_NR_',num2str(round(neigh_radius*10)),'_MAX');
                comparisonStruct(resultID).(column_name) = sprintf('%i',rew_max) ;
            
            end
        end
        %comparisonStruct(resultID).MY_REWARD_R_0_0_MEAN = rew_r0_0_mean;
        %comparisonStruct(resultID).MY_REWARD_R_0_5_MEAN = rew_r0_5_mean;
        %comparisonStruct(resultID).MY_REWARD_R_1_0_MEAN = rew_r1_0_mean;
        
        
        %comparisonStruct(resultID).CTIME_S_R_0_0 = rew_r0_0_mean_ctime;
        %comparisonStruct(resultID).CTIME_S_R_0_5 = rew_r0_5_mean_ctime;
        %comparisonStruct(resultID).CTIME_S_R_1_0 = rew_r1_0_mean_ctime;

        resultID = resultID + 1;
    end


    input.data = comparisonStruct;
    %{
    input.tableColLabels =  struct;
    input.tableColLabels.BUDGET = '$T_{max}$';
    if hasChao
        input.tableColLabels.ORIGINAL_REWARD_CHAO = 'Chao';
    end
    if has4Phase
        input.tableColLabels.ORIGINAL_REWARD_4PHASE = '4Phase';
    end
    input.tableColLabels.MY_REWARD_R_0_0 = 'DOP VNS';
    input.tableColLabels.MY_REWARD_R_0_5 = 'DOP VNS r=0.5';
    input.tableColLabels.MY_REWARD_R_1_0 = 'DOP VNS r=1.0';
    input.tableColLabels.CTIME = 'cacl time[ms]';
    %}
    
    input.dataFormat.BUDGET = '%i';
   
    
    for neigh_radius = neigh_radiuses
        for radius = radiuses
             column_name = strcat('MY_REWARD_R_',num2str(round(radius*10)),'_NR_',num2str(round(neigh_radius*10)),'_MAX');
             input.dataFormat.(column_name) = '%s';
        end
    end
    %input.dataFormat.MY_REWARD_R_0_0_MAX = '%s';
    %input.dataFormat.MY_REWARD_R_0_5_MAX = '%s';
    %input.dataFormat.MY_REWARD_R_1_0_MAX = '%s';
    %input.dataFormat.MY_REWARD_R_0_0_MEAN= '%.1f';
    %input.dataFormat.MY_REWARD_R_0_5_MEAN= '%.1f';
    %input.dataFormat.MY_REWARD_R_1_0_MEAN= '%.1f';
    %input.dataFormat.CTIME_S_R_0_0 = '%.1f';
    %input.dataFormat.CTIME_S_R_0_5 = '%.1f';
    %input.dataFormat.CTIME_S_R_1_0 = '%.1f';

    numNeighRadiuses = size(neigh_radiuses,2);
    radiusHeaders1='\multirow{3}{*}{$T_{max}$}';
    radiusHeaders2='';
    rullerHeader = '';
    actualRullerId = 2;
    for neigh_radius = neigh_radiuses
        numRadiuses = size(radiuses,2)
        text1 =  sprintf('& \\multicolumn{%d}{c}{$\\delta$=%0.1f}',numRadiuses,neigh_radius);
        radiusHeaders1= strcat(radiusHeaders1, text1 );
        
        nextRullerId = actualRullerId+numRadiuses;
        rullerText = sprintf(' \\cmidrule(lr){%d-%d} ',actualRullerId,nextRullerId-1);
        rullerHeader = strcat(rullerHeader, rullerText );
        actualRullerId = nextRullerId;
        for radius = radiuses
            %radiuses(1,radiusID)
            text2 =  sprintf('& $\\rho$=%0.1f',radius);
            radiusHeaders2= strcat(radiusHeaders2, text2 );
        end
    end
    
    %radiusHeaders
    input.tableColumnAlignment = 'r';
    
    input.header = {'%!TEX root = ../main.tex';...
                    '\begin{table}[!htb]';...
                    '\centering';...
                    %'{';...
                    '{\renewcommand{\tabcolsep}{12pt}';...
                    ['\caption{',caption,'\label{',label,'}','}']; ...
                    '\vspace{-0.75em}';...
                    ['\begin{tabular}{',repmat( input.tableColumnAlignment ,1,length(fieldnames(comparisonStruct))),'}'];...
                    '\hline \\ [-1.8ex]';...
                    [' ',strcat(radiusHeaders1),' {\smallskip} \\ ',rullerHeader, ' \\ [-1.8ex] '];...
                    [' ',strcat(radiusHeaders2),' \\ [-1.8ex] \\ \hline']};
    input.footer = {' \hline';...
                    '\end{tabular}';...
                    '}';...
                    '\end{table}'};
    
    
    input.tableBorders = 0;
    input.makeCompleteLatexDocument = 0;
    input.tableFloat = '[!htb]';
    latex1 = latexTable(input);

    
    if SAVE
        fileID = fopen(resultTableFile,'w');
        %fprintf(fileID,'\\documentclass{article}\n\\usepackage[a2paper]{geometry}\n\\begin{document}\n\n');
        for row = 1:size(latex1,1)
            fprintf(fileID,'%s\n',latex1{row,:});
        end
        %fprintf(fileID,'\n\n\\end{document}');
        fclose(fileID);
    end
    
    
    
    
    

end




