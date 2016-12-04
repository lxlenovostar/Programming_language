package org.apache.hadoop.examples;

//SequenceFileWriteDemo Writing a SequenceFile
import java.io.IOException;
import java.net.URI;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IOUtils;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.SequenceFile;
import org.apache.hadoop.io.Text;

//SequenceFileWriteDemo
public class SequenceFileWriteDemo {
  
  private static final String[] DATA = {
    "One, two, buckle my shoe",
    "Three, four, shut the door",
    "Five, six, pick up sticks",
    "Seven, eight, lay them straight",
    "Nine, ten, a big fat hen"
  };
  
  public static void main(String[] args) throws IOException {
    String uri = args[0];
    Configuration conf = new Configuration();
    FileSystem fs = FileSystem.get(URI.create(uri), conf);
    Path path = new Path(uri);

    IntWritable key = new IntWritable();
    Text value = new Text();
    /*
     public class SequenceFile extends Object

	 SequenceFiles are flat files consisting of binary key/value pairs.
	 SequenceFile provides SequenceFile.Writer, SequenceFile.Reader and SequenceFile.Sorter classes for writing, reading and sorting respectively.

	 There are three SequenceFile Writers based on the SequenceFile.CompressionType used to compress key/value pairs:
	 Writer : Uncompressed records.
	 RecordCompressWriter : Record-compressed files, only compress values.
	 BlockCompressWriter : Block-compressed files, both keys & values are collected in 'blocks' separately and compressed. 
						  The size of the 'block' is configurable.
	 The actual compression algorithm used to compress key and/or values can be specified by using the appropriate CompressionCodec.
	 The recommended way is to use the static createWriter methods provided by the SequenceFile to chose the preferred format.

	 The SequenceFile.Reader acts as the bridge and can read any of the above SequenceFile formats.
     
     public static org.apache.hadoop.io.SequenceFile.Writer createWriter(FileSystem fs,
                                                               Configuration conf,
                                                               Path name,
                                                               Class keyClass,
                                                               Class valClass)
                                                             throws IOException
		Deprecated. Use createWriter(Configuration, Writer.Option...) instead.
		
		Construct the preferred type of SequenceFile Writer.
		Parameters:
		fs - The configured filesystem.
		conf - The configuration.
		name - The name of the file.
		keyClass - The 'key' type.
		valClass - The 'value' type.
		Returns:
		Returns the handle to the constructed SequenceFile Writer.
      
     * */
    
    SequenceFile.Writer writer = null;
    try {
      writer = SequenceFile.createWriter(fs, conf, path, key.getClass(), value.getClass());
      
      for (int i = 0; i < 100; i++) {
        key.set(100 - i);
        value.set(DATA[i % DATA.length]);
        System.out.printf("[%s]\t%s\t%s\n", writer.getLength(), key, value);
        writer.append(key, value);
      }
    } finally {
      IOUtils.closeStream(writer);
    }
  }
}
// ^^ SequenceFileWriteDemo
