package org.apache.hadoop.examples;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;
import java.io.IOException;
import java.util.Set;
import java.util.HashSet;

/** 
 * LocationCountReducer implements the reduce() function for counting locations.
 *
 * @author Mahmoud Parsian
 *
 */
public  class LocationCountReducer 
	extends Reducer<Text, Text, Text, LongWritable> {

	//TODO 利用反射技术，打印这个类成员变量的属性
	//Set<String> set = new HashSet<String>();
	
    @Override
    public void reduce(Text productID, Iterable<Text> locations, Context context)
        throws  IOException, InterruptedException {
    	Set<String> set = new HashSet<String>();
    	
        for (Text location: locations) {
           set.add(location.toString());
           System.out.println("reducer:"+productID.toString() + ":" + location.toString());
        }
        
        for (String str : set) {
        	System.out.println(productID.toString() + ":" + str);	
        }
        
        context.write(productID, new LongWritable(set.size()));
    }
}
