OUTPUT_DIR=build
SOURCES=CMakeLists.txt *.c *.h

${OUTPUT_DIR}/peer: CMakeLists.txt *.c *.h
	mkdir -p ${OUTPUT_DIR}/
	cd ${OUTPUT_DIR}/; cmake ..
	cmake --build ${OUTPUT_DIR}/

${OUTPUT_DIR}/pa1.tar.gz: ${OUTPUT_DIR}/peer
	mkdir -p ${OUTPUT_DIR}/pa1/
	rm -Rf ${OUTPUT_DIR}/pa1/*
	cp ${SOURCES} ${OUTPUT_DIR}/pa1/
	rm -f $@
	cd ${OUTPUT_DIR}/ && tar cvfz pa1.tar.gz pa1/
	tar tvfz $@

run: ${OUTPUT_DIR}/peer
	${OUTPUT_DIR}/peer
