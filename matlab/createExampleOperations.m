clear all
close all

% Change default axes fonts.
set(0,'DefaultAxesFontName', 'Times New Roman')
set(0,'DefaultAxesFontSize', 14)

% Change default text fonts.
set(0,'DefaultTextFontname', 'Times New Roman')
set(0,'DefaultTextFontSize', 14)

PROBLEM_FILE_NAME = '../etc/example_problem_dopn/example_problem_dopn_1_25.txt';

grey = [0.4,0.4,0.4];
yellow = [0.99 0.99 0.3];
red = [0.99 0 0];
blue = [0 0 0.99];
black= [0 0 0 ];
green = [0 1 0];

neighborhood_color = yellow;
neighborhood_samples_color = red;
neighborhood_dubins_samples_color = blue;
target_center_color=blue;
path_bef_color = black;
path_aftr_color = green;
path_line_width = 1.5

%{
%% example for insert
output_filename = 'exampleInsertOperationDOPN.pdf'
output_filenamepng = 'exampleInsertOperationDOPN.png'
PATH_BEFORE_INSERT = 'beforeInsertRandom.txt';
PATH_AFTER_INSER = 'afterInsertRandom.txt';

path_bef=csvread(PATH_BEFORE_INSERT);
path_aftr=csvread(PATH_AFTER_INSER);


problem_data = dlmread(PROBLEM_FILE_NAME,' ')
start = problem_data(2,:)
goal = problem_data(3,:)
targets = problem_data(4:end,:);

start_goal_label_text_pos = [0.3 0]
target_label_offset = [-0.05,0.17]

neigh_radius = 0.5;
dubins_radius = 0.5;
neigh_sampling = 6;
dub_sampling = 6;
dub_sample_line_length = 0.15;

fig1 = figure(1)
widthFig = 320
heightFig = 500
set(fig1,'Position', [0 0 widthFig heightFig]);

circles(targets(:,1),targets(:,2),ones(size(targets,1),1)*neigh_radius,'edgecolor',neighborhood_color,'facecolor',neighborhood_color);
hold on

plot(targets(:,1),targets(:,2),'.','Color',target_center_color,'MarkerSize',20);

for dub_sample = 0:dub_sampling
    ang_dub =  (dub_sample/dub_sampling)*2*pi;
    dubSampleStart = start(1:2)+[cos(ang_dub)*dub_sample_line_length,sin(ang_dub)*dub_sample_line_length];
    plot([start(1) dubSampleStart(1)],[start(2) dubSampleStart(2)],'-','Color',neighborhood_dubins_samples_color,'LineWidth',1.5);
    dubSampleGoal = goal(1:2)+[cos(ang_dub)*dub_sample_line_length,sin(ang_dub)*dub_sample_line_length];
    plot([goal(1) dubSampleGoal(1)],[goal(2) dubSampleGoal(2)],'-','Color',neighborhood_dubins_samples_color,'LineWidth',1.5);
end
plot(start(1),start(2),'.','Color',target_center_color,'MarkerSize',20);
plot(goal(1),goal(2),'.','Color',target_center_color,'MarkerSize',20);

text(start(1)+start_goal_label_text_pos(1),start(2)+start_goal_label_text_pos(2),'start')
text(goal(1)+start_goal_label_text_pos(1),goal(2)+start_goal_label_text_pos(2),'goal')

for tId = 1:size(targets,1)
    for neigh_sample = 0:(neigh_sampling-1)
        ang_neigh = (neigh_sample/neigh_sampling)*2*pi;
        neighSample = targets(tId,1:2)+[cos(ang_neigh)*neigh_radius,sin(ang_neigh)*neigh_radius];
        
        
        for dub_sample = 0:dub_sampling
            ang_dub =  (dub_sample/dub_sampling)*2*pi;
            dubSample = neighSample+[cos(ang_dub)*dub_sample_line_length,sin(ang_dub)*dub_sample_line_length];
            plot([neighSample(1) dubSample(1)],[neighSample(2) dubSample(2)],'-','Color',neighborhood_dubins_samples_color,'LineWidth',1.5);
        end
        
        plot(neighSample(1),neighSample(2),'.','Color',neighborhood_samples_color,'MarkerSize',20);
    end
    
    text(targets(tId,1)+target_label_offset(1),targets(tId,2)+target_label_offset(2),strcat('s_',num2str(tId+1)))
end


plot(path_aftr(:,1),path_aftr(:,2),'-','Color',path_aftr_color,'LineWidth',path_line_width);
plot(path_bef(:,1),path_bef(:,2),'--','Color',path_bef_color,'LineWidth',path_line_width);

axis equal
axis off;
%axis tight;
set(gca, 'YTick', []); 
set(gca, 'XTick', []);
tightfig
save2pdf(output_filename,fig1,300);

%% example for exchange
output_filename = 'exampleExchangeOperationDOPN.pdf'
output_filenamepng = 'exampleExchangeOperationDOPN.png'
PATH_BEFORE_INSERT = 'beforeExchangeRandom.txt';
PATH_AFTER_INSER = 'afterExchangeRandom.txt';

path_bef=csvread(PATH_BEFORE_INSERT);
path_aftr=csvread(PATH_AFTER_INSER);


problem_data = dlmread(PROBLEM_FILE_NAME,' ')
start = problem_data(2,:)
goal = problem_data(3,:)
targets = problem_data(4:end,:);


start_goal_label_text_pos = [0.3 0]
target_label_offset = [-0.05,0.17]

neigh_radius = 0.5;
dubins_radius = 0.5;
neigh_sampling = 6;
dub_sampling = 6;
dub_sample_line_length = 0.15;

fig2 = figure(2)
widthFig = 320
heightFig = 500
set(fig2,'Position', [0 0 widthFig heightFig]);

circles(targets(:,1),targets(:,2),ones(size(targets,1),1)*neigh_radius,'edgecolor',neighborhood_color,'facecolor',neighborhood_color);
hold on

plot(targets(:,1),targets(:,2),'.','Color',target_center_color,'MarkerSize',20);

for dub_sample = 0:dub_sampling
    ang_dub =  (dub_sample/dub_sampling)*2*pi;
    dubSampleStart = start(1:2)+[cos(ang_dub)*dub_sample_line_length,sin(ang_dub)*dub_sample_line_length];
    plot([start(1) dubSampleStart(1)],[start(2) dubSampleStart(2)],'-','Color',neighborhood_dubins_samples_color,'LineWidth',1.5);
    dubSampleGoal = goal(1:2)+[cos(ang_dub)*dub_sample_line_length,sin(ang_dub)*dub_sample_line_length];
    plot([goal(1) dubSampleGoal(1)],[goal(2) dubSampleGoal(2)],'-','Color',neighborhood_dubins_samples_color,'LineWidth',1.5);
end
plot(start(1),start(2),'.','Color',target_center_color,'MarkerSize',20);
plot(goal(1),goal(2),'.','Color',target_center_color,'MarkerSize',20);

text(start(1)+start_goal_label_text_pos(1),start(2)+start_goal_label_text_pos(2),'start')
text(goal(1)+start_goal_label_text_pos(1),goal(2)+start_goal_label_text_pos(2),'goal')

for tId = 1:size(targets,1)
    for neigh_sample = 0:(neigh_sampling-1)
        ang_neigh = (neigh_sample/neigh_sampling)*2*pi;
        neighSample = targets(tId,1:2)+[cos(ang_neigh)*neigh_radius,sin(ang_neigh)*neigh_radius];
        
        
        for dub_sample = 0:dub_sampling
            ang_dub =  (dub_sample/dub_sampling)*2*pi;
            dubSample = neighSample+[cos(ang_dub)*dub_sample_line_length,sin(ang_dub)*dub_sample_line_length];
            plot([neighSample(1) dubSample(1)],[neighSample(2) dubSample(2)],'-','Color',neighborhood_dubins_samples_color,'LineWidth',1.5);
        end
        
        plot(neighSample(1),neighSample(2),'.','Color',neighborhood_samples_color,'MarkerSize',20);
    end
    
    text(targets(tId,1)+target_label_offset(1),targets(tId,2)+target_label_offset(2),strcat('s_',num2str(tId+1)))
end


plot(path_aftr(:,1),path_aftr(:,2),'-','Color',path_aftr_color,'LineWidth',path_line_width);
plot(path_bef(:,1),path_bef(:,2),'--','Color',path_bef_color,'LineWidth',path_line_width);

axis equal
axis off;
%axis tight;
set(gca, 'YTick', []); 
set(gca, 'XTick', []);
tightfig
save2pdf(output_filename,fig2,300);


%% example for path insert

output_filename = 'examplePathInsertOperationDOPN.pdf'
output_filenamepng = 'examplePathInsertOperationDOPN.png'
PATH_BEFORE_INSERT = 'beforePathInsertRandom.txt';
PATH_AFTER_INSER = 'afterPathInsertRandom.txt';

path_bef=csvread(PATH_BEFORE_INSERT);
path_aftr=csvread(PATH_AFTER_INSER);


problem_data = dlmread(PROBLEM_FILE_NAME,' ')
start = problem_data(2,:)
goal = problem_data(3,:)
targets = problem_data(4:end,:);


start_goal_label_text_pos = [0.3 0]
target_label_offset = [-0.05,0.17]

neigh_radius = 0.5;
dubins_radius = 0.5;
neigh_sampling = 6;
dub_sampling = 6;
dub_sample_line_length = 0.15;

fig3 = figure(3)
widthFig = 320
heightFig = 500
set(fig3,'Position', [0 0 widthFig heightFig]);

circles(targets(:,1),targets(:,2),ones(size(targets,1),1)*neigh_radius,'edgecolor',neighborhood_color,'facecolor',neighborhood_color);
hold on

plot(targets(:,1),targets(:,2),'.','Color',target_center_color,'MarkerSize',20);

for dub_sample = 0:dub_sampling
    ang_dub =  (dub_sample/dub_sampling)*2*pi;
    dubSampleStart = start(1:2)+[cos(ang_dub)*dub_sample_line_length,sin(ang_dub)*dub_sample_line_length];
    plot([start(1) dubSampleStart(1)],[start(2) dubSampleStart(2)],'-','Color',neighborhood_dubins_samples_color,'LineWidth',1.5);
    dubSampleGoal = goal(1:2)+[cos(ang_dub)*dub_sample_line_length,sin(ang_dub)*dub_sample_line_length];
    plot([goal(1) dubSampleGoal(1)],[goal(2) dubSampleGoal(2)],'-','Color',neighborhood_dubins_samples_color,'LineWidth',1.5);
end
plot(start(1),start(2),'.','Color',target_center_color,'MarkerSize',20);
plot(goal(1),goal(2),'.','Color',target_center_color,'MarkerSize',20);

text(start(1)+start_goal_label_text_pos(1),start(2)+start_goal_label_text_pos(2),'start')
text(goal(1)+start_goal_label_text_pos(1),goal(2)+start_goal_label_text_pos(2),'goal')

for tId = 1:size(targets,1)
    for neigh_sample = 0:(neigh_sampling-1)
        ang_neigh = (neigh_sample/neigh_sampling)*2*pi;
        neighSample = targets(tId,1:2)+[cos(ang_neigh)*neigh_radius,sin(ang_neigh)*neigh_radius];
        
        
        for dub_sample = 0:dub_sampling
            ang_dub =  (dub_sample/dub_sampling)*2*pi;
            dubSample = neighSample+[cos(ang_dub)*dub_sample_line_length,sin(ang_dub)*dub_sample_line_length];
            plot([neighSample(1) dubSample(1)],[neighSample(2) dubSample(2)],'-','Color',neighborhood_dubins_samples_color,'LineWidth',1.5);
        end
        
        plot(neighSample(1),neighSample(2),'.','Color',neighborhood_samples_color,'MarkerSize',20);
    end
    
    text(targets(tId,1)+target_label_offset(1),targets(tId,2)+target_label_offset(2),strcat('s_',num2str(tId+1)))
end


plot(path_aftr(:,1),path_aftr(:,2),'-','Color',path_aftr_color,'LineWidth',path_line_width);
plot(path_bef(:,1),path_bef(:,2),'--','Color',path_bef_color,'LineWidth',path_line_width);

axis equal
axis off;
%axis tight;
set(gca, 'YTick', []); 
set(gca, 'XTick', []);
tightfig
save2pdf(output_filename,fig3,300);
%}

%% example for path exchange

output_filename = 'examplePathExchangeOperationDOPN.pdf'
output_filenamepng = 'examplePathExchangeOperationDOPN.png'
PATH_BEFORE_INSERT = 'beforePathExchangeRandom.txt';
PATH_AFTER_INSER = 'afterPathExchangeRandom.txt';

path_bef=csvread(PATH_BEFORE_INSERT);
path_aftr=csvread(PATH_AFTER_INSER);


problem_data = dlmread(PROBLEM_FILE_NAME,' ')
start = problem_data(2,:)
goal = problem_data(3,:)
targets = problem_data(4:end,:);


start_goal_label_text_pos = [0.3 0]
target_label_offset = [-0.05,0.17]

neigh_radius = 0.5;
dubins_radius = 0.5;
neigh_sampling = 6;
dub_sampling = 6;
dub_sample_line_length = 0.15;

fig4 = figure(4)
widthFig = 320
heightFig = 500
set(fig4,'Position', [0 0 widthFig heightFig]);

circles(targets(:,1),targets(:,2),ones(size(targets,1),1)*neigh_radius,'edgecolor',neighborhood_color,'facecolor',neighborhood_color);
hold on

plot(targets(:,1),targets(:,2),'.','Color',target_center_color,'MarkerSize',20);

for dub_sample = 0:dub_sampling
    ang_dub =  (dub_sample/dub_sampling)*2*pi;
    dubSampleStart = start(1:2)+[cos(ang_dub)*dub_sample_line_length,sin(ang_dub)*dub_sample_line_length];
    plot([start(1) dubSampleStart(1)],[start(2) dubSampleStart(2)],'-','Color',neighborhood_dubins_samples_color,'LineWidth',1.5);
    dubSampleGoal = goal(1:2)+[cos(ang_dub)*dub_sample_line_length,sin(ang_dub)*dub_sample_line_length];
    plot([goal(1) dubSampleGoal(1)],[goal(2) dubSampleGoal(2)],'-','Color',neighborhood_dubins_samples_color,'LineWidth',1.5);
end
plot(start(1),start(2),'.','Color',target_center_color,'MarkerSize',20);
plot(goal(1),goal(2),'.','Color',target_center_color,'MarkerSize',20);

text(start(1)+start_goal_label_text_pos(1),start(2)+start_goal_label_text_pos(2),'start')
text(goal(1)+start_goal_label_text_pos(1),goal(2)+start_goal_label_text_pos(2),'goal')

for tId = 1:size(targets,1)
    for neigh_sample = 0:(neigh_sampling-1)
        ang_neigh = (neigh_sample/neigh_sampling)*2*pi;
        neighSample = targets(tId,1:2)+[cos(ang_neigh)*neigh_radius,sin(ang_neigh)*neigh_radius];
        
        
        for dub_sample = 0:dub_sampling
            ang_dub =  (dub_sample/dub_sampling)*2*pi;
            dubSample = neighSample+[cos(ang_dub)*dub_sample_line_length,sin(ang_dub)*dub_sample_line_length];
            plot([neighSample(1) dubSample(1)],[neighSample(2) dubSample(2)],'-','Color',neighborhood_dubins_samples_color,'LineWidth',1.5);
        end
        
        plot(neighSample(1),neighSample(2),'.','Color',neighborhood_samples_color,'MarkerSize',20);
    end
    
    text(targets(tId,1)+target_label_offset(1),targets(tId,2)+target_label_offset(2),strcat('s_',num2str(tId+1)))
end

plot(path_aftr(:,1),path_aftr(:,2),'-','Color',path_aftr_color,'LineWidth',path_line_width);
plot(path_bef(:,1),path_bef(:,2),'--','Color',path_bef_color,'LineWidth',path_line_width);


axis equal
axis off;
%axis tight;
set(gca, 'YTick', []); 
set(gca, 'XTick', []);
tightfig
save2pdf(output_filename,fig4,300);