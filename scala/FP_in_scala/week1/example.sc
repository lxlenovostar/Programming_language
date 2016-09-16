"Hello World."

1+3

def abs(x: Double) = if (x < 0) -x else x

def sqrtIter(guess: Double, x: Double) : Double  =
  if (isGoodEngouh(guess, x)) guess
  else sqrtIter(improve(guess, x), x)

def isGoodEngouh(guess: Double, x:Double) =
  abs(guess * guess - x) /x  < 0.001

def improve(guess: Double, x: Double) =
  (guess + x / guess) / 2

def sqrt(x: Double) = sqrtIter(1.0, x)

sqrt(2)

sqrt(1e-6)

sqrt(1e60)