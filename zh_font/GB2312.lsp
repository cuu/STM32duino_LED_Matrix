;;列出GB2312 所有字符,根据区位码的编排顺序
;; 01->94
;; . . .
;; . . .
;; 87->94

(for (r 16 87)
	(for (i 1 94)
		(setq gb_str (pack "b b" (+ r 0xa0) (+ i 0xa0)))
		(setq gb_num (nth 0 (unpack ">u" gb_str)))
		;(println "gbnumber: " gb_num)
;		(print (format "%02d%02d,%02x%02x " r i (+ r 0xa0) (+ i 0xa0)))
		(if (or (< gb_num 55290) (> gb_num 55294));; 跳过 55 90 -- 55 94
		;(if (< gb_num 55290)
			(print gb_str " ")
		)
		
	)
	(println)
)

(exit)

