(load "cp936.lsp")
(load "simsun16px.lsp");; 这个字体决定最大高度
;(load "wqy-zenhei15.lsp")
;(load "wqy-zenhei16px.lsp")
;(load "simsun_U16.lsp")
(load "monaco15px.lsp")

(load "pretty_hex.lsp")


(setq json_str "")

(define (deal_hex hex_lst);; cols化hex bitmap 
	(let (tmplst nil)
		(dolist (x hex_lst)
			(push (explode x 2) tmplst -1)
		)

		
		(setq cols (length (tmplst 0)))
		(extend json_str [text]{"zhn":[/text])
		(extend json_str (string line))
		(extend json_str [text],"rows":[/text])	
		(extend json_str (string (length tmplst)))
		(extend json_str [text],"cols":[/text])
		(extend json_str (string cols))
		(extend json_str [text],"hex":"[/text])
		
		;(map println tmplst)
		
		(for (i 0 (- cols 1)) ;; 把hex按 columns 的竖形队列打印出来,成为 一个又一个的[宽度为8bitx某高度]的hex内容,最大4个0xff 的bitmap 阵容
			(dolist (x tmplst)
					(begin
						(extend json_str (x i))
					)
			)
		)
		(extend json_str [text]"}[/text])
		(extend json_str (char 0x0a))
	)
)

(setq data '())

(setq line 0)

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
			(setq simsun_bitmap (BASE simsun_key))
	
			;(println "hex is:" )
			;(map pretty_hex simsun_bitmap)
			(deal_hex simsun_bitmap)
			;(println)
		)
	)
	(push (current-line) data -1)
	(++ line)
)

(println [text]{"zhdat":"[/text] (join data) [text]","screen":0,"effect":2}[/text] )
(print json_str)
(exit)

