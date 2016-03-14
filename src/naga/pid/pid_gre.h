#ifndef __GRE_H_
#define __GRE_H_

typedef enum
{
	GTP_MSG_ECHO_REQ =1,
	GTP_MSG_ECHO_RSP=2,
	GTP_MSG_CREATE_SES_REQ=32,
	GTP_MSG_CREATE_SES_RSP=33,
	GTP_MSG_DELETE_SES_REQ=34,
	GTP_MSG_DELETE_SES_RSP=36,
	GTP_MSG_MODIFY_BEARER_REQ=34,
    GTP_MSG_MODIFY_BEARER_RSP=35,
	GTP_MSG_CHANGE_NOTIFICATION_REQ=38,
	GTP_MSG_CHANGE_NOTIFICATION_RSP=39,
	GTP_MSG_CREATE_BEARER_REQ=95,
    GTP_MSG_CREATE_BEARER_RSP=96,
    GTP_MSG_PDU=0xff,
    GTP_MSG_MAX,
}gtp_msg_type_t;

struct gre_hdr
{
//#if defined(__BIG_ENDIAN_BITFIELD)
#if defined(__LITTLE_ENDIAN_BITFIELD)
	uint8_t npdu:1;
	uint8_t sequence_num:1;
    uint8_t next_header:1;
    uint8_t reserv:1;
	uint8_t protype:1;    
	uint8_t vesion:3;
//#elif defined(__LITTLE_ENDIAN_BITFIELD)
#elif defined(__BIG_ENDIAN_BITFIELD)
	uint8_t vesion:3;
	uint8_t protype:1;
	uint8_t reserv:1;
	uint8_t next_header:1;
	uint8_t sequence_num:1;
	uint8_t npdu:1;
#endif
	uint8_t message_type;
	uint16_t length;
	uint32_t teid;
};

struct gtpv2_c_hdr
{

#if defined(__LITTLE_ENDIAN_BITFIELD)
	uint8_t resv:3;
	uint8_t Teid:1;
	uint8_t pigpack:1;    
    uint8_t vesion:3;
#elif defined(__BIG_ENDIAN_BITFIELD)
	uint8_t vesion:3;
	uint8_t pigpack:1;
	uint8_t Teid:1;
	uint8_t resv:3;
#endif
	uint8_t message_type;
	uint16_t length;
	uint32_t teid;  
};



#define GRE_VERSION(grehdr) (grehdr->vesion)
#define GRE_PRO(grehdr) 	(grehdr->protype)
#define GRE_VERSION(grehdr) (grehdr->vesion)
#define GRE_NEXT_HDR_FG(grehdr) (grehdr->next_header)
#define GRE_SEQ_NUM_FG(grehdr)  (grehdr->sequence_num)
#define GRE_NPDU_FG(grehdr)     		(grehdr->npdu)

berr pid_gtpu(struct pbuf *p, hytag_t *hytag);
berr pid_gtpv2c(struct pbuf *p, hytag_t *hytag);

#define GRE_HEAD_LEN  8


#endif
