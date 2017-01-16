package org.apache.hadoop.examples;

import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.io.Text;

import java.io.IOException;

/** 
 * This is an identity mapper.
 * LocationCountMapper implements the map() function for counting locations.
 *
 * @author Mahmoud Parsian
 *
 */
public class LocationCountMapper 
	extends Mapper<Text, Text, Text, Text> {

    @Override
    public void map(Text key, Text value, Context context) 
    throws IOException, InterruptedException {
       System.out.println("mapper:"+key.toString() + ":" + value.toString());	
       context.write(key, value);
    }
}
