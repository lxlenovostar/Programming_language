package org.apache.hadoop.examples;
 
import java.io.IOException;
import java.util.StringTokenizer;
 
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.util.GenericOptionsParser;
 
public class WordCount {
 
  public static class TokenizerMapper extends Mapper<Object, Text, Text, IntWritable>{
 
    private final static IntWritable one = new IntWritable(1);
    
    private Text word = new Text();
 
    public void map(Object key, Text value, Context context) throws IOException, InterruptedException {
    	/*
    	 * Constructs a string tokenizer for the specified string. The tokenizer uses the default delimiter set, which is " \t\n\r\f": 
    	 * the space character, the tab character, the newline character, the carriage-return character, and the form-feed character. 
    	 * Delimiter characters themselves will not be treated as tokens.
    	 * */
    	StringTokenizer itr = new StringTokenizer(value.toString());
      
    	/*
    	 * Tests if there are more tokens available from this tokenizer's string. If this method returns true, then a subsequent call
    	 * to nextToken with no argument will successfully return a token.
    	 * */
    	while (itr.hasMoreTokens()) {
    		/*
    		 * Set to contain the contents of a string.
    		 * */
    		word.set(itr.nextToken());
    		context.write(word, one);
    	}
    }
  }  
 
  public static class IntSumReducer extends Reducer<Text,IntWritable,Text,IntWritable> {
    private IntWritable result = new IntWritable();
 
    public void reduce(Text key, Iterable<IntWritable> values, Context context) throws IOException, InterruptedException {
    	int sum = 0;
      
    	for (IntWritable val : values) {
    		sum += val.get();
    	}
    	
    	result.set(sum);
    	context.write(key, result);
    }
  }
 
  public static void main(String[] args) throws Exception {
	  Configuration conf = new Configuration();
	  String[] otherArgs = new GenericOptionsParser(conf, args).getRemainingArgs();
    
	  if (otherArgs.length != 2) {
		  System.err.println("Usage: wordcount <in> <out>");
		  System.exit(2);
	  }
    
	  Job job = new Job(conf, "word count");
	  
	  job.setJarByClass(WordCount.class);
	  job.setMapperClass(TokenizerMapper.class);
	  job.setCombinerClass(IntSumReducer.class);
	  job.setReducerClass(IntSumReducer.class);
	  
	  job.setOutputKeyClass(Text.class);
	  job.setOutputValueClass(IntWritable.class);
    
	  FileInputFormat.addInputPath(job, new Path(otherArgs[0]));
	  FileOutputFormat.setOutputPath(job, new Path(otherArgs[1]));
    
	  System.exit(job.waitForCompletion(true) ? 0 : 1);
  }
}