Makefile_ctest.inc��֧��Cunit��Makefile��


Cunit��������
1���Ȳο�Cunit��������д������ش���(test_hello.c)��
2��make ctest=1����ú�ִ�в����������õ�2��xml��
3������icovת���õ����븲���ʵĿ��ӻ������
lcov -c -d ./ -o app.info
������app.info����ͳ�ƽ��
genhtml app.info -o cc_result

ʹ���������cc_result/index.html�Ϳ�����

�����ǲ鿴�����ʣ������ǲ鿴���Խ��
Ҫ�鿴�������ļ�����Ҫʹ������xsl��dtd�ļ��� 

CUnit-List.dtd��CUnit-List.xsl���ڽ����б��ļ��� 

CUnit-Run.dtd��CUnit-Run.xsl���ڽ�������ļ���

���ĸ��ļ���CUnit���������ṩ����װ֮����$(PREFIX) /share/CUnitĿ¼�£�Ĭ�ϰ�װ�Ļ���/home/lirui/local/share/CUnitĿ¼�¡�
�ڲ鿴���֮ǰ����Ҫ������ ���ļ���TestMax-Listing.xml, TestMax-Results.xml, CUnit-List.dtd, CUnit-List.xsl, CUnit-Run.dtd, CUnit-Run.xsl������һ��Ŀ¼�£�Ȼ��������������������xml�ļ��Ϳ�����


ע�����
test_*���ļ�����������д���������Ĵ��룬��ͨ�ļ���Ҫȡtest_��ͷ���������֣�ǰ׺����ͨ������CTEST_PRIFIXָ��(test_%)

��Ҫ���ɲ���������ִ��make ctest=1������Makefile������ӱ���CTEST_EXEC=xxx�������������ʽ������ò�Ҫʹ�ã�ʹ��make���±���(��Ҫmakefile����û��CTEST_EXEC����)��

