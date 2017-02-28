object ex_2_3_4_5 {
	def partial1[A,B,C](a: A, f: (A,B) => C): B => C =
		(b: B) => f(a, b)

	/*
	 You have to take an argument of type `A` and return a function of 
	 type `B => C`. That function has to take an argument of type `B` 
	 and return a value of type `C`. Follow the types.
	 */
	def curry[A,B,C](f: (A, B) => C): A => (B => C) = 
	 	a => b => f(a, b)
		//(a: A) => ((b: B) => f(a, b))

	/*
	 NB: There is a method on the `Function` object in the standard library,
	`Function.uncurried` that you can use for uncurrying.

	 Note that we can go back and forth between the two forms. We can curry and uncurry
	 and the two forms are in some sense "the same". In FP jargon, we say that they
	 are _isomorphic_ ("iso" = same; "morphe" = shape, form), a term we inherit from
	 category theory.
	*/
	//def uncurry[A,B,C](f: A => (B => C)): (A, B) => C =	
	def uncurry[A,B,C](f: A => B => C): (A, B) => C =	
		(a, b) => f(a)(b)

	/*
	 You need to return a new function of type `A => C`. Start by accepting an argument of type `A`. 
	 Now follow the types. You have an `A`. What can you do with it? 
	 Do you have a function that accepts an `A`?
	 */
	def compose[A,B,C](f: B => C, g: A => B): A => C =
		a => f(g(a))	
}


