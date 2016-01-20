(setq file "CP936.TXT")

(setq hr (open file "r"))

(new Tree 'CP936)

(setq cnt 0)
(while (read-line hr )
			 (setq tmplst (parse (current-line) "\t"))
;			 (println tmplst)
			 (setq tmplst (map (fn (x) (int x 0 16)) tmplst))
;			 (println tmplst)
;			 (++ cnt)
;			 (if (> cnt 20) (exit))
			 (CP936 (tmplst 0) (tmplst 1))
)
(close hr)

(save "cp936.lsp" 'CP936)
(println "Done")

(exit)
			  
