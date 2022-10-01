####################################################################
# 启动数据中心后台服务程序的脚本。
####################################################################

# 检查服务程序是否超时，配置在/etc/rc.local中由root用户执行。
# daemon
../../tools/bin/procctl 30 ../../tools/bin/checkproc

# 生成用于测试的全国气象站点观测的分钟数据。
../../tools/bin/procctl 60 ../bin/crtmetdata ../ini/stcode.ini ../../tmp/metdata ../../log/crtmetdata.log xml,json,csv

# 压缩数据中心后台服务程序的备份日志。
../../tools/bin/procctl 300 ../../tools/bin/gzipfiles ../../log/. "*.log.20*" 0.02

# 清理原始的全国气象站点观测的分钟数据目录/tmp/metdata中的历史数据文件。
../../tools/bin/procctl 300 ../../tools/bin/deletefiles ../../tmp/metdate "*" 0.02
