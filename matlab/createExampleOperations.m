clear all
close all

grey = [0.4,0.4,0.4];
yellow = [0.99 0.99 0];

start = [1,0,0]
goal = [2,2,2]
targets = [3, 0 , 1 ;...
    4, 1 , 0;...
    5, 2 , 3];

neigh_radius = 0.5;
dubins_radius = 0.5;
neigh_sampling = 8;
dub_sampling = 8;
dub_sample_line_length = 0.15;

circles(targets(:,2),targets(:,3),ones(size(targets,1),1)*neigh_radius,'edgecolor',yellow,'facecolor',yellow);
hold on
axis equal

plot(targets(:,2),targets(:,3),'.b','MarkerSize',20);

for dub_sample = 0:dub_sampling
    ang_dub =  (dub_sample/dub_sampling)*2*pi;
    dubSampleStart = start(2:3)+[cos(ang_dub)*dub_sample_line_length,sin(ang_dub)*dub_sample_line_length];
    plot([start(2) dubSampleStart(1)],[start(3) dubSampleStart(2)],'-b','LineWidth',1.5);
    dubSampleGoal = goal(2:3)+[cos(ang_dub)*dub_sample_line_length,sin(ang_dub)*dub_sample_line_length];
    plot([goal(2) dubSampleGoal(1)],[goal(3) dubSampleGoal(2)],'-b','LineWidth',1.5);
end
plot(start(2),start(3),'.r','MarkerSize',20);
plot(goal(2),goal(3),'.r','MarkerSize',20);

for tId = 1:size(targets,1)
    for neigh_sample = 0:(neigh_sampling-1)
        ang_neigh = (neigh_sample/neigh_sampling)*2*pi;
        neighSample = targets(tId,2:3)+[cos(ang_neigh)*neigh_radius,sin(ang_neigh)*neigh_radius]
        
        
        for dub_sample = 0:dub_sampling
            ang_dub =  (dub_sample/dub_sampling)*2*pi;
            dubSample = neighSample+[cos(ang_dub)*dub_sample_line_length,sin(ang_dub)*dub_sample_line_length];
            plot([neighSample(1) dubSample(1)],[neighSample(2) dubSample(2)],'-b','LineWidth',1.5);
        end
        
        plot(neighSample(1),neighSample(2),'.r','MarkerSize',20);
    end
end