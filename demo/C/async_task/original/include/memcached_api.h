#ifndef _MEMCACHED_API_H_
#define _MEMCACHED_API_H_
#include <libmemcached/memcached.h>

#include "ds_server.h"
#include "udp_ctrl.h"
#include "../coapserver/xslock_common.h"



/*memcached
key						value
===================================
uuidType+sn				uuid
passType+sn				passwd
serveridType+sn			server_id
devinfoType+sn			mc_devinfo_t
nickType+sn				sn

venderidType+id			vender_string
venderstrType+str		vender_id
updateStatType			sn_array
*/
typedef enum{
	MC_KEY_TYPE_UUID = 0, /*UUID*/
	MC_KEY_TYPE_PASS = 1, /*PASSWD*/
	MC_KEY_TYPE_SERVERID = 2, /*ServerID �ϲ�����״̬��serveridΪ0��ʾ���ߣ���serverid��ʾ����*/
	MC_KEY_TYPE_DEVINFO = 3, /*mc_devinfo_t*/	
	MC_KEY_TYPE_NICK = 4, /*SN*/	
	MC_KEY_TYPE_VENDERID = 5, /*VENDER_ID*/	
	MC_KEY_TYPE_VENDERSTR = 6, /*VENDER_STRING*/
	MC_KEY_TYPE_UPDATESN = 7, /*��Ҫд�����ݿ��SN_ARRAY*/
	MC_KEY_TYPE_CAR_INDEX = 8, /*�������cache index*/
	MC_KEY_TYPE_CAR_KEY = 9, /*�������cache key*/
	MC_KEY_TYPE_SERVER_LANIP = 10,/*ͨ��serverid��ȡserverip*/
	MC_KEY_TYPE_TCPONLINE = 11,/*tcp�豸��¼������*/
	MC_KEY_TYPE_LINK_SUPPORT = 12, /* ��¼�豸�Ƿ�֧������ */
	MC_KEY_TYPE_SERVER_WANIP = 13,/*ͨ��serverid��ȡserverwanip*/
	MC_KEY_TYPE_HOME_INFO = 14,/*ͨ��homeid��ȡhome��Ϣ*/
	MC_KEY_TYPE_LANSRV_INFO = 15,/*ͨ��sn��ȡlansrv��Ϣ*/
	MC_KEY_TYPE_SLAVE_INFO = 16,/*ͨ��slave_sn��ȡ���豸��Ϣ�������������������ϱ�*/
	MC_KEY_TYPE_DEV_MAX_INDEX = 17,/*ͨ��sn���豸�ṩ�����ͣ���ȡ��ʷ��¼�е����ֵ*/
	MC_KEY_TYPE_DEV_HISTORY = 18,/*ͨ��sn���豸���ͺ�������������ʷ��¼*/
	MC_KEY_TYPE_CACHE_ST = 19,/*����(wifi�豸��rf���豸)�豸״̬*/
	MC_KEY_TYPE_DEV_MISC = 20,/*ͨ��sn�������豸����������*/
	MC_KEY_TYPE_RFDEV_LIST = 21,/*����macbee���豸�б���Ϣ*/
	MC_KEY_TYPE_DEV_TOTAL = 22,/*�����豸����*/
	MC_KEY_DEV_SNAPSHOT = 23,/*�賿�����豸����*/
	MC_KEY_TYPE_SRV = 24, /* server�����Ϣ���� */
	MC_KEY_TYPE_XSLOCK = 25, /* xslock�����Ϣ���� */
	MC_KEY_TYPE_AUTOBIND = 26, /*�����Զ������豸��Ϣ*/
	MC_KEY_TYPE_DOORBELL = 27,/* doorbell */
	MC_KEY_TYPE_SMS = 28,/* �����豸�澯��� */
	MC_KEY_TYPE_SN_AS = 29,/* �豸��AS���� */
	MC_KEY_TYPE_SLAVEINFO = 30,/* ���豸״̬��Ϣ�����������ϱ���ժҪ�����豸�б��ѯ  mc_slave_dev_t*/
	MC_KEY_TYPE_SLAVELIST = 31,/* �����µ����豸�������·��ֵ����豸 mc_slave_list_t */
} MC_KEY_TYPE;

typedef enum {
    MC_KEY_TYPE_XSLOCK_AUTH = 1, /* xslock auth */
	MC_KEY_TYPE_XSLOCK_DEV , /* xslock dev */
	MC_KEY_TYPE_XSLOCK_TOKEN, /* xslock token */
}MC_KEY_SUBTYPE_XSLOCK;

#pragma pack(push,1)

typedef struct {
	// ���汾��
	u_int8_t major;
	// �ΰ汾��
	u_int8_t minor;
	// �޶��汾��
	u_int8_t revise;
	u_int8_t reserved;
} version_t;

typedef struct {
	version_t stm_ver;
	version_t stm_hard_ver;
	version_t hard_ver;	
	u_int8_t pad[2];
} udp_t;

typedef struct {
	u_int8_t unbind_login;
	u_int8_t pad[1];	
} tcp_t;

typedef struct {
	u_int64_t mastersn;
	u_int32_t ip;						//area����ͨ��ip��ѯ����������岻��
	u_int32_t online_time;
	u_int32_t offline_time;	
	version_t soft_ver;
	u_int16_t vender_id; 				//����ͨ��mc_get_vender_str�ӿ�ȡ��vender_str
	u_int8_t devserver_id;				//idΪ0��ʾ���ߣ�����Ϊ��Ӧ��devserver
	u_int8_t dev_type;					//�豸����
	u_int8_t ext_type;					//��չ����
	u_int8_t proto_type; 				//��������0 udp 1 tcp
	union 								//udp��tcp���컯״̬���ݣ�����proto_type���ֶ�Ӧ�ṹ�塣
	{
		udp_t udp;
		tcp_t tcp;
	} u;
} mc_devinfo_t;

/* uc device structure */
typedef struct{
	u_int64_t sn;
	u_int32_t online;
	u_int32_t devserver_id;
	u_int32_t updatetime;
}mc_tcpdev_online_t;

/* uc device structure */
typedef struct{
	u_int64_t homeserver_sn;
	u_int32_t homeserver_devserverid;
	u_int32_t homeserver_lanip;
}mc_home_info_t;
typedef struct{
	u_int8_t rsv;
	u_int8_t log[8];
}dev_log_t;
typedef struct{
	//���豸���
	u_int8_t online;		// 1 ���ߣ�0����
	u_int32_t time;			//���һ������/����ʱ��
	ucp_dev_info_t dev_info;//���豸���͡��汾��Ϣ
	//���豸���
	u_int64_t master_sn;	//���豸sn
	u_int32_t udpserver_id;//���豸����udpserver_id
}mc_slave_info_t;

typedef struct{
	u_int8_t digest;	//���ժҪ/ttcache��cache_count
	u_int8_t pad[3];
}mc_cached_st_head_t;

typedef struct{
	u_int8_t digest;	//���ժҪ/ttcache��cache_count
	u_int8_t pad[3];
	u_int8_t data[256];//ttcache/������,�䳤
}mc_cache_st_t;

typedef struct{
	u_int32_t  timestamp;
	u_int32_t  total;
}mc_dev_statcs_t;

typedef struct{
	u_int32_t  timestamp;
	u_int32_t  count;
	u_int64_t  sn[0];
}mc_onlinedev_t;

typedef struct{
	u_int32_t  user_id;
    u_int64_t cas;
}mc_linksrv_userid_t;

typedef struct{
	u_int32_t  home_id;
    u_int64_t cas;
}mc_linksrv_homeid_t;


typedef struct{
	u_int8_t token[16];
    u_int8_t cipher[16];
}mc_xslock_auth_t;

/* gw slave dev auto bind info  */
typedef struct{
	u_int64_t gw_sn;
	u_int32_t time;
}mc_auto_bind_t;


typedef struct{
	u_int16_t asid;				
    u_int8_t pad[2];
    u_int64_t gw_sn;
}ucp_mc_as_t;

//���豸״̬��Ϣ
typedef struct {
	//status
	u_int32_t uptime;
	u_int16_t dev_id;
	u_int8_t dev_count;
	u_int8_t cache_count;
	u_int8_t flag;
	//sn type
	u_int64_t slavesn;
	u_int64_t gwsn;		//���豸δ��ʱ��gwsn = 0
	u_int8_t subtype;
    u_int8_t extype;
	//version
	u_int8_t rf_major;
	u_int8_t rf_minor;
	u_int8_t rf_revsion;
	u_int8_t app_major;
	u_int8_t app_minor;
	u_int8_t app_revsion;
	//ģ��汾
	u_int8_t tmplt_ver; 
}mc_slave_dev_t;

//�������豸�б���Ϣ(���·���δ�󶨵����豸)
typedef struct {
	u_int32_t uptime;
	u_int64_t gwsn;
	u_int16_t slave_count;
	u_int8_t pad[2];
	u_int64_t slavelist[0];	//δ�󶨵����豸sn���ܻ�����ڶ�����ص����豸�б���
}mc_slave_list_t;


#pragma pack(pop)

//��ʼ��memcached ʹ��memcached_free�ͷ�memcached��Դ ����get�����ú�ʹ��free�����ͷ��ڴ�
memcached_st* memcached_init();

//��ȡserverid��Ӧserver_wanip 
int mc_set_server_wanip(memcached_st *mc, u_int16_t id, u_int32_t ip);
u_int32_t mc_get_server_wanip(memcached_st *mc, u_int16_t id);

//��ȡserverid��Ӧserver_lanip 
int mc_set_server_lanip(memcached_st *mc, u_int16_t id, u_int32_t ip);
u_int32_t mc_get_server_lanip(memcached_st *mc, u_int16_t id);


//�����Ҫд�����ݿ��sn����sn_array
int mc_add_sn_arry(memcached_st *mc, u_int64_t sn);

//��ȡsn_array ������
u_int64_t* mc_get_clear_sn_array(memcached_st *mc, u_int32_t *count);

//���к�+ԭʼ���Ļ��浽memcached
int mc_cache_packet(memcached_st *mc, const char *ptr, int len);

//����tcp�豸��������Ϣ���ṩ��dispatcher
mc_tcpdev_online_t* mc_get_tcponline(memcached_st *mc, u_int64_t sn);
int mc_set_tcponline(memcached_st *mc, u_int64_t sn, mc_tcpdev_online_t *mc_devinfo);

//�����豸�Ƿ�֧������
int mc_set_link_support(memcached_st *mc, u_int64_t sn, ucp_dev_online_t *mc_link);
ucp_dev_online_t *mc_get_link_support(memcached_st *mc, u_int64_t sn);

//�����豸��������Ϣ
int mc_set_dev_misc(memcached_st *mc, u_int64_t sn, ucp_dev_misc_t *misc);
ucp_dev_misc_t *mc_get_dev_misc(memcached_st *mc, u_int64_t sn);


//�����ͥ��Ϣ
int mc_set_home_info(memcached_st *mc, u_int32_t home_id, mc_home_info_t *homeinfo);
mc_home_info_t* mc_get_home_info(memcached_st *mc, u_int32_t home_id);

//����lansrv��Ϣ
int mc_set_lansrv_info(memcached_st *mc, u_int64_t sn, u_int32_t lanip);
u_int32_t mc_get_lansrv_info(memcached_st *mc, u_int64_t sn);
int mc_set_history_index(memcached_st *mc, u_int64_t sn, u_int8_t type, u_int32_t max_index);
u_int32_t mc_get_history_index(memcached_st *mc, u_int64_t sn, u_int8_t type);
int mc_set_history_context(memcached_st *mc, u_int64_t sn, u_int8_t type, u_int32_t index, dev_log_t history);
dev_log_t*	mc_get_history_context(memcached_st *mc, u_int64_t sn, u_int8_t type, u_int32_t index);

/********************************** ����/wifi�豸�����豸�����豸�б����Ϣ���� **********************************/
//	MC_KEY_TYPE_SLAVEINFO = 30,/* ���豸״̬��Ϣ�����������ϱ���ժҪ�����豸�б��ѯ */
//	MC_KEY_TYPE_SLAVELIST = 31,/* �����µ����豸�������·��ֵ����豸 */

//����/wifi�豸(��udp/tcp�豸)��Ϣ����, json��ʽ
int mc_set_devinfo(memcached_st *mc, u_int64_t sn, uc_dev_info_t *mc_devinfo);
uc_dev_info_t* mc_get_devinfo(memcached_st *mc, u_int64_t sn);
void mc_delete_devinfo(memcached_st *mc, u_int64_t sn);

//���豸��Ϣ����
int mc_set_slave_dev(memcached_st *mc, mc_slave_dev_t *slave);
mc_slave_dev_t * mc_get_slave_dev(memcached_st *mc, u_int64_t sn);

//���豸�б�
int mc_set_slave_list(memcached_st *mc, mc_slave_list_t *list);
mc_slave_list_t * mc_get_slave_list(memcached_st *mc, u_int64_t sn);



//�������豸��Ϣ
int mc_set_slave_info(memcached_st *mc, u_int64_t slave_sn, mc_slave_info_t * slave_info);
mc_slave_info_t *mc_get_slave_info(memcached_st *mc, u_int64_t slave_sn);

//�����豸�б���Ϣ
int mc_set_rfdev_info(memcached_st *mc, u_int64_t gwsn, u_int16_t devid, u_int16_t len, u_int8_t *rfdev);
u_int8_t  *mc_get_rfdev_info(memcached_st *mc, u_int64_t gwsn, u_int16_t devid, u_int16_t *len);



//���������(tt_cache)
int mc_set_cache_st(memcached_st *mc, u_int64_t sn, u_int16_t index, u_int16_t len, mc_cache_st_t *st);
mc_cache_st_t *mc_get_cache_st(memcached_st *mc, u_int64_t sn, u_int16_t index, u_int16_t *len);

//���������豸����
int mc_set_dev_total(memcached_st *mc, char *srv_name, mc_dev_statcs_t * statcs);
//ÿ���賿�����豸����
int mc_set_dev_snapshoot(memcached_st *mc, char *srv_name, mc_onlinedev_t * devlist, u_int32_t len);

//linkserver�Ļ�������
mc_linksrv_userid_t *mc_get_linksrv_userid(memcached_st *mc);
int mc_set_linksrv_userid(memcached_st *mc, mc_linksrv_userid_t *srv);
int mc_set_linksrv_userid_cas(memcached_st *mc, mc_linksrv_userid_t *srv);

mc_linksrv_homeid_t *mc_get_linksrv_homeid(memcached_st *mc);
int mc_set_linksrv_homeid(memcached_st *mc, mc_linksrv_homeid_t *srv);
int mc_set_linksrv_homeid_cas(memcached_st *mc, mc_linksrv_homeid_t *srv);

//xslock auth
int mc_set_xslock_auth(memcached_st *mc, u_int64_t lock_sn, mc_xslock_auth_t *auth);
mc_xslock_auth_t *mc_get_xslock_auth(memcached_st *mc, u_int64_t lock_sn);


//���������Զ������豸��Ϣ
int mc_set_auto_bind(memcached_st *mc, u_int64_t slave_sn, mc_auto_bind_t *autobind);
mc_auto_bind_t* mc_get_auto_bind(memcached_st *mc, u_int64_t slave_sn);

int mc_set_doorbell(memcached_st *mc, u_int64_t sn, ucp_mc_doorbell_t *misc);
ucp_mc_doorbell_t *mc_get_doorbell(memcached_st *mc, u_int64_t sn);

int mc_set_dev_as(memcached_st *mc, u_int64_t sn, ucp_mc_as_t *as, time_t expired);
ucp_mc_as_t *mc_get_dev_as(memcached_st *mc, u_int64_t sn);

char *mc_get(memcached_st *mc, char *key, size_t key_length, size_t *value_length);
int mc_set(memcached_st *mc, char *key, size_t key_length, 
    char *value, size_t value_length,
    time_t expiration, u_int32_t flags);
int mc_cas(memcached_st *mc, char *key, size_t key_length, 
    char *value, size_t value_length,
    time_t expiration, u_int32_t flags, 
    u_int64_t cas);

#endif
