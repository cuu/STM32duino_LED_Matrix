(module "getopts.lsp")


(if (nil? (main-args 2))
	(setq fs 16)
	(setq fs (int (main-args 2)))
)

;(setq fs 32)
(constant 'file (string "hzk" fs))
(constant 'asc_file (string "asc" fs))

(setq hr (open file "r"))
(setq asc_hr (open asc_file "r"))

(setq bytes (* (/ fs 8) fs))
(setq cols (/ fs 8))

(setq line 0)
(setq hex_str "")
(setq json_str "")
(setq data '())

(define (get_asc)
		 (setq ch (nth 0 (current-line)))
			
		 (setq asc_bytes (* (/ cols 2) fs))

		 (setq offset (+ (* (char ch) asc_bytes) 0))

		 (seek asc_hr offset)
		 (read asc_hr buffer  bytes)
		 (for (j  0 (- (/ cols 2) 1))
		 	(for (i 0 (- fs 1))
		 		(extend hex_str (format "%02x" (unpack "b" (nth (+ (* i (/ cols 2)) j) buffer))))
		 		;(println)
		 	)
		 )

		 (extend  json_str (string [text]{"zhn":[/text] line [text],"rows":[/text] fs [text],"cols":[/text] (/ fs 16) [text],"hex":"[/text] hex_str [text]"}[/text] "\n" ))
		 (setq hex_str "")
		 (push (current-line) data -1)
	
)


(define (get_hzk)
		 (setq ch (nth 0 (current-line)))
		 (setq ch1 (nth 1 (current-line)))

		 (setq offset (* (+ (* 94 (- (char ch) 0xa0 1)) (- (char ch1) 0xa0 1) ) bytes))

		 (seek hr offset)
		 (read hr buffer  bytes)

		 (for (j  0 (- cols 1))
		 	(for (i 0 (- fs 1))
		 		(extend hex_str (format "%02x" (unpack "b" (nth (+ (* i cols) j) buffer))))
		 		;(println)
		 	)
		 )

		 (extend  json_str (string [text]{"zhn":[/text] line [text],"rows":[/text] fs [text],"cols":[/text] (/ fs 8) [text],"hex":"[/text] hex_str [text]"}[/text] "\n" ))
		 (setq hex_str "")
		 (push (current-line) data -1)
	
)

(while (read-line)
	(if (> (length (current-line)) 1)
		(begin
			(get_hzk)
		)
		(get_asc)
	)
	(++ line)
)

(setq asc_length 0)
(dolist (x data)
	(setq asc_length (+ asc_length (length x)))
)

;(println [text]{"asclen":[/text] asc_length  [text],"zhlen":[/text] line  [text],"zhdat":"[/text] (join data) [text]","screen":0,"effect":1}[/text])

(println [text]{"asclen":[/text] asc_length  [text],"zhlen":[/text] line  [text],"screen":1,"effect":4}[/text])
(print json_str)

(close hr)
(close asc_hr)

(exit)

