package org.apache.hadoop.examples;

import java.io.*;

import org.apache.hadoop.util.StringUtils;
import org.apache.hadoop.io.*;

import static org.hamcrest.Matchers.greaterThan;
import static org.hamcrest.Matchers.is;
import static org.hamcrest.MatcherAssert.assertThat;

import java.nio.ByteBuffer;


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
	
	public void string() throws UnsupportedEncodingException {
		String s = "\u0041\u00DF\u6771\uD801\uDC00";
		
		assertThat(s.length(), is(5));
		
		assertThat(s.getBytes("UTF-8").length, is(10));
		
		assertThat(s.indexOf("\u0041"), is(0));
		assertThat(s.indexOf("\u00DF"), is(1));
		assertThat(s.indexOf("\u6771"), is(2));
		assertThat(s.indexOf("\uD801\uDC00"), is(3));
		
		assertThat(s.charAt(0), is('\u0041'));
		assertThat(s.charAt(1), is('\u00DF'));
		assertThat(s.charAt(2), is('\u6771'));
		assertThat(s.charAt(3), is('\uD801'));
		assertThat(s.charAt(4), is('\uDC00'));
		
		assertThat(s.codePointAt(0), is(0x0041));
		assertThat(s.codePointAt(1), is(0x00DF));
		assertThat(s.codePointAt(2), is(0x6771));
		assertThat(s.codePointAt(3), is(0x10400));
	}
	
	public void text() {
		Text t = new Text("\u0041\u00DF\u6771\uD801\uDC00");
		
		assertThat(t.getLength(), is(10));
		
		assertThat(t.find("\u0041"), is(0));
		assertThat(t.find("\u00DF"), is(1));
		assertThat(t.find("\u6771"), is(3));
		assertThat(t.find("\uD801\uDC00"), is(6));
		
		assertThat(t.charAt(0), is(0x0041));
		assertThat(t.charAt(1), is(0x00DF));
		assertThat(t.charAt(3), is(0x6771));
		assertThat(t.charAt(6), is(0x10400));
	}
	
	public static void main(String[] args) throws IOException {
		/*
		 * The Writable Interface
		 * */
		TestWritable tw = new TestWritable(163);
		
		byte[] bytes = tw.serialize(tw.writable);
		
		assertThat(bytes.length, is(4));
		assertThat(StringUtils.byteToHexString(bytes), is("000000a3"));
		
		IntWritable newWritable = new IntWritable();
		tw.deserialize(newWritable, bytes);
		
		assertThat(newWritable.get(), is(163));
			
		System.out.println("hi");
		
		//Get a comparator for a WritableComparable implementation.
		RawComparator<IntWritable> comparator = WritableComparator.get(IntWritable.class);
		
		IntWritable w1 = new IntWritable(163);
		IntWritable w2 = new IntWritable(67);
		assertThat(comparator.compare(w1, w2), greaterThan(0));
		
		byte[] b1 = tw.serialize(w1);
		byte[] b2 = tw.serialize(w2);
		assertThat(comparator.compare(b1, 0, b1.length, b2, 0, b2.length), greaterThan(0));
		
		/*
		 * Writable Classes
		 **/
		Text t = new Text("hadoop");
		assertThat(t.getLength(), is(6));
		assertThat(t.getBytes().length, is(6));
		
		assertThat(t.charAt(2), is((int) 'd'));
		assertThat("Out of bounds", t.charAt(100), is(-1));
		
		assertThat("Find a substring", t.find("do"), is(2));
		assertThat("Finds first 'o'", t.find("o"), is(3));
		assertThat("Finds 'o' from position 4 or later", t.find("o", 4), is(4));
		assertThat("No match", t.find("pig"), is(-1));
		
		/*
		 * Iteration
		 * */
		Text t1 = new Text("\u0041\u00DF\u6771\uD801\uDC00");
		ByteBuffer buf = ByteBuffer.wrap(t1.getBytes(), 0, t1.getLength());
		int cp;
		while (buf.hasRemaining() && (cp = Text.bytesToCodePoint(buf)) != -1) {
		System.out.println(Integer.toHexString(cp));
		}
		
		/*
		 *  Mutability
		 * */
		Text t2 = new Text("hadoop");
		t2.set("pig");
		assertThat(t2.getLength(), is(3));
		assertThat(t2.getBytes().length, is(3));
		
		/*
		 * BytesWritable
		 **/
		BytesWritable b = new BytesWritable(new byte[] { 3, 5 });
		byte[] bytes1 = tw.serialize(b);
		assertThat(StringUtils.byteToHexString(bytes1), is("000000020305"));
		b.setCapacity(11);
		assertThat(b.getLength(), is(2));
		assertThat(b.getBytes().length, is(11));
		
		MapWritable src = new MapWritable();
		src.put(new IntWritable(1), new Text("cat"));
		src.put(new VIntWritable(2), new LongWritable(163));
		
		MapWritable dest = new MapWritable();
		WritableUtils.cloneInto(dest, src);
		
		assertThat((Text) dest.get(new IntWritable(1)), is(new Text("cat")));
		assertThat((LongWritable) dest.get(new VIntWritable(2)), is(new LongWritable(163)));
	}
}
