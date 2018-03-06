include $(top_srcdir)/Common.mk
nobase_include_HEADERS=CacheProxy.h server/server.h

lib_LTLIBRARIES=libCacheProxy.la

libCacheProxy_la_SOURCES=CacheProxy.cc server/server.h
libCacheProxy_la_CPPFLAGS=-I$(abs_srcdir) $(THIRD_PARTY_SERACH_FLAGS)
AM_CXXFLAGS=$(COMMON_CXXFLAGS)
