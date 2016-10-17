/* learn PrintWriter */
import java.io.*;

public class testSerialize
{
    public static void main(String[] args) throws IOException 
    {
		ObjectOutputStream out = new ObjectOutputStream(new FileOutputStream("employee.dat"));
		
		Employee harry = new Employee("Harry Hacker", 50000, 1989, 10, 1);
		Manager boss = new Manager("Carl Cracker", 80000, 1987, 12, 15);

		out.writeObject(harry);
		out.writeObject(boss);

		ObjectInputStream in = new ObjectInputStream(new FileInputStream("employee.dat"));
		
		Employee e1 = (Employee) in.readObject();
		Employee e2 = (Employee) in.readObject();
    }
}
