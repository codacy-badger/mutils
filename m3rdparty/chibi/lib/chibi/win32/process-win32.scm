(define (exit . code?)
  (%exit (if (pair? code?) 
           (let ((c (car code?)))
            (cond ((integer? c) c)
                  ((eq? #t c) 0)
                  (else 1)))
           0)))
