procs=`ps --ppid $1 | grep -v PID | awk '{print $1;}'`
for p in $procs
do
	kill -9 $p	
done
kill -9 $1

