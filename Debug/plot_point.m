load('proc.txt');
for i=1:10
    scatter(proc(36*i-35:36*i,1)*100,proc(36*i-35:36*i,2)*100);
    hold on;
end