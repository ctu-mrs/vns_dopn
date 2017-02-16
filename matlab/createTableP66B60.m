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

caption = 'Comparison Problem 66'
label = 'problem66Comparison'
resultTableFile = 'comparisonRadiiProblem66.tex'

lineNum = 1;

for DIR_ID = 1:size(directoryNames,2)
    fileToLoad = strcat(LOG_FOLDER,directoryNames(1,DIR_ID),LOG_FILE);
    actualLoaded = parseLog(fileToLoad{1});
    allLogs(lineNum) =  actualLoaded;
    lineNum = lineNum +1;
    %size(allLogs)
end

radiuses = sort(unique([allLogs(:).RADIUS]))
possible_n_radiuses = sort(unique([allLogs(:).NEIGHBORHOOD_RADIUS]))



resultID = 1;
comparisonStruct = struct;

for radius=radiuses
    radius_str =  strcat('\multirow{4}{*}{$\rho$=',sprintf('%.1f',radius),'}') ;
    comparisonStruct(resultID).radius = radius_str;
    comparisonStruct(resultID).dataformat = '$R_{max}$';
    comparisonStruct(resultID+1).dataformat = '$R_{avg}$';
    comparisonStruct(resultID+2).dataformat = '$R_{std}$';
    comparisonStruct(resultID+3).dataformat = '\textit{comp. time}';
    %comparisonStruct(resultID+5).radius = '\hline \\ [-1.8ex]';
    for neigh_radius = possible_n_radiuses
        %fieldName_rew = strcat('reward_nr', strrep(num2str(neigh_radius), '.', '_'));
        %fieldName_time =  strcat('time_nr', strrep(num2str(neigh_radius), '.', '_'));
        fieldName_value =  strcat('value_nr', strrep(num2str(neigh_radius), '.', '_'));
        comparisonStruct(resultID).(fieldName_value) = '';
        comparisonStruct(resultID+1).(fieldName_value) = '';
    end
    resultID = resultID+4;
end

resultID = 1;

for radius=radiuses
    
    
    sameRadiusIDs = find([allLogs(:).RADIUS]==radius);
    sameRadiusResults = allLogs(sameRadiusIDs);
    
    
    neigh_radiuses =  sort(unique([sameRadiusResults(:).NEIGHBORHOOD_RADIUS]));
    
    %comparisonStruct(resultID).radius = radius;

    
    for neigh_radius = neigh_radiuses
        sameBothRadiusIDs = find([sameRadiusResults(:).NEIGHBORHOOD_RADIUS]==neigh_radius);
        sameBothRadiusResults = sameRadiusResults(sameBothRadiusIDs);
        rew_max = max([sameBothRadiusResults(:).REWARDS])
        rew_mean = mean([sameBothRadiusResults(:).REWARDS])
        rew_std = std([sameBothRadiusResults(:).REWARDS])
        rew_mean_ctime_last_impr_max_rew = mean([sameBothRadiusResults(:).CTIME_LAST_IMPR])
        %fieldName_rew = strcat('value', strrep(num2str(neigh_radius), '.', '_'));
        %fieldName_time =  strcat('time_nr', strrep(num2str(neigh_radius), '.', '_'));
        fieldName_value =  strcat('value_nr', strrep(num2str(neigh_radius), '.', '_'));
        
        
        if(size(rew_max,2)>0)
            comparisonStruct(resultID).(fieldName_value) = sprintf('\\textbf{%i}',rew_max);
            comparisonStruct(resultID+1).(fieldName_value) = sprintf('%i',rew_mean);
            comparisonStruct(resultID+2).(fieldName_value) = sprintf('%.1f',rew_std);
            
            
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
            
            comparisonStruct(resultID+3).(fieldName_value) = rew_mean_ctime_str;
        end
    end
    
    
    resultID = resultID+4;
    
    
end


input.data = comparisonStruct;

input.dataFormat.radius = '%s';
input.dataFormat.dataformat = '%s';
for neigh_radius = possible_n_radiuses
        %fieldName_rew = strcat('reward_nr', strrep(num2str(neigh_radius), '.', '_'));
        %fieldName_time =  strcat('time_nr', strrep(num2str(neigh_radius), '.', '_'));
        fieldName_value =  strcat('value_nr', strrep(num2str(neigh_radius), '.', '_'));
        input.dataFormat.(fieldName_value) = '%s';
        %input.dataFormat.(fieldName_time) = '%s';
end
    
radiusHeaders=' & ';
for radiusID = 1:size(possible_n_radiuses,2)
    %radiuses(1,radiusID)
    text =  sprintf('& $\\delta$=%0.1f',possible_n_radiuses(1,radiusID));
    radiusHeaders= strcat(radiusHeaders, text );
end

radiusHeaders = strcat(radiusHeaders, '\\' );


%asdfasdfasfads
%radiuses = [ 0 , 0.5 , 0.9 , 1.3]

%radiusHeaders
input.tableColumnAlignment = 'r';

input.header = {'%!TEX root = ../main.tex';...
    '\begin{table}[!htb]';...
    '\centering';...
    '{';...
    %'\renewcommand{\tabcolsep}{2pt}';... %add if require smaller table
    ['\caption{',caption,'\label{',label,'}','}']; ...
    '\vspace{-0.75em}';...
    ['\begin{tabular}{l|',repmat( input.tableColumnAlignment ,1,length(fieldnames(comparisonStruct))-1),'}'];...
    '\hline \\ [-1.8ex]';...
    radiusHeaders;...
    '\hline \\ [-1.8ex]'};
input.footer = {' \hline';...
    '\end{tabular}';...
    '}';...
    '\end{table}'};

input.tableBordersPeriod = 4
input.tableBorders = 1;
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











