# KiSS

## Introduction

**KiSS** means **Ki**nx **S**ubset with **S**tatically-typed.

This project is one of sub projects of [Kinx](https://github.com/Kray-G/kinx).
The main purpose of that is below.

* Realization of a Kinx with statically typed.
* Improvement of performance as a compile type language.

This will be integrated to the [Kinx](https://github.com/Kray-G/kinx) itself in future.

## Syntax

The syntax is similar with the original [Kinx](https://github.com/Kray-G/kinx) because this project is planned to be integrated to the kinx.

### Statements

Currently supported statements are followings.

* Declaration Statement
* Expression Statement
* `if` Statement
* `for` Statement
* `while` Statement
* `do_while` Statement
* `return` Statement
* Function Definition
* Block

### Expressions

Currently supported expressions are followings.

* Assignment
* Comparison
* Add/Sub/Mul/Div/Mod operations
* Calling a function
* Variables

### Type

The following types are currently supported.

* `int` ... a 64 bit integer value.
* `dbl` ... a double precision floating point value.

The type will be specified by the rule below.

* **Not specified**
  * A variable is a 64bit integer.
* **`:typename`** such as `:int`
  * A variable is a specified type.
* **`:funcion(typename)`** such as `function(int)`
  * A variable is a function which returns a value of a specified type.

In the function definition, a returned value's type is specified right after `function` keyword like this.

```
function:int func() {
    ...
}
```

## Examples

If you use only a 64bit integer, it seems to be absolutely same as Kinx.

```
function fib(n) {
    if (n < 3)
        return n;
    return fib(n-2) + fib(n-1);
}
```

This means the following code with a type.

```
function:int fib(n:int) {
    if (n < 3)
        return n;
    return fib(n-2) + fib(n-1);
}
```
