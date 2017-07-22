//package headfirst.designpatterns.singleton.threadsafe;

public class Singleton {
	//private static Singleton uniqueInstance;
	private volatile static Singleton uniqueInstance;
 
	// other useful instance variables here
 
	private Singleton() {}
 
	public static synchronized Singleton getInstance() {
		// use "double-checked locking" to reduce the 
		// use of synchronization in getInstance()
		if (uniqueInstance == null) {
			synchronized(Singleton.class) {
				if (uniqueInstance == null) {
					uniqueInstance = new Singleton();
				}
			}
		}
		return uniqueInstance;

		/*
		if (uniqueInstance == null) {
			uniqueInstance = new Singleton();
		}
		return uniqueInstance;
		*/
	}
 
	// other useful methods here
	public String getDescription() {
		return "I'm a thread safe Singleton!";
	}
}
