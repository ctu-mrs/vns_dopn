clear all
close all

% Change default axes fonts.
set(0,'DefaultAxesFontName', 'Times New Roman')
set(0,'DefaultAxesFontSize', 14)

% Change default text fonts.
set(0,'DefaultTextFontname', 'Times New Roman')
set(0,'DefaultTextFontSize', 14)

output_filename = 'tryOperationDOPN.pdf'
output_filenamepng = 'tryOperationDOPN.png'
PATH_BEFORE = 'beforeInsertRandom.txt';
PATH_AFTER = 'afterInsertRandom.txt';

path_bef=csvread(PATH_BEFORE);
path_aftr=csvread(PATH_AFTER);

PROBLEM_FILE_NAME = '../etc/example_problem_dopn/example_problem_dopn_1_25.txt';
problem_data = dlmread(PROBLEM_FILE_NAME,' ')
start = problem_data(2,:)
goal = problem_data(3,:)
targets = problem_data(4:end,:);


grey = [0.4,0.4,0.4];
yellow = [0.99 0.99 0];




neigh_radius = 0.5;
dubins_radius = 0.5;
neigh_sampling = 8;
dub_sampling = 8;
dub_sample_line_length = 0.15;

fig = figure(1)

circles(targets(:,1),targets(:,2),ones(size(targets,1),1)*neigh_radius,'edgecolor',yellow,'facecolor',yellow);
hold on
axis equal

plot(targets(:,1),targets(:,2),'.b','MarkerSize',20);

for dub_sample = 0:dub_sampling
    ang_dub =  (dub_sample/dub_sampling)*2*pi;
    dubSampleStart = start(1:2)+[cos(ang_dub)*dub_sample_line_length,sin(ang_dub)*dub_sample_line_length];
    plot([start(1) dubSampleStart(1)],[start(2) dubSampleStart(2)],'-b','LineWidth',1.5);
    dubSampleGoal = goal(1:2)+[cos(ang_dub)*dub_sample_line_length,sin(ang_dub)*dub_sample_line_length];
    plot([goal(1) dubSampleGoal(1)],[goal(2) dubSampleGoal(2)],'-b','LineWidth',1.5);
end
plot(start(1),start(2),'.r','MarkerSize',20);
plot(goal(1),goal(2),'.r','MarkerSize',20);

for tId = 1:size(targets,1)
    for neigh_sample = 0:(neigh_sampling-1)
        ang_neigh = (neigh_sample/neigh_sampling)*2*pi;
        neighSample = targets(tId,1:2)+[cos(ang_neigh)*neigh_radius,sin(ang_neigh)*neigh_radius];
        
        
        for dub_sample = 0:dub_sampling
            ang_dub =  (dub_sample/dub_sampling)*2*pi;
            dubSample = neighSample+[cos(ang_dub)*dub_sample_line_length,sin(ang_dub)*dub_sample_line_length];
            plot([neighSample(1) dubSample(1)],[neighSample(2) dubSample(2)],'-b','LineWidth',1.5);
        end
        
        plot(neighSample(1),neighSample(2),'.r','MarkerSize',20);
    end
end

plot(path_bef(:,1),path_bef(:,2),'k');

plot(path_aftr(:,1),path_aftr(:,2),'g');


axis equal
axis off;
%axis tight;
set(gca, 'YTick', []); 
set(gca, 'XTick', []);
tightfig

save2pdf(output_filename,fig,300);