package org.apache.hadoop.examples;

import java.io.*;
public class DataInput_Stream {

   public static void main(String args[])throws IOException {

      // writing string to a file encoded as modified UTF-8
	  /* A data output stream lets an application write primitive Java data types to an output stream in a portable way. 
	   * An application can then use a data input stream to read the data back in. */
      DataOutputStream dataOut = new DataOutputStream(new FileOutputStream("/home/lix/test/test.txt"));
      /* Writes a string to the underlying output stream using modified UTF-8 encoding in a machine-independent manner. */
      dataOut.writeUTF("hello");

      // Reading data from the same file
      /* A data input stream lets an application read primitive Java data types from an underlying input stream in a
       *  machine-independent way. An application uses a data output stream to write data that can later be read by a data input stream.*/
      DataInputStream dataIn = new DataInputStream(new FileInputStream("/home/lix/test/test.txt"));

      /* Returns an estimate of the number of bytes that can be read */
      while(dataIn.available()>0) {
         String k = dataIn.readUTF();
         System.out.print(k+" ");
      }
      
      dataOut.close();
      dataIn.close();
   }
}