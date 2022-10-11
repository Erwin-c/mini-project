# 停止数据中心后台服务程序的脚本.

killall -9 procctl
killall gzipfiles crtmetdata deletefiles ftpgetfiles ftpputfiles

sleep 3

killall -9 gzipfiles crtmetdata deletefiles ftpgetfiles ftpputfiles
