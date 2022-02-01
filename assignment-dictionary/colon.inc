%define peak 0

%macro colon 2
	%%peak: dq peak
	db %1, 0
	%2: %define peak %%peak
%endmacro
