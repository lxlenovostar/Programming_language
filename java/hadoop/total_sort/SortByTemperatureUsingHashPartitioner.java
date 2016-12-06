package org.apache.hadoop.examples;

// cc SortByTemperatureUsingHashPartitioner A MapReduce program for sorting a SequenceFile with IntWritable keys using the default HashPartitioner
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.SequenceFile.CompressionType;
//import org.apache.hadoop.io.compress.GzipCodec;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.hadoop.io.compress.BZip2Codec;

// vv SortByTemperatureUsingHashPartitioner
public class SortByTemperatureUsingHashPartitioner extends Configured
  implements Tool {
  
  @Override
  public int run(String[] args) throws Exception {
    Job job = JobBuilder.parseInputAndOutput(this, getConf(), args);
    if (job == null) {
      return -1;
    }
    
    /*
     * public void setInputFormatClass(Class<? extends InputFormat> cls) throws IllegalStateException
	 * Set the InputFormat for the job.

	 * Parameters:
	 * cls - the InputFormat to use
	 * Throws: IllegalStateException - if the job is submitted
	 * 
	 * public abstract class InputFormat<K,V> extends Object
	 * 
	 * InputFormat describes the input-specification for a Map-Reduce job.
	 * The Map-Reduce framework relies on the InputFormat of the job to:
	 * 1. Validate the input-specification of the job.
	 * 2. Split-up the input file(s) into logical InputSplits, each of which is then assigned to an individual Mapper.
     * 3. Provide the RecordReader implementation to be used to glean input records from the logical InputSplit for processing by the Mapper.
     * 
     * The default behavior of file-based InputFormats, typically sub-classes of FileInputFormat, is to split the input into logical 
     * InputSplits based on the total size, in bytes, of the input files. However, the FileSystem blocksize of the input files is 
     * treated as an upper bound for input splits. A lower bound on the split size can be set via mapreduce.input.fileinputformat.split.minsize.
     *
	 * Clearly, logical splits based on input-size is insufficient for many applications since record boundaries are to respected. 
	 * In such cases, the application has to also implement a RecordReader on whom lies the responsibility to respect record-boundaries 
	 * and present a record-oriented view of the logical InputSplit to the individual task.
	 * 
     * */
    job.setInputFormatClass(SequenceFileInputFormat.class);
    job.setOutputFormatClass(SequenceFileOutputFormat.class);
    
    /*
     * public void setOutputKeyClass(Class<?> theClass) throws IllegalStateException
	 * Set the key class for the job output data.
	 * 
	 * Parameters: 
	 * theClass - the key class for the job output data.
	 * Throws: IllegalStateException - if the job is submitted
     * */
    job.setOutputKeyClass(IntWritable.class);
    
    /* 控制切多少份 */
    job.setNumReduceTasks(4);
   
    SequenceFileOutputFormat.setCompressOutput(job, true);
    SequenceFileOutputFormat.setOutputCompressorClass(job, BZip2Codec.class);
    SequenceFileOutputFormat.setOutputCompressionType(job, CompressionType.BLOCK);
    
    return job.waitForCompletion(true) ? 0 : 1;
  }
  
  public static void main(String[] args) throws Exception {
    int exitCode = ToolRunner.run(new SortByTemperatureUsingHashPartitioner(), args);
    System.exit(exitCode);
  }
}
// ^^ SortByTemperatureUsingHashPartitioner
