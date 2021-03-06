package org.apache.hadoop.examples;

import java.io.IOException;
import java.util.Map;
import java.util.SortedMap;
import java.util.TreeMap;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.io.LongWritable;

/**
 *  Mapper's input are read from SequenceFile and records are: (K, V)
 *    where 
 *          K is a Text
 *          V is an Integer
 * 
 * @author Mahmoud Parsian
 *
 */
public class TopNMapper extends
	Mapper<LongWritable, Text, NullWritable, Text> {
   //Mapper<Text, IntWritable, NullWritable, Text> {

   private int N = 10; // default
   private SortedMap<Integer, String> top = new TreeMap<Integer, String>();

   @Override
   //public void map(Text key, IntWritable value, Context context)
   public void map(LongWritable key, Text value, Context context)
         throws IOException, InterruptedException {

	  /* TODO
      String keyAsString = key.toString();
      int frequency =  value.get();
      
      String compositeValue = keyAsString + "," + frequency;
      */
	  String line = value.toString();
	  String[] tokens = line.split(",");
	  
	  int frequency = Integer.parseInt(tokens[0]);
	  String compositeValue = line;
	  
      top.put(frequency, compositeValue);
      
      // keep only top N
      if (top.size() > N) {
         top.remove(top.firstKey());
      }
   }
   
   /* the setup() function, which will be executed once per mapper */
   @Override
   protected void setup(Context context) throws IOException, InterruptedException {
	  /*
	   * A Configuration object encapsulates a client or server’s configuration, which is set using
       * configuration files read from the classpath, such as etc/hadoop/core-site.xml.
	   * */
      this.N = context.getConfiguration().getInt("N", 10); // default is top 10
   }
   
   /*  cleanup() function will be executed once at the end of each mapper */
   @Override
   protected void cleanup(Context context) throws IOException, InterruptedException {
      for (String str : top.values()) {
         context.write(NullWritable.get(), new Text(str));
      }
   }

}
