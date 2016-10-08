#!/bin/bash  

function walk()  
{  
  for file in `ls $1`  
  do  
    local path=$1"/"$file  
    if [ -d $path ]  
     then  
      echo "DIR $path"  
      git add *.c *.h
      walk $path  
    #else  
      #echo "FILE $path"  
    fi  
  done  
}  

if [ $# -ne 1 ]  
then  
  echo "USAGE: sh $0 TOP_DIR"  
else  
  walk $1  
fi  

git commit -m "add/update file"
git push 
