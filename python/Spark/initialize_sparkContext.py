#!/usr/bin/python
# -*- coding: utf-8 -*-  

from pyspark import SparkConf, SparkContext

# • A cluster URL, namely local in these examples, which tells Spark how to connect
# to a cluster. local is a special value that runs Spark on one thread on the local
# machine, without connecting to a cluster.
# • An application name, namely My App in these examples. This will identify your
# application on the cluster manager’s UI if you connect to a cluster.
conf = SparkConf().setMaster("local").setAppName("My App")
sc = SparkContext(conf = conf)
