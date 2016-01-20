(println (real-path))

(setq cur_dir (real-path))
\
(load "cp936.lsp")
;(load "simsun16px.lsp")
;(load "simsun32.lsp")
;(load "simsun_U16.lsp")

;(load "monaco16px.lsp")

(load "pretty_hex.lsp")


(while (read-line)
	(setq dl (length (current-line)))
	(if (= dl 1)
			(setq ustr "b")
			(= dl 2)
			(setq ustr ">u")
	)
	(if (> dl 0)
		(begin
			(setq cp_key (nth 0 (unpack ustr (current-line))))
			(setq simsun_key (upper-case (format "%04x"  (CP936 cp_key))))
			(println "simsun key: " simsun_key)
			(setq simsun_bitmap (BASE simsun_key))
	
			(println "bitmap is:" )
			(map pretty_hex simsun_bitmap)
			(println)
		)
	)
)

(exit)

