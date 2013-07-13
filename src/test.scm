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
(defmacro if (condition then else) 
        (cond! 
                (list condition then) 
                (list #t else)))

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
        (repl 
                (print (list (quote lisp)) 
                (eval(read)))))
; call as (repl nil)
(defun map (what aggregate)
        (if (null what) 
                nil
                (progn
                        (var tmp (aggregate (car what)))
                        (set! what (cdr what))
                        (loop (null what)
                                (progn
                                        (append tmp (aggregate(car what)))
                                        (set! what (cdr what))))
                        tmp)))

(defmacro let (vars body) 
        (progn 
                (var vars_(eval vars)) 
                ((lambda! (map vars_ (lambda (x)(car x))) body) 
                        (map vars_ (lambda(x) (car (cdr x)))))))
; creating lambda for new scope, vars - ( (a (expr) (b (expr ).. so on))), so with calling map - this macro changes
; (let ((a 10) (b 11)) (print (+ a b))) => ((lambda (a b)) (print ( + a b))) 10 11) ; and this is awesome!