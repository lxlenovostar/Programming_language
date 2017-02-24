object ex_2_1 {
	def fib(n: Int): Int = {
		def go(i: Int, acc: Int, bcc: Int): Int =
			if (i == n) acc
			else go(i+1, bcc, acc+bcc)

		go(0, 0, 1)
	}
}


