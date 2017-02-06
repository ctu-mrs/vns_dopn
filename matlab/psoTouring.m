clear all
close all

neighborhood_radius = 1.5;

radius = 0.6;
positionsT = [0 1;...
              5 0;...
              3 3;...
              2 7;...
              9 9;...
              7 4;...
              12 9;...
              14 1;...
              18 5;...
              20 7;...
              22 2];
positions = positionsT';          
     
velocity_angle_max = pi/2;
velocity_angle_min = -pi/2;

velocity_radius_dist_max = neighborhood_radius/2;
velocity_radius_dist_min = -neighborhood_radius/2;



velocities = [];

angle_max = 2*pi;
angle_min = 0;

radius_dist_max = neighborhood_radius;
radius_dist_min = neighborhood_radius;

particle_attraction = 2;
global_attraction = 1.0;
particle_attraction = 1.2;
global_attraction = 0.6;
inertia = 0.9;
inertia_min = 0.2;

inertia_decrease = 0.995
velocity_decrease = 1.0
velocity_decrease_rate = 1.0

%p_pos =[0.5968    4.8915    4.4503    5.8714    4.8028    1.6518    6.2458];
%p_vel = [-0.3926   -0.7470   -0.2836   -0.4165   -0.7466    0.6945    0.5927];

p_pos = [angle_max*rand(1,size(positions,2));...
         angle_max*rand(1,size(positions,2));...
         ((radius_dist_max-radius_dist_min)*rand(1,size(positions,2))-(radius_dist_max-radius_dist_min)*0.5*ones(1,size(positions,2)))];

p_vel_angle_heading = (velocity_angle_max-velocity_angle_min)*rand(1,size(positions,2))-(((velocity_angle_max-velocity_angle_min)/2.0)*ones(1,size(positions,2)));
p_vel_angle_neigh = (velocity_angle_max-velocity_angle_min)*rand(1,size(positions,2))-(((velocity_angle_max-velocity_angle_min)/2.0)*ones(1,size(positions,2)));
p_vel_radius_dist = (velocity_radius_dist_max-velocity_radius_dist_min)*rand(1,size(positions,2))-(((velocity_radius_dist_max-velocity_radius_dist_min)/2.0)*ones(1,size(positions,2)));
p_vel = [p_vel_angle_heading;...
         p_vel_angle_neigh;...
         p_vel_radius_dist]
   
%{
p_pos = [angle_max*rand(1,size(positions,2))];
p_vel = [(velocity_angle_max-velocity_angle_min)*rand(1,size(positions,2))-(((velocity_angle_max-velocity_angle_min)/2.0)*ones(1,size(positions,2)))];     
%}  
  
p_best = p_pos;%zeros(1,size(positions,2));
g_best = p_pos;%zeros(1,size(positions,2));

p_best_fitnesses = ones(3,size(positions,2))*realmax;
g_best_fitnesses = realmax;

%{
for pId=1:(size(p_pos,2)-1)
    p_pos(1,pId) = atan2(positions(2,pId+1)-positions(2,pId),positions(1,pId+1)-positions(1,pId));
end
p_pos(1,size(p_pos,2)) = p_pos(1,size(p_pos,2)-1);
p_best = p_pos;%zeros(1,size(positions,2));
g_best = p_pos;%zeros(1,size(positions,2));
%}

%gBestPath = getPathTour(positions,p_pos,radius);
%{
dub_lengths = zeros(1,size(positions,2)-1) 
for pId=2:size(positions,2)
    startPos = [positions(1,pId - 1),positions(2,pId - 1),p_pos(pId - 1)];
    goalPos = [positions(1,pId ),positions(2,pId),p_pos(pId)];
    dubinsRet = dubins(startPos,goalPos,radius);
    dub_lengths(1,pId-1) = dubinsRet.length;
end
%}




figure(1)
hold on
xlim([-4 28])
ylim([-4 28])
plot(positions(1,:),positions(2,:),'r+-')
wholePath = getPathTour(positions,p_pos,radius)
plot(wholePath.path(:,1),wholePath.path(:,2),'b-')
length_best = wholePath.length;
currentBest = getPathTour(positions,g_best,radius);
for iter=1:2000
    %velocity_angle_max=velocity_angle_max*0.997;
    %fprintf('%d/%d\n',iter,1000)
    
    
    positions_in_neigh = positions + [p_pos(3,:).*cos(p_pos(2,:));p_pos(3,:).*sin(p_pos(2,:))];
    %positions_in_neigh = positions;
    new_wholePath = getPathTour(positions_in_neigh,p_pos(1,:),radius);
    
    %positions
    %positions_in_neigh
    particle_impr = false;
    for pId=1:size(p_best,2)
        fitnessParticle = 0;
        %for width = 0
        for width = -1:0
            pID_width =  pId + width;
            if pID_width >= 1 && pID_width <= size(new_wholePath.lengths,2)
                %pId
                %pID_width
                fitnessParticle = fitnessParticle + new_wholePath.lengths(1,pID_width);
            end
        end
        %{
        if pId - 1 >=1
            startPos = [positions(1,pId - 1),positions(2,pId - 1),p_pos(1,pId - 1)];
            goalPos = [positions(1,pId ),positions(2,pId),p_pos(1,pId)];
            dubinsRet = dubins(startPos,goalPos,radius);
            fitnessParticle = fitnessParticle + dubinsRet.length;
        end
        if pId + 1 <= size(p_best,2)
            startPos = [positions(1,pId ),positions(2,pId ),p_pos(1,pId )];
            goalPos = [positions(1,pId +1),positions(2,pId+1),p_pos(1,pId+1)];
            dubinsRet = dubins(startPos,goalPos,radius);
            fitnessParticle = fitnessParticle + dubinsRet.length;
        end
        %}
        if( fitnessParticle < p_best_fitnesses(1,pId))
            p_best_fitnesses(1,pId) = fitnessParticle;
            p_best(:,pId) = p_pos(:,pId);
            particle_impr = true;
        end
    end
    if particle_impr
       positions_in_neigh_best = positions + [p_best(3,:).*cos(p_best(2,:));p_best(3,:).*sin(p_best(2,:))];
       particleBest = getPathTour(positions_in_neigh_best,p_best,radius); 
    end
    
    %p_best_fitnesses
    %p_best

    
    if new_wholePath.length < g_best_fitnesses
        g_best_fitnesses = new_wholePath.length;
        g_best = p_pos;
        positions_in_neigh_best = positions + [g_best(3,:).*cos(g_best(2,:));g_best(3,:).*sin(g_best(2,:))];
        currentBest = getPathTour(positions_in_neigh_best,g_best,radius);
        fprintf('%f iter %d intertia %f\n',g_best_fitnesses,iter,inertia)
    end
    
 
    diff_p_best_p_pos_heading = mod(p_best(1,:) - p_pos(1,:) + pi,2*pi) - pi;
    diff_p_best_p_pos_ang_neigh = mod(p_best(2,:) - p_pos(2,:) + pi,2*pi) - pi;
    diff_p_best_p_pos_radius_dist = abs(p_best(3,:) - p_pos(3,:));
    
    particle_best_attraction = [particle_attraction*(diff_p_best_p_pos_heading)*diag(rand(1,size(p_best,2)));...
                                particle_attraction*(diff_p_best_p_pos_ang_neigh)*diag(rand(1,size(p_best,2)));...
                                particle_attraction*(diff_p_best_p_pos_radius_dist)*diag(rand(1,size(p_best,2)))];
                            
    diff_g_best_p_pos_heading = mod(g_best(1,:) - p_pos(1,:) + pi,2*pi) - pi;
    diff_g_best_p_pos_ang_neigh = mod(g_best(2,:) - p_pos(2,:) + pi,2*pi) - pi;
    diff_g_best_p_pos_radius_dist = abs(g_best(3,:) - p_pos(3,:));
    global_best_attraction = [global_attraction*(diff_g_best_p_pos_heading)*diag(rand(1,size(p_best,2)));...
                              global_attraction*(diff_g_best_p_pos_ang_neigh)*diag(rand(1,size(p_best,2)));...
                              global_attraction*(diff_g_best_p_pos_radius_dist)*diag(rand(1,size(p_best,2)))];
    %p_best
    
    %g_best
    p_vel_next = inertia*p_vel + particle_best_attraction+global_best_attraction;
    inertia=inertia*inertia_decrease;
    if(inertia < inertia_min)
        inertia=inertia_min;
    end
    %p_vel_next = velocity_decrease*p_vel_next;
    
    p_vel_next(1,p_vel_next(1,:)>velocity_angle_max) = velocity_angle_max;
    p_vel_next(1,p_vel_next(1,:)<velocity_angle_min) = velocity_angle_min;
    p_vel_next(2,p_vel_next(2,:)>velocity_angle_max) = velocity_angle_max;
    p_vel_next(2,p_vel_next(2,:)<velocity_angle_min) = velocity_angle_min;
    p_vel_next(3,p_vel_next(3,:)>velocity_radius_dist_max) = velocity_radius_dist_max;
    p_vel_next(3,p_vel_next(3,:)<velocity_radius_dist_min) = velocity_radius_dist_min;
    
    
    p_pos_next = p_pos + p_vel;
    p_pos_next(1,p_pos_next(1,:)>2*pi)=p_pos_next(1,p_pos_next(1,:)>2*pi)-2*pi;
    p_pos_next(1,p_pos_next(1,:)<0)=p_pos_next(1,p_pos_next(1,:)<0)+2*pi;
    p_pos_next(2,p_pos_next(2,:)>2*pi)=p_pos_next(2,p_pos_next(2,:)>2*pi)-2*pi;
    p_pos_next(2,p_pos_next(2,:)<0)=p_pos_next(2,p_pos_next(2,:)<0)+2*pi;
    p_pos_next(3,p_pos_next(3,:)>radius_dist_max)=radius_dist_max;
    p_pos_next(3,p_pos_next(3,:)<radius_dist_min)=radius_dist_min;
    p_pos = p_pos_next;
    p_vel = p_vel_next;
    %velocities = [velocities;p_vel];
    
    
    hold off
    plot(positions(1,:),positions(2,:),'r+-')
    hold on
    
    for pos=1:size(positions,2)
        rectangle('Position',[positions(1,pos)-neighborhood_radius positions(2,pos)-neighborhood_radius 2*neighborhood_radius 2*neighborhood_radius],'Curvature',[1 1],'EdgeColo',[0.5 0.5 0.5])
    end
    plot(positions_in_neigh(1,:),positions_in_neigh(2,:),'c*')
    
    plot(new_wholePath.path(:,1),new_wholePath.path(:,2),'b-')
    plot(currentBest.path(:,1),currentBest.path(:,2),'g-')
    plot(particleBest.path(:,1),particleBest.path(:,2),'c-')
    xlim([-4 28])
    ylim([-4 14])
    drawnow
    
end
