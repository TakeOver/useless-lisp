; lambda var macro - do not evaluate argsuments, type - FSUBR
; lambda! var! macro! - evaluates, type - SUBR

(var defmacro 
        (macro (name args body) 
                (var! name 
                        (macro! args body))))
(defmacro defun (name args body) 
        (var! name 
                (lambda! args body)))
(defmacro defunf (name args body) 
        (var! name 
                (flambda! args body)))
(defmacro let (vars body) 
        ((lambda! (list (car vars)) body) 
        (car (cdr vars))))

; examples
(let (a 10) 
        (print a)) ; prints 10 as expected.

(defmacro a (b c) 
        (var! b c))

(a q 10)

(print q) ; prints 10 as expected; macroses - like FEXPR, but uses passive lispstate, so 'var' would write new variable to parent state

(defun a (b c) 
        (var! b c))

(a q1 10) ; (print q1) - error, q1 is undefined

;also
(defun repl (a) 
        (repl (print 'repl: (eval(read)))))
; call as (repl nil)