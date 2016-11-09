// cc MinimalMapReduceWithDefaults A minimal MapReduce driver, with the defaults explicitly set
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.mapreduce.lib.partition.HashPartitioner;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

//vv MinimalMapReduceWithDefaults
public class MinimalMapReduceWithDefaults extends Configured implements Tool {
  
  @Override
  public int run(String[] args) throws Exception {
    Job job = JobBuilder.parseInputAndOutput(this, getConf(), args);
    if (job == null) {
      return -1;
    }
    
    /*
     * The default input format is TextInputFormat, which produces keys of type LongWritable
     * (the offset of the beginning of the line in the file) and values of type Text (the line of text).
     * */
    job.setInputFormatClass(TextInputFormat.class);
    
    /*
     * The default mapper is just the Mapper class, which writes the input key and value unchanged to the output.
     * */
    job.setMapperClass(Mapper.class);
    
    job.setMapOutputKeyClass(LongWritable.class);
    job.setMapOutputValueClass(Text.class);
    
    /*
     * The default partitioner is HashPartitioner, which hashes a record’s key to determine
     * which partition the record belongs in. Each partition is processed by a reduce task, so the
     * number of partitions is equal to the number of reduce tasks for the job.
     * */
    job.setPartitionerClass(HashPartitioner.class);
    
    /*
     * You may have noticed that we didn’t set the number of map tasks. The reason for this is
     * that the number is equal to the number of splits that the input is turned into, which is
     * driven by the size of the input and the file’s block size (if the file is in HDFS).
     * */
    job.setNumReduceTasks(1);
    
    /*
     * The default reducer is Reducer, again a generic type, which simply writes all its input to its output.
     * */
    job.setReducerClass(Reducer.class);

    job.setOutputKeyClass(LongWritable.class);
    job.setOutputValueClass(Text.class);
    
    /*
     * The default output format is TextOutputFormat, which writes out records, one per line, by
     * converting keys and values to strings and separating them with a tab character. This is why
     * the output is tab-separated: it is a feature of TextOutputFormat.
     */
    job.setOutputFormatClass(TextOutputFormat.class);/*]*/
    
    return job.waitForCompletion(true) ? 0 : 1;
  }
  
  public static void main(String[] args) throws Exception {
    int exitCode = ToolRunner.run(new MinimalMapReduceWithDefaults(), args);
    System.exit(exitCode);
  }
}
// ^^ MinimalMapReduceWithDefaults
