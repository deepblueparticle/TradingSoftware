#!/bin/bash

nanexDir=$1
today=`date '+%Y%m%d'`
filename=$today
filenamefullpath=$nanexDir"/"$filename".DQ.nx2"
echo `date`" Current filenamefullpath: "$filenamefullpath >> /var/tmp/NanexReconnectScript
oldfilepath=$nanexDir"/.old"
echo `date`" Oldfilepath: "$oldfilepath >> /var/tmp/NanexReconnectScript
startMinutes=5
endMinutes=1435
cd "$nanedDir"
shouldRestart=0

#check if file is created or not in dated file
if [ ! -f $filenamefullpath ]; then
 shouldRestart=1
 echo `date`" "$filenamefullpath" does not exist. So need to restart" >> /var/tmp/NanexReconnectScript
fi

#check if filesize is increased or not
if [ $shouldRestart -eq 0 ]
then
  filesz=`ls -lrt $filenamefullpath|awk '{print $5}'`
  oldfilesize=0
  oldfilename=""
  if [ -f $oldfilepath ]; then
    oldfilesize=`awk -F ',' '{print $2}' $oldfilepath`
    oldfilename=`awk -F ',' '{print $1}' $oldfilepath`
  else
    echo $filenamefullpath","$filesz > $oldfilepath
  fi
  if [ ! -z $oldfilename ]
  then
    if [ $filenamefullpath == $oldfilename ] && [ $filesz == $oldfilesize ]
    then
     shouldRestart=1
     echo `date`" "$filenamefullpath" did not have increased size. So need to restart." >> /var/tmp/NanexReconnectScript
    fi
  echo $filenamefullpath","$filesz > $oldfilepath
  fi
fi

#ignore time between [23:55] and [00:05]
if [ $shouldRestart -eq 1 ]
then
  hour=`date +%H`
  min=`date +%M`
  day=`date +%A`
  totalMins=`expr $hour \* 60 + $min`
  if [ $day == "Sunday" ] || [ $day == "Saturday" ]
  then
    echo `date`" "$day" it is. So skipping" >> /var/tmp/NanexReconnectScript
    shouldRestart=0
  elif [ $totalMins -le $endMinutes ] && [ $totalMins -gt $startMinutes ]
  then
    shouldRestart=1
  else
    shouldRestart=0
    echo `date`" "$filenamefullpath" time is "$totalMins". So no need to restart." >> /var/tmp/NanexReconnectScript
  fi
fi

#restart process
if [ $shouldRestart -eq 1 ]
then
  pid=`ps -ef|grep -i $nanexDir/NxCoreAccess|grep -v grep|awk '{print $2}'`
  if [[ $pid ]]
  then
    echo `date`" Stopping process: "$pid". Restarting process again." >> /var/tmp/NanexReconnectScript
    kill -SIGINT $pid
    cd $nanexDir
    nohup $nanexDir/NxCoreAccess -n 6220 -u Generation3Trading -p 62Gt0 &
  else
    echo `date`" Could not find process. Looks like process is not running" >> /var/tmp/NanexReconnectScript
  fi
fi
