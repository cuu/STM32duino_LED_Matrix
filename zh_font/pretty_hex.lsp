(define (pretty_hex hex_str,bs bl bsl)
	(setq bs (bits (int hex_str 0 16)))
	(setq bl (length hex_str))
	(setq bsl (* (/ bl 2) 8))				
	(println (dup " " (- bsl (length bs))) (replace "0" bs " "))
)


