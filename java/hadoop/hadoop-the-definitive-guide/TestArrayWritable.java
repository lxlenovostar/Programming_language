package org.apache.hadoop.examples;

import org.apache.hadoop.io.ArrayWritable;
import org.apache.hadoop.io.Text;

public class TestArrayWritable extends ArrayWritable {
	public TestArrayWritable() {
		super(Text.class);
		
	}
	
  
	public static void main(String[] args) {
		System.out.println("hello");
	}
	
}
