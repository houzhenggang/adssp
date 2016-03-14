.PHONY:all




all:pcre-8.13 libpcre.a

libpcre.a:
	ln -s  pcre-8.13/.libs/libpcre.a

pcre-8.13:
	tar xvf pcre-8.13.tar.gz>>/dev/null
	ln -s pcre-8.13 pcre
	cd pcre && ./configure >>/dev/null
	cd pcre &&  make >>/dev/null


clean:
	rm -rf  pcre-8.13
	rm -rf  libpcre.a
	rm -rf  pcre
