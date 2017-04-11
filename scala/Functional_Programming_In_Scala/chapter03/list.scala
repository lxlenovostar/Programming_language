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

    /*
     * 在函数子句中使用了 pattern match.
     * */
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

    def dropWhileInfer[A](as: List[A])(f: A => Boolean): List[A] =
      as match {
        case Cons(h,t) if f(h) => dropWhileInfer(t)(f)
        case _ => as
      }

	def sum(ints: List[Int]): Int = ints match {
		case Nil => 0
		case Cons(x,xs) => x + sum(xs)
	}

    def productNew(ds: List[Double]): Double = ds match {
        case Nil => 1.0
        case Cons(x, xs) => x * productNew(xs)
    }

	def product(ds: List[Double]): Double = ds match {
		case Nil => 1.0
		case Cons(0.0, _) => 0.0
		case Cons(x,xs) => x * product(xs)
	}

    /*
     * foldRight(List(1,2,3), Nil:List[Int])(Cons(_,_))
     *
     * We get back the original list! Why is that? As we mentioned earlier, one way of thinking about 
     * what `foldRight` "does" is it replaces the `Nil` constructor of the list with the `z` argument, 
     * and it replaces the `Cons` constructor with the given function, `f`. If we just supply `Nil` for 
     * `z` and `Cons` for `f`, then we get back the input list.
     *
     * foldRight(Cons(1, Cons(2, Cons(3, Nil))), Nil:List[Int])(Cons(_,_))
     * Cons(1, foldRight(Cons(2, Cons(3, Nil)), Nil:List[Int])(Cons(_,_)))
     * Cons(1, Cons(2, foldRight(Cons(3, Nil), Nil:List[Int])(Cons(_,_))))
     * Cons(1, Cons(2, Cons(3, foldRight(Nil, Nil:List[Int])(Cons(_,_)))))
     * Cons(1, Cons(2, Cons(3, Nil))) 
     * */

    /*
     * Again, placing f in its own argument group after as and z lets type inference
     * determine the input types to f.
     * */
    def foldRight[A,B](as: List[A], z: B)(f: (A, B) => B): B =
      as match {
        case Nil => z
        case Cons(x, xs) => f(x, foldRight(xs, z)(f))
      }

    /*
     * Since `append` takes time proportional to its first argument, and this first argument 
     * never grows because of the right-associativity of `foldRight`, this function is linear in 
     * the total length of all lists. You may want to try tracing the execution of the implementation 
     * on paper to convince yourself that this works.
     *
     * Note that we're simply referencing the `append` function, without writing something 
     * like `(x,y) => append(x,y)` or `append(_,_)`. In Scala there is a rather arbitrary distinction 
     * between functions defined as _methods_, which are introduced with the `def` keyword, and function 
     * values, which are the first-class objects we can pass to other functions, put in collections, and 
     * so on. This is a case where Scala lets us pretend the distinction doesn't exist. In other cases, you'll 
     * be forced to write `append _` (to convert a `def` to a function value) or even 
     * `(x: List[A], y: List[A]) => append(x,y)` if the function is polymorphic and the type arguments aren't known. 
     * */
    def concat[A](l: List[List[A]]): List[A] = 
        foldRight(l, Nil: List[A])(append)

    /*
     * 使用 tail-recursive 实现
     * */
    /* 
     * It's common practice to annotate functions you expect to be tail-recursive with the `tailrec` annotation. 
     * If the function is not tail-recursive, it will yield a compile error, rather than silently compiling 
     * the code and resulting in greater stack space usage at runtime. 
     * */
    @annotation.tailrec
    def foldLeft[A,B](as: List[A], z: B)(f: (B, A) => B): B =  
      as match {
        case Nil => z
        case Cons(x, xs) =>  foldLeft(xs, f(z, x))(f)
    }

    def append[A](a1: List[A], a2: List[A]): List[A] = a1 match {
        case Nil => a2
        case Cons(h,t) => Cons(h, append(t, a2))
    }

    def addOne(a1: List[Int]): List[Int] = 
      foldRight(a1, Nil:List[Int])((x, y) => Cons(x+1, y))

    def doubleToString(a1: List[Double]): List[String] = 
      foldRight(a1, Nil:List[String])((x, y) => Cons(x.toString, y))
    /*
     * `append` simply replaces the `Nil` constructor of the first list with the second list, 
     * which is exactly the operation performed by `foldRight`.
     * */
    def append1[A](a1: List[A], a2: List[A]): List[A] = 
      foldRight(a1, a2)(Cons(_, _))
    /*
     * 这里用foldLeft是不可以，注意f函数的声明。
     * */
    //foldLeft(a1, a2)((x: A, y: List[A]) => Cons(x, y))

    /*
     * Compute the length of a list using foldRight.
     * */
    def length[A](as: List[A]) : Int = 
      foldRight(as, 0)((x, y) => 1 + y)

    def length2[A](l: List[A]): Int = 
      foldLeft(l, 0)((acc,h) => acc + 1)

    def sum2(ns: List[Int]) =
      foldRight(ns, 0)((x,y) => x + y)

    def sum3(ns: List[Int]) =
      foldLeft(ns, 0)((x,y) => x + y)

    def reverse[A](l: List[A]) : List[A] = 
       foldLeft(l, List[A]())((acc, h) => Cons(h, acc))

    def product3(l: List[Double]) = 
      foldLeft(l, 1.0)(_ * _)
  
    /*
     * _ * _ is more concise notation for (x,y) => x * y
     *
     *  
     * ex3.7 No, this is not possible! The reason is because _before_ we ever call our function, 
     * `f`, we evaluate its argument, which in the case of `foldRight` means traversing the 
     * list all the way to the end. We need _non-strict_ evaluation to support early termination
     * ---we discuss this in chapter 5.
     * */
    def product2(ns: List[Double]) =
      foldRight(ns, 1.0)(_ * _)

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

