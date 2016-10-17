/* learn PrintWriter */
import java.io.*;

public class testPrintWriter
{
    public static void main(String[] args) 
    {
		try {
			//PrintWriter out = new PrintWriter("employee.txt");
			PrintWriter out = new PrintWriter(new FileWriter("employee.txt"), true);
	
			String name = "Harry Hacker";
			double salary = 75000;
			out.print(name);
			out.print(' ');
			out.println(salary);

			//out.close();
		//} catch(FileNotFoundException e) {
		} catch(IOException e) {
			System.out.println(e);
		}
    }
}
