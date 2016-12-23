package org.apache.hadoop.examples;

// cc WholeFileRecordReader The RecordReader used by WholeFileInputFormat for reading a whole file as a record
import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.BytesWritable;
import org.apache.hadoop.io.IOUtils;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.InputSplit;
import org.apache.hadoop.mapreduce.RecordReader;
import org.apache.hadoop.mapreduce.TaskAttemptContext;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;

//vv WholeFileRecordReader
class WholeFileRecordReader extends RecordReader<NullWritable, BytesWritable> {
  
  private FileSplit fileSplit;
  private Configuration conf;
  private BytesWritable value = new BytesWritable();
  private boolean processed = false;

  @Override
  public void initialize(InputSplit split, TaskAttemptContext context)
      throws IOException, InterruptedException {
	  this.fileSplit = (FileSplit) split;
	  this.conf = context.getConfiguration();
  }
  
  @Override
  public boolean nextKeyValue() throws IOException, InterruptedException {
    if (!processed) {
      /*  create a byte array whose length is the length of the file. */
      byte[] contents = new byte[(int) fileSplit.getLength()];
      Path file = fileSplit.getPath();
      /*
       * public FileSystem getFileSystem(Configuration conf) throws IOException 
       * Return the FileSystem that owns this Path. 
       * Throws: 
       * IOException
       * */
      FileSystem fs = file.getFileSystem(conf);
      FSDataInputStream in = null;
      
      try {
        in = fs.open(file);
        /*
         * public static void readFully(InputStream in, byte[] buf, int off, int len) throws IOException
		 * Reads len bytes in a loop.
		 * Parameters:
		 * in - InputStream to read from
		 * buf - The buffer to fill
		 * off - offset from the buffer
		 * len - the length of bytes to read
         * */
        IOUtils.readFully(in, contents, 0, contents.length);
        /*
         * public void set(byte[] newData, int offset, int length) 
         * Set the value to a copy of the given byte range
		 * Parameters:
		 * newData - the new values to copy in
		 * offset - the offset in newData to start at
		 * length - the number of bytes to copy
         * */
        value.set(contents, 0, contents.length);
      } finally {
        IOUtils.closeStream(in);
      }
      processed = true;
      return true;
    }
    return false;
  }
  
  @Override
  public NullWritable getCurrentKey() throws IOException, InterruptedException {
    return NullWritable.get();
  }

  @Override
  public BytesWritable getCurrentValue() throws IOException,
      InterruptedException {
    return value;
  }

  /*
   * float getProgress() throws IOException 
   * 
   * How much of the input has the RecordReader consumed i.e. has been processed by?
   * Returns:
   * progress from 0.0 to 1.0.
   * Throws:
   * IOException
   * */
  @Override
  public float getProgress() throws IOException {
    return processed ? 1.0f : 0.0f;
  }

  @Override
  public void close() throws IOException {
    // do nothing
  }
}
//^^ WholeFileRecordReader
