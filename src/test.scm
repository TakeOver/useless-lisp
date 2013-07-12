(var defmacro (macro (name args body) (var! name (macro! args body))))
(defmacro defun (name args body) (var! name (lambda! args body)))
(defmacro defunf (name args body) (var! name (flambda! args body)))
(defmacro let (vars body) ((lambda! (list (car vars)) body) (car (cdr vars))))