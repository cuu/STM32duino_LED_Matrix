;;; line by line send to MCU and check the CRC8 ,
;;; faild max retry 4 times

(load "CRC8.lsp")
(setq socat_str "socat STDIO tcp:localhost:9999")
(setq try_times 0)
(constant 'max_retry 5)

(define (line_by_line str, cmd result try_ret mcu_ret mcu_res)
	
	(setq crc8 (CRC8 str (length str)))
	(println  "line#"  line_number  " crc8: " crc8)
	(setq cmd (string "echo '" str "' | " socat_str))
	(setq result (exec cmd))
	(println "line#" line_number " mcu return: " result)
	(setq try_ret (catch  (eval-string (result 0)) 'mcu_res))
	(if (not (nil? try_ret))
		(begin
			(if 
				(!= mcu_res crc8)
				(begin
					(println mcu_ret)
					(if 
						(< try_times max_retry)
						(begin
						;	(sleep 50)
							(++ try_times)	
							(line_by_line str)
						)
						(println "exceed max retry times")
					)
				)
			)
		)
		(begin
			(println "parse newlisp return failed")
			(println result)
		)
	)
)

(setq line_number 0)
(while (read-line)
	
	(setq try_times 0)
	(line_by_line (current-line))
	(println "line#" line_number  " made " (+ try_times 1) " times")
	(println)
	(++ line_number)

)

(println "exiting....")
(exit)

