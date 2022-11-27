# 启动数据中心后台服务程序的脚本.

# 检查服务程序是否超时, 配置在 /etc/rc.local 中由 root 用户执行.
# daemon
/home/erwin/Coding/mini-project/tools/bin/procctl 30 /home/erwin/Coding/mini-project/tools/bin/checkproc

# 生成用于测试的全国气象站点观测的分钟数据.
/home/erwin/Coding/mini-project/tools/bin/procctl 60 /home/erwin/Coding/mini-project/idc/bin/crtsurfdata /home/erwin/Coding/mini-project/idc/ini/stcode.ini /home/erwin/Coding/mini-project/tmp/surfdata /home/erwin/Coding/mini-project/log/crtsurfdata.log xml,json,csv

# 清理原始的全国气象站点观测的分钟数据目录 /home/erwin/Coding/mini-project/tmp/surfdata/. 中的历史数据文件.
/home/erwin/Coding/mini-project/tools/bin/procctl 300 /home/erwin/Coding/mini-project/tools/bin/deletefiles /home/erwin/Coding/mini-project/tmp/surfdate "*" 0.02

# 文件传输的服务端程序.
/home/erwin/Coding/mini-project/tools/bin/procctl 10 /home/erwin/Coding/mini-project/tools/bin/fileserver 5005 /home/erwin/Coding/mini-project/log/idc/fileserver.log

# 下载全国气象站点观测的分钟数据的 xml 文件.
/home/erwin/Coding/mini-project/tools/bin/procctl 30 /home/erwin/Coding/mini-project/tools/bin/ftpgetfiles /home/erwin/Coding/mini-project/log/idc/ftpgetfiles_surfdata.log "<host>127.0.0.1:21</host><mode>1</mode><username>wucz</username><password>wuczpwd</password><localpath>/idcdata/surfdata</localpath><remotepath>/tmp/idc/surfdata</remotepath><matchname>SURF_ZH*.XML</matchname><listfilename>/idcdata/ftplist/ftpgetfiles_surfdata.list</listfilename><ptype>1</ptype><okfilename>/idcdata/ftplist/ftpgetfiles_surfdata.xml</okfilename><checkmtime>true</checkmtime><timeout>80</timeout><pname>ftpgetfiles_surfdata</pname>"

# 上传全国气象站点观测的分钟数据的 xml 文件.
/home/erwin/Coding/mini-project/tools/bin/procctl 30 /home/erwin/Coding/mini-project/tools/bin/ftpputfiles /home/erwin/Coding/mini-project/log/idc/ftpputfiles_surfdata.log "<host>127.0.0.1:21</host><mode>1</mode><username>wucz</username><password>wuczpwd</password><localpath>/tmp/idc/surfdata</localpath><remotepath>/tmp/ftpputest</remotepath><matchname>SURF_ZH*.JSON</matchname><ptype>1</ptype><okfilename>/idcdata/ftplist/ftpputfiles_surfdata.xml</okfilename><timeout>80</timeout><pname>ftpputfiles_surfdata</pname>"

# 清理采集的全国气象站点观测的分钟数据目录 /home/erwin/Coding/mini-project/tmp/tcpgetest 中的历史数据文件.
/home/erwin/Coding/mini-project/tools/bin/procctl 300 /home/erwin/Coding/mini-project/tools/bin/deletefiles /home/erwin/Coding/mini-project/tmp/tcpgetest "*" 0.02

# 把全国站点参数数据保存到数据库表中, 如果站点不存在则插入, 站点已存在则更新.
/home/erwin/Coding/mini-project/tools/bin/procctl 120 /home/erwin/Coding/mini-project/idc/bin/obtcodetodb //home/erwin/Coding/mini-project/idc/ini/stcode.ini "127.0.0.1,root,rooterwin,mysql,3306" utf8 /home/erwin/Coding/mini-project/log/idc/obtcodetodb.log

# 把全国站点分钟观测数据保存到数据库的 T_ZHOBTMIND 表中, 数据只插入, 不更新.
/home/erwin/Coding/mini-project/tools/bin/procctl 10 /home/erwin/Coding/mini-project/idc/bin/obtmindtodb /home/erwin/Coding/mini-project/idcdata/surfdata "127.0.0.1,root,rooterwin,mysql,3306" utf8 /home/erwin/Coding/mini-project/log/idc/obtmindtodb.log

# 清理 T_ZHOBTMIND 表中 120 分之前的数据, 防止磁盘空间被撑满.
/home/erwin/Coding/mini-project/tools/bin/procctl 120 /home/erwin/Coding/mini-project/tools/bin/execsql /home/erwin/Coding/mini-project/idc/sql/cleardata.sql "127.0.0.1,root,rooterwin,mysql,3306" utf8 /home/erwin/Coding/mini-project/log/idc/execsql.log
