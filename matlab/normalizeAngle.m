function [ normalized ] = normalizeAngle( angle , min , max)
%NORMALIZEANGLE Summary of this function goes here
%   Detailed explanation goes here
normalized = angle;
  if nargin < 2
    min =   0;
    max = 2*pi;
  end
while (normalized < min) 
    normalized = normalized + 2*pi;
end
while (normalized >= max) 
	normalized = normalized - 2*pi;
end
	

end

