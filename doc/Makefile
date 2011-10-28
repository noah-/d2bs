.PHONY : all
all : docs updateFiles

.PHONY : docs
docs : *.h
	doxygen D2BS.conf

.PHONY : updateFiles
updateFiles : docs
	sudo cp output/html/* /var/www/d2bsDocs/htdocs/
	sudo chown -R apache:apache /var/www/d2bsDocs/htdocs/
