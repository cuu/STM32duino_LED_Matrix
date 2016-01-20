conver_simsun.lsp <- 转换 simsun?.bdf 变成simsun?.lsp的key-value的数据库文件
用法: newlisp conver_simsun.lsp simsun16.bdf 


cp936.lsp
CP936.TXT <- GBK 与 UTF8的映射关系 

print_to_binary.lsp <- 一个测试性的把每行的hex 打印成 整的 01 

read_cp936.lsp <- convert CP936.TXT -> cp936.lsp


simsun16.bdf
simsun16.lsp

simsun32.bdf

---- how to convert ttf
转换 ttf 到 bdf 用otf2bdf xxx.ttf -o xxx.bdf ,默认是 12pt(16px),参见参数 -p 

---- how to use the lsp file

root@debian:/home/guu/work/zh_font# newlisp simsun16.lsp 
newLISP v.10.6.2 64-bit on Linux IPv4/6 libffi, options: newlisp -h

> (BASE "0041")
("18" "18" "3C" "34" "24" "24" "66" "7E" "42" "42" "43" "C1")

=======

newlisp cp936.lsp

(BASE 0x41) -> 65 == 0x41 but in simsun?.lsp ,it will be  "0041"

newlisp simsun16.lsp pretty_hex.lsp

> (map (fn (x) (pretty_hex x)) (BASE "0041"))

-------------------------------------

cat test_words.txt 开始
把含有中文的数据要转换成json 指令,包含bitmap信息而已,一条一条的通过line_by_line.lsp 吐进LED控制板

# cat test_words.txt | iconv -f UTF8 -t GBK | newlisp debug.lsp

--------------------
新建config.json 的格式
其中 会有scrs 和与scrs配套的 fs等配置的数组


生成了一个包含 asc hzk 16和32的字库
cat asc16 hzk16 asc32 hzk32 > zk_ashz_16_32

asc.lsp <- 读asc字库
hzk.lsp <- 读hzk字库
asc_hzk.lsp  <- 把hzk 和asc 合一起读生成 json数据条码

最后 arduino采用了EQLED的指令 !#001....$$的格式
SPI字库的速度快于串口几十倍

arduino sketch is /root/Arduino/sketch_jan07a_arduino_json_newline_test


