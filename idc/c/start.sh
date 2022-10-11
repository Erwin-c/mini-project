# 启动数据中心后台服务程序的脚本.

# 检查服务程序是否超时, 配置在 /etc/rc.local 中由 root 用户执行.
# daemon
~/Coding/mini-project/tools/bin/procctl 30 ~/Coding/mini-project/tools/bin/checkproc

# 生成用于测试的全国气象站点观测的分钟数据.
~/Coding/mini-project/tools/bin/procctl 60 ~/Coding/mini-project/idc/bin/crtmetdata ~/Coding/mini-project/idc/ini/stcode.ini ~/Coding/mini-project/tmp/metdata ~/Coding/mini-project/log/crtmetdata.log xml,json,csv

# 压缩数据中心后台服务程序的备份日志.
~/Coding/mini-project/tools/bin/procctl 300 ~/Coding/mini-project/tools/bin/gzipfiles ~/Coding/mini-project/log/. "*.log.20*" 0.02

# 清理原始的全国气象站点观测的分钟数据目录 ~/Coding/mini-project/tmp/metdata/. 中的历史数据文件.
~/Coding/mini-project/tools/bin/procctl 300 ~/Coding/mini-project/tools/bin/deletefiles ~/Coding/mini-project/tmp/metdate/. "*" 0.02

# 下载全国气象站点观测的分钟数据的 xml 文件.
~/Coding/mini-project/tools/bin/procctl 30 ~/Coding/mini-project/tools/bin/ftpgetfiles ~/Coding/mini-project/log/idc/ftpgetfiles_surfdata.log "<host>127.0.0.1:21</host><mode>1</mode><username>wucz</username><password>wuczpwd</password><localpath>/idcdata/surfdata</localpath><remotepath>/tmp/idc/surfdata</remotepath><matchname>SURF_ZH*.XML</matchname><listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename><ptype>1</ptype><okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename><checkmtime>true</checkmtime><timeout>80</timeout><pname>ftpgetfiles_surfdata</pname>"

# 上传全国气象站点观测的分钟数据的 xml 文件.
~/Coding/mini-project/tools/bin/procctl 30 ~/Coding/mini-project/tools/bin/ftpputfiles ~/Coding/mini-project/log/idc/ftpputfiles_surfdata.log "<host>127.0.0.1:21</host><mode>1</mode><username>wucz</username><password>wuczpwd</password><localpath>/tmp/idc/surfdata</localpath><remotepath>/tmp/ftpputest</remotepath><matchname>SURF_ZH*.JSON</matchname><ptype>1</ptype><okfilename>/idcdata/ftplist/ftpputfiles_surfdata.xml</okfilename><timeout>80</timeout><pname>ftpputfiles_surfdata</pname>"
