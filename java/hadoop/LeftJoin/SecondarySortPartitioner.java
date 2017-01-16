package org.apache.hadoop.examples;

import org.apache.hadoop.mapreduce.Partitioner;
import org.w3c.dom.Text;
//import edu.umd.cloud9.io.pair.PairOfStrings;

/** 
 * This is an plug-in class.
 * The SecondarySortPartitioner class indicates how to partition data.
 * 
 * @author Mahmoud Parsian
 *
 */

public class SecondarySortPartitioner extends Partitioner<PairOfStrings, Object> {
    @Override
    public int getPartition(PairOfStrings key, 
                            Object value, 
                            int numberOfPartitions) {
       return (key.getLeftElement().hashCode() & Integer.MAX_VALUE) % numberOfPartitions;
    }
}





