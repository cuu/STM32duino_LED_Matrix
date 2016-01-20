(if (< (length (main-args)) 3)
	(begin
		(println "arg less")
		(exit)
	)
)

(setq simsun_bdf  (main-args 2))

;(replace "bdf" simsun_bdf "lsp")
;(println "saving fonts to " simsun_bdf)

(setq hr (open simsun_bdf "r"))

(setq bmp_lst '())

(new Tree 'BASE)

(setq stat 0) ;; 0 is normal,3 is store bitmap 


(while (read-line hr)
			 
		(if (= stat 3)
			(push (current-line) bmp_lst -1)
		)

		(if (starts-with (current-line) "STARTCHAR")
			(begin
				(setq tmplst (parse (current-line) " "))
				(setq cur_key (tmplst 1))	
			)
			(starts-with (current-line) "BITMAP")
			(begin
				(setq stat 3)
			)
			(starts-with (current-line) "ENDCHAR")
			(begin
				(pop bmp_lst -1);; 
				(BASE cur_key bmp_lst)
				(setq bmp_lst '());; clear bmp_lst
				(setq stat 0)
			)
		)	
)

(close hr)

(save (replace "bdf" simsun_bdf "lsp") 'BASE)

(println "Done")
(exit)

