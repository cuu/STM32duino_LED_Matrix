(if (nil? (main-args 2))
	(setq fs 16)
	(setq fs (int (main-args 2)))
)
(constant 'file (string "asc" fs))

(setq bytes (* (/ fs 8) fs))
(setq cols (/ fs 8))



(while (read-line)
			 (setq ch (nth 0 (current-line)))

		 	 (setq asc_bytes (* (/ cols 2) fs))

			 (setq offset (* (char ch) asc_bytes) )
			 (println "offset: " offset)

			 (setq hr (open file "r"))
			 (seek hr offset)
			 (read hr buffer  asc_bytes)

		 	(for (i 0 (- fs 1))
		 		(for (j 0 (- (/ cols 2) 1) )
		 			(print (format "%02x" (unpack "b" (nth (+ (* i (/ cols 2)) j) buffer))))
		 		)
		 		(println)
		 	)


)


(exit)

