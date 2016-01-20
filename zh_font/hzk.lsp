(if (nil? (main-args 2))
	(setq fs 16)
	(setq fs (int (main-args 2)))
)
(constant 'file (string "hzk"  fs ))

(setq cols (/ fs 8))
(setq bytes (* cols fs))

(setq line 0)
(setq hex_str "")
(setq json_str "")
(setq data '())

(while (read-line)
			 (setq ch (nth 0 (current-line)))
			 (setq ch1 (nth 1 (current-line)))

			 (setq offset (* (+ (* 94 (- (char ch) 0xa0 1)) (- (char ch1) 0xa0 1) ) bytes))

			 (setq hr (open file "r"))
			 (seek hr offset)
			 (read hr buffer  bytes)

			 (for (i 0 (- fs 1))
						(for (j 0 (- cols 1))
			 				(print (format "%02x" (unpack "b" (nth (+ (* i cols) j) buffer))))
			 			)
			 			(println)
			 )

)

(exit)

