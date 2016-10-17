/* learn PrintWriter */
import java.io.*;

public class testBufferedReader
{
    public static void main(String[] args) throws IOException 
    {
			BufferedReader in = new BufferedReader(new InputStreamReader(new FileInputStream("employee.txt"), "UTF-8"));

			String line;
			while ((line = in.readLine()) != null)
			{
				System.out.println(line);
			}
    }
}
