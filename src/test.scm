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
(defun _map (f w) 
        (progn 
                (var tmp (list (f (car w)))) 
                (set! w (cdr w))
                (loop w 
                        (progn 
                                (set! tmp (append tmp (f (car w)))) 
                                (set! w (cdr w))))
                tmp))
(defun map (f w)
        (cond (w 
                (_map f w)) 
                (#f nil)))
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
(var endofloop #t)
(loop endofloop (print (quote lisp) (eval(read))))
; call as (repl nil)
(defun not (val) (if val #f #t))


(defmacro let (vars body) 
        (progn 
                (var vars_(eval(eval vars))) 
                (funcall (lambda! (map vars_ (lambda (x) (car x))) body) 
                        (map vars_ (lambda (x) (car (cdr x)))))))
; creating lambda for new scope, vars - ( (a (expr) (b (expr ).. so on))), so with calling map - this macro changes
; (let ((a 10) (b 11)) (print (+ a b))) => ((lambda (a b)) (print ( + a b))) 10 11) ; and this is awesome!

(defun reduce (fun what) (
        if what (progn
                (var tmp (list (car what)))
                (loop (cdr what) (progn
                        (funcall fun (list tmp (car (cdr what))))
                        (set! what (cdr what))))
                tmp)
                nil)) 


(defmacro let (vars body)(progn(var vars_(eval(eval vars)))(funcall (lambda! (map (lambda (x) (car x)) vars_) body)(map (lambda (x) (car (cdr x)))vars_))))