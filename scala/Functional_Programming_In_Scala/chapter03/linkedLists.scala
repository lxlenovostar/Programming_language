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
/* Objects are used to hold single instances of a class. Often used for factories. */
case object Nil extends List[Nothing]

/*
 * 样本类
 * 1. 使用样本类可以方便得存储和匹配类的内容。你不用new关键字就可以创建它们。
 * 2. 样本类就是被设计用在模式匹配中的。
 * */
case class Cons[+A](head: A, tail: List[A]) extends List[A]

object List {
    /*
     * It's generally good practice when pattern matching to use `_` for any variables 
     * you don't intend to use on the right hand side of a pattern. This makes it clear 
     * the value isn't relevant.
     * */
	def tail[A](list: List[A]): List[A] = list match {
		case Nil => Nil
		case Cons(_, xs) => xs
	}

     /*
      * 这个使用了递归，有可能导致堆栈溢出。
      * Note that we're copying the entire list up until the last element. Besides being inefficient, 
      * the natural recursive solution will use a stack frame for each element of the list, which can 
      * lead to stack overflows for large lists (can you see why?). With lists, it's common to use a 
      * temporary, mutable buffer internal to the function (with lazy lists or streams, which we discuss 
      * in chapter 5, we don't normally do this). So long as the buffer is allocated internal to the function, 
      * the mutation is not observable and RT is preserved.
      *
      * Another common convention is to accumulate the output list in reverse order, then reverse it at the end, 
      * which doesn't require even local mutation. We'll write a reverse function later in this chapter.
      *
      * */
    def init[A](l: List[A]) : List[A] = 
         l match {
           case Nil => sys.error("init of empty list")
           case Cons(_, Nil) => Nil
           case Cons(h, t) => Cons(h,init(t))
         }

    def init2[A](l: List[A]): List[A] = {
        import collection.mutable.ListBuffer
        val buf = new ListBuffer[A]
        @annotation.tailrec
        def go(cur: List[A]): List[A] = cur match {
            case Nil => sys.error("init of empty list")
            case Cons(_, Nil) => List(buf.toList: _*)
            case Cons(h,t) => buf += h; go(t)
        }
        go(l)
    }

	def drop[A](l: List[A], n: Int): List[A] = 
        if (n <= 0) l
        else l match {
            case Nil => Nil 
            case Cons(_, t) => drop(t, n-1)
        }

    /*
     * Somewhat overkill, but to illustrate the feature we're using a _pattern guard_, 
     * to only match a `Cons` whose head satisfies our predicate, `f`. The syntax is to 
     * add `if <cond>` after the pattern, before the `=>`, where `<cond>` can use any of 
     * the variables introduced by the pattern.
     * */
    def dropWhile[A](l: List[A], f: A => Boolean): List[A] = l match { 
        case Cons(h, t) if f(h)  => dropWhile(t, f)
        case _ => l
    }


    def append[A](a1: List[A], a2: List[A]): List[A] = a1 match {
        case Nil => a2
        case Cons(h,t) => Cons(h, append(t, a2))
    }

	def sum(ints: List[Int]): Int = ints match {
		case Nil => 0
		case Cons(x,xs) => x + sum(xs)
	}

	def product(ds: List[Double]): Double = ds match {
		case Nil => 1.0
		case Cons(0.0, _) => 0.0
		case Cons(x,xs) => x * product(xs)
	}

    /*
     * The function apply in the object List is a variadic function, meaning it accepts zero
     * or more arguments of type A. 
     *
     * The argument as will be bound to a Seq[A] which has the functions head (returns the 
     * first element) and tail (returns all elements but the first). The special _* type 
     * annotation allows us to pass a Seq to a variadic method.
     * */
	def apply[A](as: A*): List[A] =
		if (as.isEmpty) Nil
		else Cons(as.head, apply(as.tail: _*))
}

