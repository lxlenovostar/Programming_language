package org.apache.hadoop.examples;

import org.apache.log4j.Logger;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;

//import org.apache.hadoop.examples.HadoopUtil;

/**
 * TopNDriver: assumes that all K's are unique for all given (K,V) values.
 * Uniqueness of keys can be achieved by using AggregateByKeyDriver job.
 *
 * @author Mahmoud Parsian
 *
 */
public class TopNDriver  extends Configured implements Tool {

   private static Logger THE_LOGGER = Logger.getLogger(TopNDriver.class);

   public int run(String[] args) throws Exception {
	  /*
	   * It allows the user to configure the job, submit it, control its execution, and query the state. 
	   * The set methods only work until the job is submitted, afterwards they will throw an IllegalStateException.
	   * 
	   * Normally the user creates the application, describes various facets of the job via Job and then submits 
	   * the job and monitor its progress.
	   * */
	   /*
	    * public Configuration getConf()
	    * 
		* Description copied from interface: Configurable  
		* Return the configuration used by this object.
	    * */
	   // 构造函数　Job(Configuration conf)
      Job job = new Job(getConf());
      
      //HadoopUtil.addJarsToDistributedCache(job, "/lib/");
      
      int N = Integer.parseInt(args[0]); // top N
      
      /*
       * public interface Configurable
       * 
	   * Something that may be configured with a Configuration.
       * */
      
      /*
       * public interface Tool extends Configurable
	   * 
	   * A tool interface that supports handling of generic command-line options.
	   * Tool, is the standard for any Map-Reduce tool/application. The tool/application 
	   * should delegate the handling of standard command-line options to ToolRunner.run(Tool, String[])
	   * and only handle its custom arguments.
       * */
      
      /*
       * public class Configured extends Object implements Configurable
	   *
	   * Base class for things that may be configured with a Configuration.
       * */
            
      /* 
       * public class Configuration extends Object implements Iterable<Map.Entry<String,String>>, Writable
       * 
       * Provides access to configuration parameters.
       * */
      /* Return the configuration for the job. */
      job.getConfiguration().setInt("N", N);
      job.setJobName("TopNDriver");

      /*
       先修改 TODO
      job.setInputFormatClass(SequenceFileInputFormat.class);
      job.setOutputFormatClass(SequenceFileOutputFormat.class);
	  */
      
      job.setMapperClass(TopNMapper.class);
      job.setReducerClass(TopNReducer.class);
      job.setNumReduceTasks(1);

      // map()'s output (K,V)
      job.setMapOutputKeyClass(NullWritable.class);   
      job.setMapOutputValueClass(Text.class);   
      
      // reduce()'s output (K,V)
      job.setOutputKeyClass(IntWritable.class);
      job.setOutputValueClass(Text.class);

       // args[1] = input directory
       // args[2] = output directory
      FileInputFormat.setInputPaths(job, new Path(args[1]));
      FileOutputFormat.setOutputPath(job, new Path(args[2]));

      boolean status = job.waitForCompletion(true);
      THE_LOGGER.info("run(): status="+status);
      return status ? 0 : 1;
   }

   /**
   * The main driver for "Top N" program.
   * Invoke this method to submit the map/reduce job.
   * @throws Exception When there is communication problems with the job tracker.
   */
   public static void main(String[] args) throws Exception {
      // Make sure there are exactly 3 parameters
      if (args.length != 3) {
         THE_LOGGER.warn("usage TopNDriver <N> <input> <output>");
         System.exit(1);
      }

      THE_LOGGER.info("N="+args[0]);
      THE_LOGGER.info("inputDir="+args[1]);
      THE_LOGGER.info("outputDir="+args[2]);
      /*
       * public class ToolRunner extends Object
       * 
       * ToolRunner can be used to run classes implementing Tool interface. It works in conjunction 
       * with GenericOptionsParser to parse the generic hadoop command line arguments and modifies
       * the Configuration of the Tool. The application-specific options are passed along without being modified.
       * 
       * public static int run(Tool tool, String[] args) throws Exception
	   * Runs the Tool with its Configuration. Equivalent to run(tool.getConf(), tool, args).
       * Parameters:
	   * tool - Tool to run.
       * args - command-line arguments to the tool.
       * */
      int returnStatus = ToolRunner.run(new TopNDriver(), args);
      System.exit(returnStatus);
   }

}