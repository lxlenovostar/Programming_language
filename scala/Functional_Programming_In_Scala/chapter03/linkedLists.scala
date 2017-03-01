/*
 A trait is an abstract interface that may optionally contain implementations 
 of some methods. Here we’re declaring a trait, called List, with no methods on it. 

 Adding sealed in front means that all implementations of the trait must be declared 
 in this file.

 the + indicates that the type parameter A is covariant. This means that, for instance, 
 List[Dog] is considered a subtype of List[Animal], assuming Dog is a subtype of Animal.
 */
sealed trait List[+A]

/*
 There are two such implementations, or data constructors, of List (each introduced
 with the keyword case) 

 But notice now that Nil extends List[Nothing]. Nothing is a subtype of all types,
 which means that in conjunction with the variance annotation, Nil can be considered
 a List[Int], a List[Double], and so on, exactly as we want.
 */
case object Nil extends List[Nothing]

case class Cons[+A](head: A, tail: List[A]) extends List[A]

object List {
	def sum(ints: List[Int]): Int = ints match {
		case Nil => 0
		case Cons(x,xs) => x + sum(xs)
	}

	def product(ds: List[Double]): Double = ds match {
		case Nil => 1.0
		case Cons(0.0, _) => 0.0
		case Cons(x,xs) => x * product(xs)
	}

	def apply[A](as: A*): List[A] =
		if (as.isEmpty) Nil
		else Cons(as.head, apply(as.tail: _*))
}

