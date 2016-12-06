package org.apache.hadoop.examples;

// cc SortByTemperatureUsingTotalOrderPartitioner A MapReduce program for sorting a SequenceFile with IntWritable keys using the TotalOrderPartitioner to globally sort the data
import java.net.URI;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.io.*;
import org.apache.hadoop.io.SequenceFile.CompressionType;
import org.apache.hadoop.io.compress.BZip2Codec;
//import org.apache.hadoop.io.compress.GzipCodec;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.mapreduce.lib.partition.InputSampler;
import org.apache.hadoop.mapreduce.lib.partition.TotalOrderPartitioner;
import org.apache.hadoop.util.*;
import org.apache.hadoop.io.compress.BZip2Codec;

// vv SortByTemperatureUsingTotalOrderPartitioner
public class SortByTemperatureUsingTotalOrderPartitioner extends Configured
  implements Tool {
  
  @Override
  public int run(String[] args) throws Exception {
    Job job = JobBuilder.parseInputAndOutput(this, getConf(), args);
    if (job == null) {
      return -1;
    }
    
    job.setInputFormatClass(SequenceFileInputFormat.class);
    job.setOutputFormatClass(SequenceFileOutputFormat.class);
    
    job.setOutputKeyClass(IntWritable.class);
    
    SequenceFileOutputFormat.setCompressOutput(job, true);
    SequenceFileOutputFormat.setOutputCompressorClass(job, BZip2Codec.class);
    //SequenceFileOutputFormat.setOutputCompressorClass(job, GzipCodec.class);
    SequenceFileOutputFormat.setOutputCompressionType(job, CompressionType.BLOCK);

    /*
     * public class TotalOrderPartitioner<K extends WritableComparable<?>,V> extends Partitioner<K,V> implements Configurable
     * 
     * Partitioner effecting a total order by reading split points from an externally generated source.
     * 
     * */
    job.setPartitionerClass(TotalOrderPartitioner.class);

    /*
     * public class InputSampler<K,V> extends Configured implements Tool 
     * 
     * Utility for collecting samples and writing a partition file for TotalOrderPartitioner.
     * */
    InputSampler.Sampler<IntWritable, Text> sampler = new InputSampler.RandomSampler<IntWritable, Text>(0.1, 10000, 10);
    
    /*
     * public static <K,V> void writePartitionFile(Job job,
                            org.apache.hadoop.mapreduce.lib.partition.InputSampler.Sampler<K,V> sampler)
                               throws IOException,
                                      ClassNotFoundException,
                                      InterruptedException
     * Write a partition file for the given job, using the Sampler provided. Queries the sampler for 
     * a sample keyset, sorts by the output key comparator, selects the keys for each rank, and writes 
     * to the destination returned from TotalOrderPartitioner.getPartitionFile(org.apache.hadoop.conf.Configuration).
     * */
    InputSampler.writePartitionFile(job, sampler);

    // Add to DistributedCache
    Configuration conf = job.getConfiguration();
    String partitionFile = TotalOrderPartitioner.getPartitionFile(conf);
    URI partitionUri = new URI(partitionFile);
    job.addCacheFile(partitionUri);

    return job.waitForCompletion(true) ? 0 : 1;
  }
  
  public static void main(String[] args) throws Exception {
    int exitCode = ToolRunner.run(
        new SortByTemperatureUsingTotalOrderPartitioner(), args);
    System.exit(exitCode);
  }
}
// ^^ SortByTemperatureUsingTotalOrderPartitioner
