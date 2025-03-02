FAKE_ALL: all

BOOST_DIR = /home/bywang/boost/include
CPPFLAGS += -I$(BOOST_DIR)

# 設定最終產生的執行檔名稱
BINTARGET = main

# 如有需要，可以指定相關的標頭檔案路徑（此處空白代表無額外設定）
HEADERS =

# 如有需要，PUBLIC_HDRS 可設定要安裝的公開標頭檔案
PUBLIC_HDRS =

# 連結時所需要的函式庫，這裡連結到 libdef.a
LDLIBS = ../lib/libdef.a

# 列出所有需要編譯的 C++ 原始檔案
BINSRCS = \
    main.cpp \
    def_parser.cpp

# 引入共用的編譯與連結規則
include ../template.mk
