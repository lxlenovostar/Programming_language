import java.util.Arrays;

public class SortClient
{
    public static void main(String[] args) 
    {
        int N = Integer.parseInt(args[0]);
        String[] names = new String[N];

        for (int i = 0; i < N; i++)
            names[i] = StdIn.readString();

        Arrays.sort(names);

        for (int i = 0; i < N; i++)
            StdOut.println(names[i]);
    }
}
