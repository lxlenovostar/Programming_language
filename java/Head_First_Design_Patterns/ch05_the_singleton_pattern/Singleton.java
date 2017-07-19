public class Singleton {
	//We hava a static variable to hold our one instance 
	//of the class Singleton.
	private static Singleton uniqueInstance;
	private int count;

	private int add() {
		count += 1;
		return count;
	}

	//Our constructor is declared private;
	//only Singleton can instantiate this class.
	private Singleton() {
		count = 1;
	}

	//The getInstance() method give us a way to 
	//instantiate the class and also to return 
	//an instance of it.
	public static Singleton getInstance() {
		if (uniqueInstance == null) {
			uniqueInstance = new Singleton();
		}

		return uniqueInstance;
	}


	public static void main(String []args)
	{
		Singleton i1 = Singleton.getInstance();
		System.out.println(i1.add());
		Singleton i2 = Singleton.getInstance();
		System.out.println(i2.add());
	}
};

