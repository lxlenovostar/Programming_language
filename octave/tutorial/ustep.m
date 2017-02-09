function y = ustep(t, t0)
%USTEP(t, t0) unit step at t0
% A unit step is defined as
% 	0 for t < t0
% 	1 for t >= t0

% the size function, which returns two values: the number of rows and
% then the number of columns of the vector (or matrix). 
[m,n] = size(t);

% Check that this is a vector, not a matrix , should in this way i.e. (1 x n) or (m x 1)
if m ~= 1 & n ~=1
	% The error function prints out a message and aborts the function if there is a problem.
	error('T must be a vector');
end

y = zeros(m, n); %Initialise output array

% The length function tells us how many elements there are in the vector t.
for k = 1:length(t)
	if t(k) >= t0
		y(k) = 1; %Otherwise, leave it at zero, which is correct
	end
end
