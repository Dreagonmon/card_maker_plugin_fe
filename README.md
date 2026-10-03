# [fe](https://github.com/rxi/fe) port to wasm, for "the card maker" (WIP)

## Special Forms

| Form | Arguments | Return Type | Description |
|---|---|---|---|
| `(let sym val)` | `sym`, `val` | `nil` | Creates a new binding of `sym` to `val` in the current environment. |
| `(= sym val)` | `sym`, `val` | `nil` | Sets the existing binding of `sym` to `val`; if no binding exists, sets the global value. |
| `(if cond then else ...)` | `cond`, `then`, `else...` | value of selected branch | If `cond` is true, evaluates `then`; otherwise evaluates `else`. Clauses can be chained for else-if behavior. |
| `(fn params ...)` | `params`, `body...` | function object | Creates a new function. |
| `(mac params ...)` | `params`, `body...` | macro object | Creates a new macro. |
| `(while cond ...)` | `cond`, `body...` | `nil` | Repeatedly evaluates `body` while `cond` is true. |
| `(quote val)` | `val` | unevaluated value | Returns `val` unevaluated. |
| `(and ...)` | `expr...` | last value or `nil` | Evaluates each expression until one is `nil`; returns the last value if all are true. |
| `(or ...)` | `expr...` | first true value or `nil` | Evaluates each expression until one is true; returns that value, otherwise `nil`. |
| `(do ...)` | `expr...` | last value | Evaluates each expression and returns the value of the last one. |

## Functions

| Function | Arguments | Return Type | Description |
|---|---|---|---|
| `(cons car cdr)` | `car`, `cdr` | pair | Creates a new pair with the given `car` and `cdr` values. |
| `(car pair)` | `pair` | car value or `nil` | Returns the `car` of `pair`, or `nil` if `pair` is `nil`. |
| `(cdr pair)` | `pair` | cdr value or `nil` | Returns the `cdr` of `pair`, or `nil` if `pair` is `nil`. |
| `(setcar pair val)` | `pair`, `val` | `nil` | Sets the `car` of `pair` to `val`. |
| `(setcdr pair val)` | `pair`, `val` | `nil` | Sets the `cdr` of `pair` to `val`. |
| `(list ...)` | `values...` | list | Returns all arguments as a list. |
| `(not val)` | `val` | `t` or `nil` | Returns true if `val` is `nil`, otherwise returns `nil`. |
| `(is a b)` | `a`, `b` | `t` or `nil` | Returns true if `a` and `b` are equal in value. Numbers and strings are equal if equivalent; other values are equal only if the same underlying object. |
| `(atom x)` | `x` | `t` or `nil` | Returns true if `x` is not a pair, otherwise `nil`. |
| `(print ...)` | `values...` | `nil` | Prints all arguments to `stdout`, separated by spaces and followed by a newline. |
| `(< a b)` | `a`, `b` | `t` or `nil` | Returns true if the numerical value `a` is less than `b`. |
| `(<= a b)` | `a`, `b` | `t` or `nil` | Returns true if the numerical value `a` is less than or equal to `b`. |
| `(+ ...)` | `numbers...` | number | Adds all arguments together. |
| `(- ...)` | `numbers...` | number | Subtracts all arguments, left-to-right. |
| `(* ...)` | `numbers...` | number | Multiplies all arguments. |
| `(/ ...)` | `numbers...` | number | Divides all arguments, left-to-right. |

## Extended Functions

| Function | Arguments | Returns | Description |
|---|---|---|---|
| `(> a b)` | `a`, `b`: numbers | `t` or `nil` | Returns true if `a > b`. |
| `(>= a b)` | `a`, `b`: numbers | `t` or `nil` | Returns true if `a >= b`. |
| `(type obj)` | `obj`: any | number | Returns the type code of `obj`. |
| `(stacksize)` | none | number | Returns the current GC stack size. |
| `(dumpglobal)` | none | `nil` | Prints all global symbols and their values. |
| `(memfree)` | none | number | Returns the number of free pairs. |
| `(strlen str)` | `str`: string | number | Returns the character count of `str`. |
| `(strappend str chr ...)` | `str`: string; `chr`: number(s) | string | Appends characters as unsigned bytes to `str`. |
| `(strcat str obj ...)` | `str`: string; `obj`: any | string | Appends objects as strings to `str`. |
| `(vfopen path)` | `path`: string | pointer or `nil` | Opens a virtual file. |
| `(vfclose vfp)` | `vfp`: pointer | `nil` | Closes a virtual file. |
| `(vfgetc vfp)` | `vfp`: pointer | number | Reads one character; returns byte or `-1` at EOF. |
| `(setstr name value)` | `name`: string; `value`: any | `nil` | Sets a global string variable. |
| `(setnum name value)` | `name`: string; `value`: number | `nil` | Sets a global number variable. |
| `(getstr name)` | `name`: string | string or `nil` | Gets a global string variable. |
| `(getnum name)` | `name`: string | number or `nil` | Gets a global number variable. |

## Builtin Types

```
    ┌─[pair]─┐          ┌─[list]─┐             
    ▼        ▼          ▼        ▼             
  [car]    [cdr]      [item]┌─[list]─┐         
                            ▼        ▼         
                          [item]  [list]       
                                               
  ┌─[symbol]─┐             ┌─[string]─┐        
  ▼          ▼             ▼          ▼        
[tag]  ┌───[pair]─┐     [bytes] ┌─[string]─┐   
       ▼          ▼             ▼          ▼   
    [name]    [value]        [bytes]   [string]
```
