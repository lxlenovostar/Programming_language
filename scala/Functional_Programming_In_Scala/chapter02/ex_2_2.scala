object ex_2_2 {
	/*
     we introduce a comma-separated list of type parameters, 
	 surrounded by square brackets (here, just a single [A]), 
	 following the name of the function.
     */
	def isSorted[A](as: Array[A], gt: (A,A) => Boolean): Boolean = {
    	@annotation.tailrec
    	def go(n: Int): Boolean =
      		if (n >= as.length-1) true
      		else if (gt(as(n), as(n+1))) false
      		else go(n+1)

    	go(0)
  	}
}


