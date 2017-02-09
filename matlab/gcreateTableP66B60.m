clear all
close all

SAVE=true;
LOG_FILE = '/results.log';
LOG_FOLDER = '../localTests/results/';
%sets={'Tsiligirides Set 1','Tsiligirides Set 2','Tsiligirides Set 3','Chao diamond-shaped Set','Chao squared-shaped Set'};
%sets={'Chao diamond-shaped Set'};
files = dir(LOG_FOLDER);
directoryNames = {files([files.isdir]).name};
directoryNames = directoryNames(~ismember(directoryNames,{'.','..'}))

directoryNames

lineNum = 1;

for DIR_ID = 1:size(directoryNames,2)
   fileToLoad = strcat(LOG_FOLDER,directoryNames(1,DIR_ID),LOG_FILE)
   actualLoaded = parseLog(fileToLoad{1});
   allLogs(lineNum) =  actualLoaded;
   lineNum = lineNum +1;
   size(allLogs)
end

radiuses = unique([resultsMy(myResultIDs).RADIUS])

for radius=radiuses

    
neigh_radiuses =  unique([resultsMy(myResultIDs).NEIGHBORHOOD_RADIUS]);       
    
set = sets{:,setID};
resultID = 1;
comparisonStruct = struct;
setName = set{1};
resultTableFile = set{2};
resultsMy = parseLog(set{3})
caption = set{4}
label = set{5};
myResultIDs = find(ismember({resultsMy(:).NAME},{setName}));
 

%radiuses = [ 0 , 0.5 , 0.9 , 1.3]

hasChao = false;
has4Phase = false;
hasVNS = false;

    for actualBudget = budegets
     
        sameBudgetIDs = find(ismember({resultsMy(:).NAME},{setName}) & [resultsMy(:).BUDGET] == actualBudget);
   
        %mean(A)
       
         %add budget
        comparisonStruct(resultID).BUDGET = actualBudget;
        
       
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
        
        
       
        
        for radius = radiuses
            sameBudgetRadiusIDs = find(ismember({resultsMy(:).NAME},{setName}) & [resultsMy(:).BUDGET] ==actualBudget & [resultsMy(:).RADIUS] == radius);
           
            rew_max = max([resultsMy(sameBudgetRadiusIDs).REWARDS]);
            
            sameBudgetRadiusIDs_max_rew = find(ismember({resultsMy(:).NAME},{setName}) & [resultsMy(:).BUDGET] ==actualBudget & [resultsMy(:).RADIUS] == radius & [resultsMy(:).REWARDS] == rew_max);

       
            rew_mean = mean([resultsMy(sameBudgetRadiusIDs).REWARDS]);
        
            rew_std = std([resultsMy(sameBudgetRadiusIDs).REWARDS]);
         
            rew_mean_ctime_last_impr = round(mean([resultsMy(sameBudgetRadiusIDs).CTIME_LAST_IMPR]));

            rew_mean_ctime_end = mean([resultsMy(sameBudgetRadiusIDs).CTIME]);
            
            rew_mean_ctime_last_impr_max_rew =round(mean([resultsMy(sameBudgetRadiusIDs_max_rew).CTIME_LAST_IMPR]));
            
            rew_mean_num_iter_impr_max_rew = round(mean([resultsMy(sameBudgetRadiusIDs_max_rew).NUM_ITERS_LAST_IMPR]));

            if(rew_mean_ctime_last_impr_max_rew<100)
                if rew_mean_ctime_last_impr_max_rew<1
                    rew_mean_ctime_last_impr_max_rew = 1;
                end
                
                rew_mean_ctime_str = sprintf('%dms',rew_mean_ctime_last_impr_max_rew);
            elseif(rew_mean_ctime_last_impr_max_rew<1000*100)
                
                rew_mean_ctime_last_impr_max_rew=round(rew_mean_ctime_last_impr_max_rew/100)*100;
                rew_mean_ctime_str = sprintf('%.1fs',rew_mean_ctime_last_impr_max_rew/1000);       
            else
                rew_mean_ctime_last_impr_max_rew=round(rew_mean_ctime_last_impr_max_rew/6000)*6000;
                rew_mean_ctime_str = sprintf('%.1fm',rew_mean_ctime_last_impr_max_rew/60000);           
            end

          
                
            comparisonStruct(resultID).(strcat('MY_REWARD_R_',num2str(round(radius*10)),'_MAX')) = sprintf('%i',rew_max) ;
            %comparisonStruct(resultID).(strcat('MY_CTIME_R_',num2str(round(radius*10)))) = sprintf('%s',rew_mean_ctime_str) ;
            %comparisonStruct(resultID).MY_REWARD_R_0_0_MAX 
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
    hasChao
    if hasChao
        input.dataFormat.ORIGINAL_REWARD_CHAO = '%i';
    end
    has4Phase
    if has4Phase
        input.dataFormat.ORIGINAL_REWARD_4PHASE = '%i';
    end
    hasVNS
    if hasVNS
        input.dataFormat.ORIGINAL_REWARD_VNS = '%i';
    end
    
    for radius = radiuses
         input.dataFormat.(strcat('MY_REWARD_R_',num2str(round(radius*10)),'_MAX')) = '%s';
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

    radiusHeaders='';
    for radiusID = 1:size(radiuses,2)
        %radiuses(1,radiusID)
        text =  sprintf('& $\\rho$=%0.1f',radiuses(1,radiusID));
        radiusHeaders= strcat(radiusHeaders, text );
    end
    
    %radiusHeaders
    input.tableColumnAlignment = 'r';
    
    input.header = {'%!TEX root = ../main.tex';...
                    '\begin{table}[!htb]';...
                    '\centering';...
                    '{\renewcommand{\tabcolsep}{2pt}';...
                    ['\caption{',caption,'\label{',label,'}','}']; ...
                    '\vspace{-0.75em}';...
                    ['\begin{tabular}{',repmat( input.tableColumnAlignment ,1,length(fieldnames(comparisonStruct))),'}'];...
                    '\hline \\ [-1.8ex]';...
                    ['\multirow{3}{*}{$\mathbf{T_{max}}$}  & \multirow{3}{*}{\textbf{Chao}} ',repmat(' & \multirow{3}{*}{\textbf{4Phase}} ',1,has4Phase),repmat(' & \multirow{3}{*}{\textbf{VNS}} ',1,hasVNS),' & \multicolumn{',num2str(size(radiuses,2)),'}{c}{\textbf{Proposed VNS-based DOP}} {\smallskip} \\ \cline{',num2str(3+has4Phase+hasVNS),'-',num2str(size(radiuses,2)+2+has4Phase+hasVNS),'} \\ [-1.8ex]'];...
                    [' &  ',repmat(' & ',1,has4Phase),repmat(' & ',1,hasVNS),strcat(radiusHeaders),' \\ [-1.8ex] \\ \hline']};
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




