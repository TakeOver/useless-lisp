(var defmacro (macro (name args body) (var! name (macro! args body))))
(defmacro defun (name args body) (var! name (lambda! args body)))
