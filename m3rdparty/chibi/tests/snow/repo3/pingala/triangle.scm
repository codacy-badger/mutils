(import (scheme base)
        (scheme write)
        (scheme process-context)
        (pingala binomial))

(let* ((args (command-line))
       (rows (or (and (pair? args) (pair? (cdr args))
                      (string->number (cadr args)))
                 5)))
  (do ((i 0 (+ i 1)))
      ((> i rows))
    (do ((j 0 (+ j 1)))
        ((> j i) (newline))
      (if (not (zero? j))
          (write-char #\space))
      (write (binomial i j)))))
