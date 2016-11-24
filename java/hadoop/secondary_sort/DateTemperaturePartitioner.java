package org.apache.hadoop.examples;


import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Partitioner;

/** 
 * The DateTemperaturePartitioner is a custom partitioner class,
 * whcih partitions data by the natural key only (using the yearMonth).
 * Without custom partitioner, Hadoop will partition your mapped data 
 * based on a hash code.
 *
 * In Hadoop, the partitioning phase takes place after the map() phase 
 * and before the reduce() phase
 *
 * @author Mahmoud Parsian
 *
 */
/*
 * Partitioner controls the partitioning of the keys of the intermediate map-outputs. The key 
 * (or a subset of the key) is used to derive the partition, typically by a hash function. The 
 * total number of partitions is the same as the number of reduce tasks for the job.
 * 
 * Note: A Partitioner is created only when there are multiple reducers.
 * Note: If you require your Partitioner class to obtain the Job's configuration object, implement the Configurable interface.
 * */
public class DateTemperaturePartitioner 
   extends Partitioner<DateTemperaturePair, Text> {

    @Override
    public int getPartition(DateTemperaturePair pair, Text text, int numberOfPartitions) {
    	// make sure that partitions are non-negative
        return Math.abs(pair.getYearMonth().hashCode() % numberOfPartitions);
    }
}
