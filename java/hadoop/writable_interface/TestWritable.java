package org.apache.hadoop.examples;

import java.io.*;

import org.apache.hadoop.util.StringUtils;
import org.apache.hadoop.io.*;
import org.hamcrest.core.Is;
import static  org.junit.Assert.*;  
import org.hamcrest.*;

public class TestWritable {
	public IntWritable writable;
	
	public TestWritable(int newint) {
		writable = new IntWritable(newint); 
	}
	
	public byte[] serialize(Writable writable) throws IOException {
		ByteArrayOutputStream out = new ByteArrayOutputStream();
		DataOutputStream dataOut = new DataOutputStream(out);
		writable.write(dataOut);
		dataOut.close();
		
		return out.toByteArray();
	}
	
	public byte[] deserialize(Writable writable, byte[] bytes) throws IOException {
		ByteArrayInputStream in = new ByteArrayInputStream(bytes);
		DataInputStream dataIn = new DataInputStream(in);
		writable.readFields(dataIn);
		dataIn.close();
			
		return bytes;
	}
	
	public static void main(String[] args) throws IOException {
		TestWritable tw = new TestWritable(163);
		
		byte[] bytes = tw.serialize(tw.writable);
		
		assertThat(bytes.length, Is.is(4));
		assertThat(StringUtils.byteToHexString(bytes), CoreMatchers.is("000000a3"));
		
		IntWritable newWritable = new IntWritable();
		tw.deserialize(newWritable, bytes);
		
		assertThat(newWritable.get(), Is.is(163));
			
		
		System.out.println("hi");
	}
}
