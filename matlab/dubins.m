function toRet= dubins(start2d,goal2d,radius)

SAMPLE_DIST = 0.2;
MIN_R = radius;
R1 = MIN_R;
R2 = MIN_R;
angle_sampleR1 = SAMPLE_DIST/R1;
angle_sampleR2 = SAMPLE_DIST/R2;

MAX_HEIGHT_DIFF_PER_METER = 0.0039;
path1_length = 999999999999999999999999999;
path2_length = 999999999999999999999999999;

%start_x_y_yaw = [ 0  0  0 -90]
%goal_x_y_yaw = [20 50 pi/2 -90]

start_x_y_yaw = [ start2d(1) start2d(2) start2d(3) 0];
goal_x_y_yaw = [ goal2d(1) goal2d(2) goal2d(3) 0];



path1 = [];
path2 = [];
path3 = [];
path4 = [];
path5 = [];
path6 = [];

%dubins

dist_start_goal = sqrt((start_x_y_yaw(1)-goal_x_y_yaw(1))^2 + (start_x_y_yaw(2)-goal_x_y_yaw(2))^2);

start_orient_vec = [cos(start_x_y_yaw(3)),sin(start_x_y_yaw(3))];
start_orient_vec_n = [-start_orient_vec(2),start_orient_vec(1)];
dist_start_orient_vec_n = norm(start_orient_vec_n);

goal_orient_vec = [cos(goal_x_y_yaw(3)),sin(goal_x_y_yaw(3))];
goal_orient_vec_n = [-goal_orient_vec(2),goal_orient_vec(1)];
dist_goal_orient_vec_n = norm(goal_orient_vec_n);



% t_squared_s = (R1^2)/(start_orient_vec_n(1)^2 + start_orient_vec_n(2)^2)
% ts1 = sqrt(t_squared_s);
% ts2 = -sqrt(t_squared_s);
%
% t_squared_g = (R2^2)/(goal_orient_vec_n(1)^2 + goal_orient_vec_n(2)^2)
% tg1 = sqrt(t_squared_g);
% tg2 = -sqrt(t_squared_g);
%{
close all
fgr = figure(1);
set(fgr,'Position', [700, 0, 600, 500])
hold on
%}
Rs1 = start_x_y_yaw(1:2) + R1*start_orient_vec_n/dist_start_orient_vec_n;
Rs2 = start_x_y_yaw(1:2) - R1*start_orient_vec_n/dist_start_orient_vec_n;

Rg1 = goal_x_y_yaw(1:2) + R2*goal_orient_vec_n/dist_goal_orient_vec_n;
Rg2 = goal_x_y_yaw(1:2) - R2*goal_orient_vec_n/dist_goal_orient_vec_n;

vercRs1start = start_x_y_yaw(1:2)-Rs1;
vercRs2start = start_x_y_yaw(1:2)-Rs2;
vercRg1goal = goal_x_y_yaw(1:2)-Rg1;
vercRg2goal = goal_x_y_yaw(1:2)-Rg2;

start_angleRs1 = atan2(vercRs1start(2),vercRs1start(1));
start_angleRs2 = atan2(vercRs2start(2),vercRs2start(1));
goal_angleRg1 = atan2(vercRg1goal(2),vercRg1goal(1));
goal_angleRg2 = atan2(vercRg2goal(2),vercRg2goal(1));




%% calc path 1
%first vectors and distances
p_btwRs1_Rg2 = (Rs1*R2 + Rg2*R1)./(R1+R2);
vec_pointBtw_Rs1 = Rs1 - p_btwRs1_Rg2;
distPointBtwRs1 = norm(vec_pointBtw_Rs1);
vec_pointBtw_Rg2 = Rg2 - p_btwRs1_Rg2;
distPointBtwRg2 = norm(vec_pointBtw_Rg2);


%angles of first tangent
angle_vec_pointBtw_Rs1 = atan2(vec_pointBtw_Rs1(2),vec_pointBtw_Rs1(1));
angle_vec_pointBtw_Rg2 = atan2(vec_pointBtw_Rg2(2),vec_pointBtw_Rg2(1));

if (distPointBtwRs1>R1)
    
    angle_tangent_1_Rs = asin(R1/distPointBtwRs1);
    
    tangent1_vec_angle1_Rs = angle_vec_pointBtw_Rs1 + angle_tangent_1_Rs;
    
    tangent1_length_to_start = sqrt(distPointBtwRs1^2 - R1^2);
    tangent1_length_to_goal = sqrt(distPointBtwRg2^2 - R2^2);
    
    tangent_11_intersec_start = p_btwRs1_Rg2 + [cos(tangent1_vec_angle1_Rs),sin(tangent1_vec_angle1_Rs)]*tangent1_length_to_start;
    
    tangent_11_intersec_goal = p_btwRs1_Rg2 + [-cos(tangent1_vec_angle1_Rs),-sin(tangent1_vec_angle1_Rs)]*tangent1_length_to_goal;
    
    vercRs1intersect = tangent_11_intersec_start - Rs1;
    vercRg2intersect = tangent_11_intersec_goal - Rg2;
    
    tangent_11_intersec_start_angle = atan2(vercRs1intersect(2),vercRs1intersect(1));
    tangent_11_intersec_start_angle = normalizeAngle(tangent_11_intersec_start_angle,start_angleRs1,start_angleRs1+2*pi);
    
    tangent_11_intersec_goal_angle = atan2(vercRg2intersect(2),vercRg2intersect(1));
    tangent_11_intersec_goal_angle = normalizeAngle(tangent_11_intersec_goal_angle,goal_angleRg2,goal_angleRg2+2*pi);
    
    vect_intersects_Rs1Rg2 = tangent_11_intersec_goal - tangent_11_intersec_start;
    dist_intersects_Rs1Rg2 = norm(vect_intersects_Rs1Rg2);
    
    path1_length_wo_z = abs(tangent_11_intersec_start_angle - start_angleRs1)*R1 + dist_intersects_Rs1Rg2 + abs(tangent_11_intersec_goal_angle-goal_angleRg2)*R2;
    path1_length = path1_length_wo_z;
    
    
    
    for ang=start_angleRs1:angle_sampleR1:tangent_11_intersec_start_angle      
        path1 = [ path1 ; [(Rs1+ [cos(ang) , sin(ang)]*R1),normalizeAngle(ang+pi/2)]] ;
    end
    
    for dist=0:SAMPLE_DIST:dist_intersects_Rs1Rg2
        path1 = [ path1 ; [(tangent_11_intersec_start + vect_intersects_Rs1Rg2*dist/dist_intersects_Rs1Rg2),normalizeAngle(tangent_11_intersec_start_angle+pi/2)]] ;     
    end
    for ang2=tangent_11_intersec_goal_angle:(-angle_sampleR2):goal_angleRg2     
        path1 = [ path1 ; [(Rg2 + [cos(ang2) , sin(ang2)]*R2), normalizeAngle(ang2-pi/2)]] ;       
    end
   
    path1 = [ path1 ; [(Rg2 + [cos(goal_angleRg2) , sin(goal_angleRg2)]*R2), normalizeAngle(goal_angleRg2-pi/2)]] ;
    
end

%% calc path2
%second vectors and distances
p_btwRs2_Rg1 = (Rs2*R2 + Rg1*R1)./(R1+R2);
vec_pointBtw_Rs2 = Rs2 - p_btwRs2_Rg1;
distPointBtwRs2 = norm(vec_pointBtw_Rs2);
vec_pointBtw_Rg1 = Rg1 - p_btwRs2_Rg1;
distPointBtwRg1 = norm(vec_pointBtw_Rg1);
%angles of second tangent
angle_vec_pointBtw_Rs2 = atan2(vec_pointBtw_Rs2(2),vec_pointBtw_Rs2(1));
angle_vec_pointBtw_Rg1 = atan2(vec_pointBtw_Rg1(2),vec_pointBtw_Rg1(1));

if (distPointBtwRs2>R1)
    
    angle_tangent_2_Rs = asin(R1/distPointBtwRs2);
    tangent2_vec_angle2_Rs = angle_vec_pointBtw_Rs2 - angle_tangent_2_Rs;
    
    tangent2_length_to_start = sqrt(distPointBtwRs2^2 - R1^2);
    tangent2_length_to_goal = sqrt(distPointBtwRg1^2 - R2^2);
    
    tangent_22_intersec_start = p_btwRs2_Rg1 + [cos(tangent2_vec_angle2_Rs),sin(tangent2_vec_angle2_Rs)]*tangent2_length_to_start;
    
    tangent_22_intersec_goal = p_btwRs2_Rg1 + [-cos(tangent2_vec_angle2_Rs),-sin(tangent2_vec_angle2_Rs)]*tangent2_length_to_goal;
    
    vercRs2intersect = tangent_22_intersec_start - Rs2;
    vercRg1intersect = tangent_22_intersec_goal - Rg1;
    
    tangent_22_intersec_start_angle = atan2(vercRs2intersect(2),vercRs2intersect(1));
    tangent_22_intersec_start_angle = normalizeAngle(tangent_22_intersec_start_angle,start_angleRs2-2*pi,start_angleRs2);
    
    tangent_22_intersec_goal_angle = atan2(vercRg1intersect(2),vercRg1intersect(1));
    tangent_22_intersec_goal_angle = normalizeAngle(tangent_22_intersec_goal_angle,goal_angleRg1-2*pi,goal_angleRg1);
    
    vect_intersects_Rs2Rg1 = tangent_22_intersec_goal - tangent_22_intersec_start;
    dist_intersects_Rs2Rg1 = norm(vect_intersects_Rs2Rg1);
       
    for ang=start_angleRs2:(-angle_sampleR1):tangent_22_intersec_start_angle
        path2 = [ path2 ; [(Rs2+ [cos(ang) , sin(ang)]*R1),normalizeAngle(ang-pi/2)]] ;
    end
        
    for dist=0:SAMPLE_DIST:dist_intersects_Rs2Rg1
        path2 = [ path2 ; [(tangent_22_intersec_start + vect_intersects_Rs2Rg1*dist/dist_intersects_Rs2Rg1),normalizeAngle(tangent_22_intersec_start_angle-pi/2)]] ;
    end
    
    for ang2=tangent_22_intersec_goal_angle:angle_sampleR2:goal_angleRg1
        path2 = [path2 ; [(Rg1 + [cos(ang2) , sin(ang2)]*R2),normalizeAngle(ang2+pi/2)]];
    end
  
end

%% calc path3
%third vectors and distances
vec_tangent_Rs1Rg1 = Rg1 - Rs1;
vec_norm_tangent_Rs1Rg1 = [vec_tangent_Rs1Rg1(2),-vec_tangent_Rs1Rg1(1)];
tangent_33_intersec_start = Rs1 + R1*vec_norm_tangent_Rs1Rg1/norm(vec_norm_tangent_Rs1Rg1);
tangent_33_intersec_goal = tangent_33_intersec_start + vec_tangent_Rs1Rg1;
distTangent_33 = norm(vec_tangent_Rs1Rg1);
tangent_33_intersec_angle = atan2(vec_norm_tangent_Rs1Rg1(2),vec_norm_tangent_Rs1Rg1(1));

for ang=start_angleRs1:angle_sampleR1:normalizeAngle(tangent_33_intersec_angle,start_angleRs1,start_angleRs1+2*pi)
    path3 = [ path3 ; [(Rs1+ [cos(ang) , sin(ang)]*R1), normalizeAngle(ang+pi/2) ]] ;
end
for dist=0:SAMPLE_DIST:distTangent_33
    path3 = [ path3 ; [(tangent_33_intersec_start + vec_tangent_Rs1Rg1*dist/distTangent_33), normalizeAngle(tangent_33_intersec_angle+pi/2) ]] ;
end
for ang2=tangent_33_intersec_angle:angle_sampleR2:normalizeAngle(goal_angleRg1,tangent_33_intersec_angle,tangent_33_intersec_angle+2*pi)
    path3 = [path3 ; [(Rg1 + [cos(ang2) , sin(ang2)]*R2) ,normalizeAngle(ang2+pi/2)]  ] ;
end


%% calc path4
%fourth vector and distances
vec_tangent_Rs2Rg2 = Rg2 - Rs2;
vec_norm_tangent_Rs2Rg2 = [-vec_tangent_Rs2Rg2(2),vec_tangent_Rs2Rg2(1)];
tangent_44_intersec_start = Rs2 + R1*vec_norm_tangent_Rs2Rg2/norm(vec_norm_tangent_Rs2Rg2);
tangent_44_intersec_goal = tangent_44_intersec_start + vec_tangent_Rs2Rg2;
distTangent_44 = norm(vec_tangent_Rs2Rg2);
tangent_44_intersec_angle = atan2(vec_norm_tangent_Rs2Rg2(2),vec_norm_tangent_Rs2Rg2(1));

for ang=normalizeAngle(start_angleRs2,tangent_44_intersec_angle,tangent_44_intersec_angle+2*pi):(-angle_sampleR2):tangent_44_intersec_angle
    path4 = [ path4 ; [(Rs2 + [cos(ang) , sin(ang)]*R1),normalizeAngle(ang-pi/2) ]] ;
end
for dist=0:SAMPLE_DIST:distTangent_44
    path4 = [ path4 ; [(tangent_44_intersec_start + vec_tangent_Rs2Rg2*dist/distTangent_44),normalizeAngle(tangent_44_intersec_angle-pi/2)]] ;
end
for ang2= normalizeAngle(tangent_44_intersec_angle,goal_angleRg2,goal_angleRg2+2*pi):(-angle_sampleR2):goal_angleRg2
    path4 = [path4 ; [(Rg2 + [cos(ang2) , sin(ang2)]*R2),normalizeAngle(ang2-pi/2)]] ;
end


%% calc path5

vec_Rs1Rg1 = Rg1 - Rs1;
norm_vec_Rs1Rg1 = [-vec_Rs1Rg1(2),vec_Rs1Rg1(1)];
distRs1Rg1 = norm(vec_Rs1Rg1);
R31 = MIN_R;
if (distRs1Rg1-R1-R2-2*MIN_R) < 0
    R31 = MIN_R;
else
    R31 = (distRs1Rg1-R1-R2)/2;
end
angle_sampleR31 = SAMPLE_DIST / R31;

dist_point_btw_Rs1_Rg1 = ((R1+R31)^2 - (R2+R31)^2)/(2*distRs1Rg1) + distRs1Rg1/2;
point_btw_Rs1_Rg1 = Rs1 + dist_point_btw_Rs1_Rg1*(vec_Rs1Rg1/distRs1Rg1);
dist_ptw_btw_Rm1 = sqrt((R1+R31)^2 - dist_point_btw_Rs1_Rg1^2);
if((R1+R31)^2 - dist_point_btw_Rs1_Rg1^2 < 0 && (R1+R31)^2 - dist_point_btw_Rs1_Rg1^2 > -0.001 );
    dist_ptw_btw_Rm1 = 0;
end

Rm1 = point_btw_Rs1_Rg1 + dist_ptw_btw_Rm1*(norm_vec_Rs1Rg1/norm(norm_vec_Rs1Rg1));
vec_Rs1Rm1 = Rm1 - Rs1;
vec_Rg1Rm1 = Rm1 - Rg1;
point_intersec_Rs1Rm1 = Rs1 + R1*vec_Rs1Rm1/(R1+R31);
point_intersec_Rg1Rm1 = Rg1 + R2*vec_Rg1Rm1/(R2+R31);

vec_Rs1Rm1_angle = atan2(vec_Rs1Rm1(2),vec_Rs1Rm1(1));
vec_Rs1Rm1_angle_invers = normalizeAngle(vec_Rs1Rm1_angle+pi);
vec_Rg1Rm1_angle = atan2(vec_Rg1Rm1(2),vec_Rg1Rm1(1));
vec_Rg1Rm1_angle_invers = normalizeAngle(vec_Rg1Rm1_angle+pi);

for ang=start_angleRs1:angle_sampleR1:normalizeAngle(vec_Rs1Rm1_angle,start_angleRs1,start_angleRs1+2*pi)
    path5 = [ path5 ; [(Rs1 + [cos(ang) , sin(ang)]*R1), normalizeAngle(ang+pi/2)]] ;
end
for ang=normalizeAngle(vec_Rs1Rm1_angle_invers,vec_Rg1Rm1_angle_invers,vec_Rg1Rm1_angle_invers+2*pi):(-angle_sampleR31):vec_Rg1Rm1_angle_invers
    path5 = [ path5 ; [(Rm1 + [cos(ang) , sin(ang)]*R31), normalizeAngle(ang-pi/2)]] ;
end
for ang2= vec_Rg1Rm1_angle:angle_sampleR1:normalizeAngle(goal_angleRg1,vec_Rg1Rm1_angle,vec_Rg1Rm1_angle+2*pi)
    path5 = [path5 ; [(Rg1 + [cos(ang2) , sin(ang2)]*R2), normalizeAngle(ang2+pi/2)]] ;
end


%% calc path6

vec_Rs2Rg2 = Rg2 - Rs2;
norm_vec_Rs2Rg2 = [vec_Rs2Rg2(2),-vec_Rs2Rg2(1)];
distRs2Rg2 = norm(vec_Rs2Rg2);
R32 = MIN_R;
if (distRs2Rg2-R1-R2-2*MIN_R) < 0
    R32 = MIN_R;
else
    R32 = (distRs2Rg2-R1-R2)/2;
end
angle_sampleR32 = SAMPLE_DIST / R32;

dist_point_btw_Rs2_Rg2 = ((R1+R32)^2 - (R2+R32)^2)/(2*distRs2Rg2) + distRs2Rg2/2;
point_btw_Rs2_Rg2 = Rs2 + dist_point_btw_Rs2_Rg2*(vec_Rs2Rg2/distRs2Rg2);

dist_ptw_btw_Rm2 = sqrt((R1+R32)^2 - dist_point_btw_Rs2_Rg2^2);
if((R1+R32)^2 - dist_point_btw_Rs2_Rg2^2 < 0 && (R1+R32)^2 - dist_point_btw_Rs2_Rg2^2 > -0.001 )
    dist_ptw_btw_Rm2 = 0;
end
Rm2 = point_btw_Rs2_Rg2 + dist_ptw_btw_Rm2*(norm_vec_Rs2Rg2/norm(norm_vec_Rs2Rg2));
vec_Rs2Rm2 = Rm2 - Rs2;
vec_Rg2Rm2 = Rm2 - Rg2;
point_intersec_Rs2Rm2 = Rs2 + R1*vec_Rs2Rm2/(R1+R32);
point_intersec_Rg2Rm2 = Rg2 + R2*vec_Rg2Rm2/(R2+R32);

vec_Rs2Rm2_angle = atan2(vec_Rs2Rm2(2),vec_Rs2Rm2(1));
vec_Rs2Rm2_angle_invers = normalizeAngle(vec_Rs2Rm2_angle+pi);
vec_Rg2Rm2_angle = atan2(vec_Rg2Rm2(2),vec_Rg2Rm2(1));
vec_Rg2Rm2_angle_invers = normalizeAngle(vec_Rg2Rm2_angle+pi);

for ang=normalizeAngle(start_angleRs2,vec_Rs2Rm2_angle,vec_Rs2Rm2_angle+2*pi):(-angle_sampleR2):vec_Rs2Rm2_angle
    path6 = [ path6 ; [(Rs2 + [cos(ang) , sin(ang)]*R1),normalizeAngle(ang-pi/2)]] ;
end
for ang=vec_Rs2Rm2_angle_invers:angle_sampleR32:normalizeAngle(vec_Rg2Rm2_angle_invers,vec_Rs2Rm2_angle_invers,vec_Rs2Rm2_angle_invers+2*pi)
    path6 = [ path6 ; [(Rm2 + [cos(ang) , sin(ang)]*R32),normalizeAngle(ang+pi/2)]] ;
end
for ang2= normalizeAngle(vec_Rg2Rm2_angle,goal_angleRg2,goal_angleRg2+2*pi):(-angle_sampleR2):goal_angleRg2
    path6 = [path6 ; [(Rg2 + [cos(ang2) , sin(ang2)]*R2),normalizeAngle(ang2-pi/2)]] ;
end

%% plot start and goal positions
SIZE_DISPL_VEC = 8;
%{
plot3(start_x_y_yaw(1),start_x_y_yaw(2),start_x_y_yaw(4),'r*')
plot3([start_x_y_yaw(1) , start_x_y_yaw(1)+ SIZE_DISPL_VEC*cos(start_x_y_yaw(3))],[start_x_y_yaw(2), start_x_y_yaw(2)+ SIZE_DISPL_VEC*sin(start_x_y_yaw(3))],[start_x_y_yaw(4),start_x_y_yaw(4)],'r')
plot3(goal_x_y_yaw(1),goal_x_y_yaw(2),goal_x_y_yaw(4),'g*')
plot3([goal_x_y_yaw(1) , goal_x_y_yaw(1)+ SIZE_DISPL_VEC*cos(goal_x_y_yaw(3))],[goal_x_y_yaw(2), goal_x_y_yaw(2)+ SIZE_DISPL_VEC*sin(goal_x_y_yaw(3))],[goal_x_y_yaw(4),goal_x_y_yaw(4)],'g')
%}

%% plot circles
%
% circle(Rs1(1),Rs1(2),R1,'y')
% circle(Rs2(1),Rs2(2),R1,'b')
%
% circle(Rg1(1),Rg1(2),R2,'y')
% circle(Rg2(1),Rg2(2),R2,'b')

%% plot points between and intercecs of tangents
% plot(p_btwRs1_Rg2(1),p_btwRs1_Rg2(2),'y*')
% plot(p_btwRs2_Rg1(1),p_btwRs2_Rg1(2),'b*')
%
% plot(point_btw_Rs1_Rg1(1),point_btw_Rs1_Rg1(2),'g*')
% plot(point_btw_Rs2_Rg2(1),point_btw_Rs2_Rg2(2),'g*')

%
% plot(Rm1(1),Rm1(2),'r*')
% circle(Rm1(1),Rm1(2),R32,'r')
%
% plot(Rm2(1),Rm2(2),'r*')
% circle(Rm2(1),Rm2(2),R32,'r')

%plot([p_btwRs1_Rg2(1) , p_btwRs1_Rg2(1)+cos(angle_vec_pointBtw_Rs1)*SIZE_DISPL_VEC],[p_btwRs1_Rg2(2),p_btwRs1_Rg2(2)+sin(angle_vec_pointBtw_Rs1)*SIZE_DISPL_VEC],'b')
%plot([p_btwRs2_Rg1(1) , p_btwRs2_Rg1(1)+cos(angle_vec_pointBtw_Rs2)*SIZE_DISPL_VEC],[p_btwRs2_Rg1(2),p_btwRs2_Rg1(2)+sin(angle_vec_pointBtw_Rs2)*SIZE_DISPL_VEC],'b')


%plot(tangent_11_intersec_start(1),tangent_11_intersec_start(2),'y*')
%plot(tangent_12_intersec_start(1),tangent_12_intersec_start(2),'y*')

%plot(tangent_21_intersec_start(1),tangent_21_intersec_start(2),'b*')
%plot(tangent_22_intersec_start(1),tangent_22_intersec_start(2),'b*')

%plot(tangent_33_intersec_start(1),tangent_33_intersec_start(2),'r*')

%plot(tangent_11_intersec_goal(1),tangent_11_intersec_goal(2),'y*')
%plot(tangent_12_intersec_goal(1),tangent_12_intersec_goal(2),'y*')

%plot(tangent_21_intersec_goal(1),tangent_21_intersec_goal(2),'b*')
%plot(tangent_22_intersec_goal(1),tangent_22_intersec_goal(2),'b*')


%plot(tangent_33_intersec_start(1),tangent_33_intersec_start(2),'y*')
%plot(tangent_33_intersec_goal(1),tangent_33_intersec_goal(2),'y*')

%plot(tangent_44_intersec_start(1),tangent_44_intersec_start(2),'b*')
%plot(tangent_44_intersec_goal(1),tangent_44_intersec_goal(2),'b*')
ang_show = 2;

%{
if (size(path1,1) > 0)
plot(path1(:,1),path1(:,2),'y')
end

if (size(path2) > 0)
plot(path2(:,1),path2(:,2),'m')
end

if (size(path3) > 0)
plot(path3(:,1),path3(:,2),'c')
end

if (size(path4) > 0)
plot(path4(:,1),path4(:,2),'r')
end

if (size(path5) > 0)
plot(path5(:,1),path5(:,2),'b')
end

if (size(path6) > 0)
plot(path6(:,1),path6(:,2),'k')
end
%}

sizes = [ path1_length , path2_length , size(path3,1) , size(path4,1) , size(path5,1) , size(path6,1) ];
notNullIDX =  sizes > 0;
[minimalSize,I] = min(sizes(notNullIDX));
smalestIDX = find(sizes == minimalSize,1);

toRet = struct;
toRet.length = minimalSize;

switch smalestIDX
    case 1
        toRet.path=path1;
        %plot(path1(:,1),path1(:,2),'g')
    case 2
        toRet.path=path2   ;
        %plot(path2(:,1),path2(:,2),'g')
    case 3
        toRet.path=path3;
        %plot(path3(:,1),path3(:,2),'g')
    case 4
        toRet.path=path4  ;
        %plot(path4(:,1),path4(:,2),'g')
    case 5
        toRet.path=path5 ;
        %plot(path5(:,1),path5(:,2),'g')
    case 6
        toRet.path=path6  ;
        %plot(path6(:,1),path6(:,2),'g')
end

%{
legend('start position','start orientation','goal position','goal orientation',...
       'LSR','RSL','LSL','RSR','LRL','RLR','Location','southeast')
xlabel('x [m]')
ylabel('y [m]')
%axes equal
title('Dubins paths between start and goal in 2D')
set(fgr, 'PaperPosition', [0 0 18 15]); %Position plot at left hand corner with width 5 and height 5.
set(fgr, 'PaperSize', [18 15]); %Set the paper to have width 5 and height 5.
print(fgr','subins_paths','-dpdf')
%}

end


