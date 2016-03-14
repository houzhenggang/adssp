#include "pid.h"



berr pid_gtpu(struct pbuf *p, hytag_t *hytag)
{
	struct gre_hdr *grehdr;
	int len = GRE_HEAD_LEN;
    uint8_t *next_hdr=0;
    uint8_t *next_hdr_len=0;
    
	if(check_pbuf_len(p, GRE_HEAD_LEN))
	{
		pid_incr_count(GTP_U_HD);//drop packet and incr counter, then return;
		BRET(E_EXCEED);
	}

	PBUF_CUR_FORMAT(struct gre_hdr *, grehdr, p);	

    
	if(GRE_VERSION(grehdr) != GREV1 
		|| GRE_PRO(grehdr) != GRE_GTP)
	{
		pid_incr_count(GTP_U_HD);
		BRET(E_FAIL);
	}
	hytag->teid = ntohl(grehdr->teid);

    pid_incr_count(GTP_U);
    
	if(GRE_NEXT_HDR_FG(grehdr)
		||GRE_SEQ_NUM_FG(grehdr)
		||GRE_NPDU_FG(grehdr))
	{
		len	+= 4;//expend 
		if (unlikely(GRE_NEXT_HDR_FG(grehdr)))
		{
            //PBUF_CUR_FORMAT(struct gre_hdr *, grehdr, p);
            PBUF_OFF_FORMAT(uint8_t *, next_hdr,p, len);
            len +=1;
            while(*next_hdr != 0) 
            {
                 PBUF_OFF_FORMAT(uint8_t *, next_hdr_len, p, len);
                 len += (*next_hdr_len)*4;
                 len -=1;
                 PBUF_OFF_FORMAT(uint8_t *, next_hdr,p,  len);
            }
            //FIXME:skip next header    
            len += 1;
		}
      
	}
    /*check Message Type , When The port =2152 without Payload, eg, echo Msg*/
    switch(grehdr->message_type)
    {
        case GTP_MSG_PDU:
            UPDATE_PBUF_OFFSET(p, len);
	        return pid_innerip(p, hytag);
            break;
        case GTP_MSG_ECHO_REQ:
        case GTP_MSG_ECHO_RSP:    
        default:
            pid_incr_count(GTP_U_NOPAYLOAD);
            break;
    }    
    return E_SUCCESS;
}


berr pid_gtpv2c(struct pbuf *p, hytag_t *hytag __attribute__ ((unused)))
{
	struct gtpv2_c_hdr *gtpc_hdr __attribute__ ((unused));
	int len = GRE_HEAD_LEN;

    
	if(check_pbuf_len(p, GRE_HEAD_LEN))
	{
		;//drop packet and incr counter, then return;
		BRET(E_EXCEED);
	}

	PBUF_CUR_FORMAT(struct gtpv2_c_hdr *, gtpc_hdr, p);	


    UPDATE_PBUF_OFFSET(p, len);
    pid_incr_count(GTPV2_C);
    
    return E_SUCCESS;
}


