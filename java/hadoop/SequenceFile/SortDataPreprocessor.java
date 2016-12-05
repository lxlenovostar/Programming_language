package org.apache.hadoop.examples;

//SortDataPreprocessor A MapReduce program for transforming the data into SequenceFile format
import java.io.IOException;

import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.SequenceFile.CompressionType;
import org.apache.hadoop.io.Text;
//import org.apache.hadoop.io.compress.GzipCodec;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.log4j.Logger;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.compress.BZip2Codec;

public class SortDataPreprocessor extends Configured implements Tool {
	
  private static Logger theLogger = Logger.getLogger(SecondarySortDriver.class);
  
  static class CleanerMapper
    extends Mapper<LongWritable, Text, IntWritable, Text> {
      
    @Override
    protected void map(LongWritable key, Text value, Context context)
        throws IOException, InterruptedException {
    	String line = value.toString();
        String[] tokens = line.split(",");
        /*
         12,cat1,cat1
		 13,cat2,cat2
		 14,cat3,cat3
		 15,cat4,cat4
		 10,cat5,cat5
         * */
        int out_key = Integer.parseInt(tokens[0]);
        context.write(new IntWritable(out_key), value);
      
    }
  }
  
  @Override
  public int run(String[] args) throws Exception {
	/* Configuration processed by ToolRunner */
  	Configuration conf = getConf();
	
	Job job = new Job(conf);
	
	job.setJarByClass(SecondarySortDriver.class);
	job.setJobName("SecondarySortDriver");

	/* Process custom command-line options */
	// args[0] = input directory
	// args[1] = output directory
	FileInputFormat.setInputPaths(job, new Path(args[0]));
	FileOutputFormat.setOutputPath(job, new Path(args[1]));

	/* Specify various job-specific parameters */
	job.setOutputKeyClass(IntWritable.class);
	job.setOutputValueClass(Text.class);
	
	job.setMapperClass(CleanerMapper.class);
    job.setNumReduceTasks(0);			
	
    job.setOutputFormatClass(SequenceFileOutputFormat.class);

    SequenceFileOutputFormat.setCompressOutput(job, true);
    //SequenceFileOutputFormat.setOutputCompressorClass(job, GzipCodec.class);
    SequenceFileOutputFormat.setOutputCompressorClass(job, BZip2Codec.class);
    SequenceFileOutputFormat.setOutputCompressionType(job, CompressionType.BLOCK);

	boolean status = job.waitForCompletion(true);
	
	theLogger.info("run(): status="+status);
	
	return status ? 0 : 1;
  }
  
 
  /**
   * The main driver for word count map/reduce program.
   * Invoke this method to submit the map/reduce job.
   * @throws Exception When there is communication problems with the job tracker.
   */
  public static void main(String[] args) throws Exception {
  	// Make sure there are exactly 2 parameters
  	if (args.length != 2) {
  		theLogger.warn("SecondarySortDriver <input-dir> <output-dir>");
  		throw new IllegalArgumentException("SecondarySortDriver <input-dir> <output-dir>");
  	}

  	//String inputDir = args[0];
  	//String outputDir = args[1];
  	int returnStatus = submitJob(args);
  	theLogger.info("returnStatus="+returnStatus);
	
  	System.exit(returnStatus);
  }


  /**
   * The main driver for word count map/reduce program.
   * Invoke this method to submit the map/reduce job.
   * @throws Exception When there is communication problems with the job tracker.
   */
  public static int submitJob(String[] args) throws Exception {
  	//String[] args = new String[2];
  	//args[0] = inputDir;
  	//args[1] = outputDir;
  	/* Runs the Tool with its Configuration. */
  	int returnStatus = ToolRunner.run(new SortDataPreprocessor(), args);
  	return returnStatus;
  }
}

