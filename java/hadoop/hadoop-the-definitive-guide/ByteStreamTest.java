package org.apache.hadoop.examples;

import java.io.*;
public class ByteStreamTest {

   public static void main(String args[])throws IOException {
	  /* This constructor creates a ByteArrayOutputStream having buffer of the given size. */
      ByteArrayOutputStream bOutput = new ByteArrayOutputStream(12);

      /* Returns the current size of the buffer. */
      while( bOutput.size()!= 10 ) {
         // Gets the inputs from the user
    	 /* getBytes: This method encodes this String into a sequence of bytes using the platform's default charset,
    	  *  storing the result into a new byte array.*/
    	 /* write : Writes the specified array to the output stream. */
         bOutput.write("hello".getBytes());  
      }
      
      /* Creates a newly allocated byte array. */
      byte b [] = bOutput.toByteArray();
      System.out.println("Print the content");
      
      for(int x = 0; x < b.length; x++) {
         // printing the characters
         System.out.print((char)b[x]  + "   "); 
      }
      System.out.println("   ");

      int c;
      /* The ByteArrayInputStream class allows a buffer in the memory to be used as an InputStream. The input source is a byte array. */
      ByteArrayInputStream bInput = new ByteArrayInputStream(b);
      System.out.println("Converting characters to Upper case " );
      
      /* This method reads the next byte of data from the InputStream. Returns an int as the next byte of data. 
       * If it is the end of the file, then it returns -1. */
      while(( c = bInput.read())!= -1) {
    	 System.out.println(Character.toUpperCase((char)c));
      }
      
      /* Resets the buffer to the marked position. The marked position is 0 unless another position was marked 
       * or an offset was specified in the constructor.*/
      bInput.reset(); 
  }
}