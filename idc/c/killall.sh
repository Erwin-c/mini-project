####################################################################
# 停止数据中心后台服务程序的脚本。
####################################################################

killall -9 procctl
# Inform service program to exit
killall gzipfiles crtmetdata deletefiles

sleep 3

killall -9 gzipfiles crtmetdata deletefiles
