../bin/crtmetdata ../ini/stcode.ini ../../tmp/metdata ../../log/crtmetdata.log xml,json,csv 20220910123000

g++ -std=c++2a -g -Wall -Wextra

../bin/procctl 60 ../../idc/bin/crtmetdata ../../idc/ini/stcode.ini ../../tmp/metdata/. ../../log/crtmetdata.log xml,json,csv
