������Ҫ��hook malloc���ڴ���亯�����ڼ���ڴ�й¶

���ӱ��⣺
-L../check_mem_leaks -lmemcheck -Wl,--export-dynamic -fno-omit-frame-pointer -rdynamic

ע��
1��Ĭ���ڴ�й¶��鲻��������Ϊ��ʼ���Ĵ���û��Ҫ����ڴ�й¶.
2��mem_leaks_start()���ڿ������(���ӿ��Ĭ�Ϲر�),mem_leaks_stop()����ֹͣ��顣mem_leaks_show()����չʾ��������ݡ�
3����� addr2line ���м��
    addr2line -e prog -f -C -a 0x425355 -p

4, parse_mem.py��һ��������־�Ĺ��ߣ���������ʹ��addr2line�������������к�
    