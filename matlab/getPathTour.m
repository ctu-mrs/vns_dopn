function wholepath = getPathTour(positions,headings,radius)
    wholepath = struct;
    wholepath.length = 0;
    wholepath.path = 0;
    wholepath.lengths = zeros(1,size(positions,2)-1);
    for i=1:(size(positions,2)-1)
        startPos = [positions(1,i) positions(2,i) headings(1,i)];
        goalPos = [positions(1,i+1) positions(2,i+1) headings(1,i+1)];
        dubinsRet = dubins(startPos,goalPos,radius);
        wholepath.lengths(1,i)=dubinsRet.length;
        wholepath.length =  wholepath.length + dubinsRet.length;
        if i==1
        wholepath.path = dubinsRet.path;
        else
        %fprintf('size wholepath.path %d %d \n',size(wholepath.path,1),size(wholepath.path,2))
        %fprintf('size dubinsRet.path %d %d \n',size(dubinsRet.path,1),size(dubinsRet.path,2))
        wholepath.path =  [wholepath.path ; dubinsRet.path];
        end
    end
end