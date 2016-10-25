package org.apache.hadoop.examples;

//cc MaxTemperature Application to find the maximum temperature in the weather dataset
//vv MaxTemperature
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

public class MaxTemperature {

	public static void main(String[] args) throws Exception {
		if (args.length != 2) {
			System.err.println("Usage: MaxTemperature <input path> <output path>");
			System.exit(-1);
		}   
 
		/*
		 A Job object forms the specification of the job and gives you control over how the 
		 job is run. When we run this job on a Hadoop cluster, we will package the code into
		 a JAR file (which Hadoop will distribute around the cluster). Rather than explicitly 
		 specifying the	name of the JAR file, we can pass a class in the Job’s setJarByClass() 
		 method, which	Hadoop will use to locate the relevant JAR file by looking for the JAR
		 file containing this class.
		 */
		Job job = new Job();
		job.setJarByClass(MaxTemperature.class);
		job.setJobName("Max temperature");

		/*
		 Having constructed a Job object, we specify the input and output paths. An input path is
		 specified by calling the static addInputPath() method on FileInputFormat, and it can be
		 a single file, a directory (in which case, the input forms all the files in that directory), or a
		 file pattern. As the name suggests, addInputPath() can be called more than once to use
		 input from multiple paths.
		 */
		FileInputFormat.addInputPath(job, new Path(args[0]));
		/*
		 The output path (of which there is only one) is specified by the static setOutputPath()
		 method on FileOutputFormat. It specifies a directory where the output files from the
		 reduce function are written. The directory shouldn’t exist before running the job because
		 Hadoop will complain and not run the job. This precaution is to prevent data loss (it can
		 be very annoying to accidentally overwrite the output of a long job with that of another).
		 */
		FileOutputFormat.setOutputPath(job, new Path(args[1]));
 
		/*
		 we specify the map and reduce types to use via the setMapperClass() and setReducerClass() methods.
		 */
		job.setMapperClass(MaxTemperatureMapper.class);
		job.setReducerClass(MaxTemperatureReducer.class);

		/*
		 The setOutputKeyClass() and setOutputValueClass() methods control the output types
		 for the reduce function, and must match what the Reduce class produces. The map output
		 types default to the same types, so they do not need to be set if the mapper produces the
		 same types as the reducer (as it does in our case). However, if they are different, the map
		 output types must be set using the setMapOutputKeyClass() and setMapOutputValueClass() methods.
		 */
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(IntWritable.class);
 
		/*
		 The waitForCompletion() method on Job submits the job and waits for it to finish.
		 The single argument to the method is a flag indicating whether verbose output is
		 generated. When true, the job writes information about its progress to the console.
		 The return value of the waitForCompletion() method is a Boolean indicating success
		 (true) or failure (false), which we translate into the program’s exit code of 0 or 1
		 */
		System.exit(job.waitForCompletion(true) ? 0 : 1); 
	}
}
//^^ MaxTemperature

