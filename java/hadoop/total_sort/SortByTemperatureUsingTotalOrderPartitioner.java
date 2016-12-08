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
     *  The most basic random sample is called a simple random sample, and which is equivalent to using
     *   a raffle to select cases. This means that each case in the population has an equal chance of being 
     *   included and there is no implied connection between the cases in the sample.
     * */

    /*
     * public class InputSampler<K,V> extends Configured implements Tool 
     * 
     * Utility for collecting samples and writing a partition file for TotalOrderPartitioner.
     * */
    /*
     * public InputSampler.RandomSampler(double freq, int numSamples, int maxSplitsSampled) 
     * 
     * Create a new RandomSampler. 
     * Parameters:
     * freq - Probability with which a key will be chosen.
     * numSamples - Total number of samples to obtain from all selected splits.
     * maxSplitsSampled - The maximum number of splits to examine.
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
    /*
     * A range partitioner will distribute map outputs based on a predefined range of values, 
     * where each range maps to a reducer that will receive all outputs within that range. 
     * This is exactly how the TotalOrderPartitioner works.
     * For range partitioners such as the TotalOrderPartitioner to do their work, they need 
     * to know the output key ranges for a given job. The TotalOrderPartitioner is accompanied by
     * a sampler that samples the input data and writes these ranges to HDFS, which 
     * is then used by the TotalOrderPartitioner when partitioning. 
     * */
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
