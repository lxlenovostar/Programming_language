/**
  * Created by lix on 16-11-5.
  */
object MyModule {
	def abs(n: Int): Int =
    	if (n < 0) -n
    	else n

  	def factorial(n: Int): Int = {
    	def go(n: Int, acc: Int): Int =
        	if (n <= 0) acc
            else go(n-1, n*acc)

        go(n, 1)
    }

	private def formatAbs(x: Int) = {
    	val msg = "The absolute value of %d is %d"
    	msg.format(x, abs(x))
  	}

	private def formatFactorial(n: Int) = {
		val msg = "The factorial of %d is %d."
		msg.format(n, factorial(n))
	}

	def formatResult(name: String, n: Int, f: Int => Int) = {
		val msg = "The %s of %d is %d."
		msg.format(name, n, f(n))
	}

	def findFirst(ss: Array[String], key: String): Int = {
		@annotation.tailrec
		def loop(n: Int): Int =
			if (n >= ss.length) -1
			else if (ss(n) == key) n
			else loop(n + 1)

		loop(0)
	}

	def findFirst[A](as: Array[A], p: A => Boolean): Int = {
		@annotation.tailrec
		def loop(n: Int): Int =
			if (n >= as.length) -1
			else if (p(as(n))) n
			else loop(n + 1)

		loop(0)
	}

	def main(args: Array[String]): Unit =
    	println(formatAbs(-42))

}


