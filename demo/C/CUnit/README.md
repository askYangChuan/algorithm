Makefile_ctest.inc��֧��Cunit��Makefile��


Cunit��������
1���Ȳο�Cunit��������д������ش���(test_hello.c)��
2��make ctest=1����ú�ִ�в����������õ�2��xml��
3������icovת���õ����븲���ʵĿ��ӻ������
lcov -c -d ./ -o app.info
������app.info����ͳ�ƽ��
genhtml app.info -o cc_result

ʹ���������cc_result/index.html�Ϳ�����


ע�����
test_*���ļ�����������д���������Ĵ��룬��ͨ�ļ���Ҫȡtest_��ͷ���������֣�ǰ׺����ͨ������CTEST_PRIFIXָ��(test_%)

��Ҫ���ɲ���������ִ��make ctest=1������Makefile������ӱ���CTEST_EXEC=xxx�������������ʽ������ò�Ҫʹ�ã�ʹ��make���±���(��Ҫmakefile����û��CTEST_EXEC����)��

