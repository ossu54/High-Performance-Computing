%% Import data from text file.
% Script for importing data from the following text file:
%
%    /zhome/f7/e/77392/02614_HPC/Week1/Assignment1/High-Performance-Computing/Project1/from-mikkel/submit_mkn.sh.o3615142
%
% To extend the code to different selected data or a different text file,
% generate a function instead of a script.

% Auto-generated by MATLAB on 2017/01/05 10:22:15

%% Initialize variables.
filename = '/zhome/f7/e/77392/02614_HPC/Week1/Assignment1/High-Performance-Computing/Project1/from-mikkel/submit_mkn.sh.o3615142';
delimiter = {'    ','   '};
startRow = 24;

%% Format string for each line of text:
%   column1: double (%f)
%	column2: double (%f)
%   column3: double (%f)
% For more information, see the TEXTSCAN documentation.
formatSpec = '%f%f%f%*s%*s%[^\n\r]';

%% Open the text file.
fileID = fopen(filename,'r');

%% Read columns of data according to format string.
% This call is based on the structure of the file used to generate this
% code. If an error occurs for a different file, try regenerating the code
% from the Import Tool.
dataArray = textscan(fileID, formatSpec, 'Delimiter', delimiter, 'EmptyValue' ,-1.0,'HeaderLines' ,startRow-1, 'ReturnOnError', false);

%% Close the text file.
fclose(fileID);

%% Post processing for unimportable data.
% No unimportable data rules were applied during the import, so no post
% processing code is included. To generate code which works for
% unimportable data, select unimportable cells in a file and regenerate the
% script.

%% Create output variable
submitmkn = [dataArray{1:end-1}];
%% Clear temporary variables
clearvars filename delimiter startRow formatSpec fileID dataArray ans;