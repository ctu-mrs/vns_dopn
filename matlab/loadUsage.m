clear all
close all

usage = csvread('../results/usage.txt');

[Y,I]=sort(usage(:,7));
sortedusage=usage(I,:);
figure(1)
plot(sortedusage(:,7),sortedusage(:,8))

parts = 100
usage_size = size(sortedusage,1);

sumparts = zeros(parts,1)

for part = 0:parts-1
    fromId=1+floor(usage_size * part/parts)
    toId=floor(usage_size * (part+1)/parts)
    sumparts(part+1,1) = sum(sortedusage(fromId:toId,8))
end


figure(2)
plot([0:parts-1],sumparts(:,1))
