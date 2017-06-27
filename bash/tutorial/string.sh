#!/bin/bash

#长度
stringZ=abcABC123ABCabc
echo `expr length $stringZ` 

#匹配字符串开头的子串长度
echo `expr match "$stringZ" 'abc[A-Z]*.2'`

#子串查找
echo `expr index "$stringZ" C12`
echo `expr index "$stringZ" 1c`

#提取子串
echo ${stringZ:0}
echo ${stringZ:1} 
echo ${stringZ:7}
echo ${stringZ:7:3}
echo ${stringZ:(-4)}

#使用substr提取子串
echo `expr substr $stringZ 1 2`
echo `expr substr $stringZ 4 3`

#子串消除
echo ${stringZ#a*C} 	# 123ABCabc
echo ${stringZ##a*C} 	# abc
echo ${stringZ%b*c} 	# abcABC123ABCa
echo ${stringZ%%b*c} 	# a

#子串替换
echo ${stringZ/abc/xyz}  # xyzABC123ABCabc
echo ${stringZ//abc/xyz} # xyzABC123ABCxyz
echo ${stringZ/#abc/XYZ} # XYZABC123ABCabc
echo ${stringZ/%abc/XYZ} # abcABC123ABCXYZ
