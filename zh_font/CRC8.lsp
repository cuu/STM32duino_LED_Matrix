(define (CRC8 data len,i crc sum ch)
	(setq i 0)
	(setq crc 0)
	(while (>= (-- len) 0)	
		(setq ch (char (nth i data)))
		(++ i)
		(for (tempI 0 7)
			(setq sum (& (^  crc ch) 0x01))
			(setq crc (>> crc 1))
			(if (> sum 0)
				(setq crc (^ crc 0x8c))
			)
			(setq ch (>> ch 1))
		)
	)
	crc
)



