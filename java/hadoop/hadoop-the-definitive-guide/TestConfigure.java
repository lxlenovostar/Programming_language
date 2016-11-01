package org.apache.hadoop.examples;

import static org.hamcrest.Matchers.is;
import static org.hamcrest.MatcherAssert.assertThat;

import org.apache.hadoop.conf.Configuration;

public class TestConfigure {
	public static void main(String[] args) {
		Configuration conf = new Configuration();
		conf.addResource("configuration-1.xml");
		assertThat(conf.get("color"), is("yellow"));
		assertThat(conf.getInt("size", 0), is(10));
		assertThat(conf.get("breadth", "wide"), is("wide"));
		assertThat(conf.get("size-weight"), is("10,heavy"));
		
		System.setProperty("size", "14");
		assertThat(conf.get("size-weight"), is("14,heavy"));
		
		System.setProperty("length", "2");
		assertThat(conf.get("length"), is((String) null));
	}
}
